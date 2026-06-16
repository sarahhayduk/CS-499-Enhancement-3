#include "Database.h"
#include "Log.h"
#include "CryptoUtils.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>

using namespace std;
namespace fs = filesystem;

/*--------------------------------------------------------------------------------
 * Database.cpp
 *
 * This module centralizes all database operations for the Client Service Management
 * System. It provides a clean separation between UI logic and persistent storage by
 * encapsulating all SQLite operations. This design ensures predictable behavior,
 * secure handling of sensitive information, and a maintainable structure that
 * supports future expansion.
 *
 * All SQL operations use prepared statements with bound parameters (sqlite3_bind_*)
 * This ensures user input is always treated as literal data rather an executable SQL.
 * Because SQL commands are never constructed through string concatenation, malicious
 * input cannot alter the query structure, it is safely stored as text instead of
 * being executed. This design fully mitigates SQL injection vulnerabilities.
 * 
 * Internal errors are routed to the diagnostics log to avoid exposing sensitive 
 * details to the user interface, replacing cout with internal log supports secure 
 * defensive error handling practices.
 * 
 * The SQLite connection is maintained as a single pointer so that all database-
 * related functions share the same context. This keeps the structure simple while
 * preserving clear separation of concerns.
 * ------------------------------------------------------------------------------*/
sqlite3* db = nullptr;

/*---------------------------------------------------------------------------------
 * Smart Pointer
 * 
 * Prepared statement resources are automatically cleaned up using RAII smart 
 * wrappers with sqlite3_finalize as a custom deleter. This structurally ensures
 * resource cleanup even if runtime exceptions interrupt standard execution flow and
 * prevents raw sqlite3_stmt leaks.
 ---------------------------------------------------------------------------------*/
using StmtPtr = unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

/*--------------------------------------------------------------------------------
 * ExecuteSqlFile
 *
 * This function reads the external SQL file containing sensitive data and executes
 * it at runtime. Keeping this data in an external file prevents it from being
 * compiled into the binary, which protects it against reverse engineering tools
 * that can extract string literals. The SQL file is only executed when the database
 * is first created.
 * -------------------------------------------------------------------------------*/
bool ExecuteSqlFile(const string& filename) {
    ifstream file(filename);

    // If the seed file cannot be opened, log error but do not crash
    if (!file.is_open()) {
        LogError("Could not open seed file: " + filename);
        return false;
    }

    string sql;
    string line;

    // Read the entire SQL file into a single string
    while (getline(file, line)) {
        sql += line + "\n";
    }

    char* errMsg = nullptr;

    // Execute the SQL script in one call
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        // errMsg in the case is char, so its made a string
        LogError(string("SQL file execution failed: ") + errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------------
 * InitDatabase
 *
 * This function detects if the database already exists, creates the schema
 * automatically when needed, and executes the external seed.sql file only on the
 * first run. This design ensures the system initializes itself while keeping all
 * sensitive literals out of the executable, preventing exposure through reverse
 * engineering.
 * -------------------------------------------------------------------------------*/
bool InitDatabase() {
    bool dbExists = fs::exists("clients.db");

    // Open or create the SQLite database file
    int rc = sqlite3_open("clients.db", &db);
    if (rc != SQLITE_OK) {
        LogError(string("Cannot open database: ") + sqlite3_errmsg(db));
        return false;
    }

    // Database schema: Clients table, contains no sensitive data
    const char* sqlCreateClients =
        "CREATE TABLE IF NOT EXISTS Clients ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "service INTEGER NOT NULL"
        ");";

    // Execute Clients table creation
    rc = sqlite3_exec(db, sqlCreateClients, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        LogError("Failed to create Clients table.");
        return false;
    }

    // Database schema: Users table, contains no sensitive data
    // Edit user table to use password_hash and salt
    const char* sqlCreateUsers =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY,"
        "password_hash BLOB NOT NULL,"
        "salt BLOB NOT NULL"
        ");";

    // Execute Users table creation
    rc = sqlite3_exec(db, sqlCreateUsers, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        LogError("Failed to create Users table.");
        return false;
    }

    // Only seed if the DB does not exist before this run
    if (!dbExists) {
        // this cout is info rather than error so I left it for now
        cout << "[Info] Preparing system for first use. Running initial seed..." << endl;
        if (!ExecuteSqlFile("seed.sql")) {
            LogError("Failed to execute seed.sql during initialization.");
            return false;
        }
    }
    return true;
}

/*---------------------------------------------------------------------------------
 * GetPassword
 *
 * This function retrieves the stored password hash and salt from the Users table.
 * This data is used by the login function to verify the user's input by hashing
 * it with the stored salt and comparing the result. Only one credential record
 * is expected.
 * -------------------------------------------------------------------------------*/
bool GetPassword(vector<uint8_t>& hash, vector<uint8_t>& salt) {
    // Select the stored Argon2id hash and salt from the Users table. Stored as Blobs, not text.
    const char* sql = "SELECT password_hash, salt FROM Users LIMIT 1;";
    sqlite3_stmt* raw = nullptr;

    // Prepare the SQL statement for execution
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        LogError(string("Failed to prepare password query: ") + sqlite3_errmsg(db));
        return false;
    }

    // Wrap raw sqlite3_stmt* in RAII smart pointer for automatic sqlite3_finalize
    StmtPtr stmt(raw, sqlite3_finalize);

    // sqlite3_step moves to the next row of the result set
    int rc = sqlite3_step(stmt.get());
    if (rc == SQLITE_ROW) {
        // Extract the stored password hash from column 0, returns pointer to raw bytes
        const void* hashBlob = sqlite3_column_blob(stmt.get(), 0);
        int hashSize = sqlite3_column_bytes(stmt.get(), 0);

        // Extract the stored salt from column 1, returns pointer to raw bytes
        const void* saltBlob = sqlite3_column_blob(stmt.get(), 1);
        int saltSize = sqlite3_column_bytes(stmt.get(), 1);

        // If either is missing, the database is incorrectly seeded
        if (!hashBlob || !saltBlob) {
            LogError("Password or salt blob was null.");
            return false;
        }

        // Copy raw bytes into output vectors using assign to resize and copy safetly
        hash.assign((const uint8_t*)hashBlob, (const uint8_t*)hashBlob + hashSize);
        salt.assign((const uint8_t*)saltBlob, (const uint8_t*)saltBlob + saltSize);

        return true;
    }
    // No row returned or error
    return false;
}
/*---------------------------------------------------------------------------------
 * LoadClients
 *
 * Loads all client records from the Clients table into the Client vector. This
 * allows the UI logic to work with a clean in-memory list of clients.
 * -------------------------------------------------------------------------------*/
bool LoadClients(vector<Client>& clients) {
    const char* sql = "SELECT id, name, service FROM Clients;";
    sqlite3_stmt* raw = nullptr;

    // Prepare the SELECT statement
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        LogError(string("Failed to prepare client load query: ") + sqlite3_errmsg(db));
        return false;
    }

    // Wrap raw sqlite3_stmt* in RAII smart pointer for automatic sqlite3_finalize
    StmtPtr stmt(raw, sqlite3_finalize);

    clients.clear();

    // Loop through each row returned by the query, add to client vector
    while (sqlite3_step(stmt.get()) == SQLITE_ROW) {
        Client c;
        c.id = sqlite3_column_int(stmt.get(), 0);
        c.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt.get(), 1));
        c.service = sqlite3_column_int(stmt.get(), 2);

        clients.push_back(c);
    }
    return true;
}


/*---------------------------------------------------------------------------------
 * AddClient
 *
 * This function inserts a new client record into the Clients table. The UI handles
 * input validation and passes clean values here.
 * -------------------------------------------------------------------------------*/
bool AddClient(const string& name, int service) {
    const char* sql = "INSERT INTO Clients (name, service) VALUES (?, ?);";
    sqlite3_stmt* raw = nullptr;

    // Prepare the INSERT statement
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        LogError(string("Failed to prepare add client statement: ") + sqlite3_errmsg(db));
        return false;
    }

    // Wrap raw sqlite3_stmt* in RAII smart pointer for automatic sqlite3_finalize
    StmtPtr stmt(raw, sqlite3_finalize);

    // Bind parameters to the SQL statement
    sqlite3_bind_text(stmt.get(), 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt.get(), 2, service);

    // Execute the INSERT
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        LogError("Failed to add new client.");
        return false;
    }
    return true;
}

/*---------------------------------------------------------------------------------
 * UpdateClient
 *
 * This function updates an existing client record in the Clients table. The UI
 * determines which client is being modified and passed the updated value here.
 * -------------------------------------------------------------------------------*/
bool UpdateClient(int id, const string& name, int service) {
    const char* sql = "UPDATE Clients SET name = ?, service = ? WHERE id = ?;";
    sqlite3_stmt* raw = nullptr;

    // Prepare the UPDATE statement
    if (sqlite3_prepare_v2(db, sql, -1, &raw, nullptr) != SQLITE_OK) {
        LogError(string("Failed to prepare update statement: ") + sqlite3_errmsg(db));
        return false;
    }

    // Wrap raw sqlite3_stmt* in RAII smart pointer for automatic sqlite3_finalize
    StmtPtr stmt(raw, sqlite3_finalize);

    // Bind new values to SQL statement
    sqlite3_bind_text(stmt.get(), 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt.get(), 2, service);
    sqlite3_bind_int(stmt.get(), 3, id);

    // Execute the UPDATE
    if (sqlite3_step(stmt.get()) != SQLITE_DONE) {
        LogError("Failed to update client.");
        return false;
    }

    return true;
}

/*---------------------------------------------------------------------------------
 * CloseDatabase
 *
 * This function closes the database and performs cleanup to prevent resouce leaks
 * and keep ownership consistent.
 * -------------------------------------------------------------------------------*/
void CloseDatabase() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr;
    }
}
