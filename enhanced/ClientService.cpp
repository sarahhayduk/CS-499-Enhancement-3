// ClientService.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include <string>
#include "Database.h"
#include "Menu.h"
#include "CryptoUtils.h"

/*---------------------------------------------------------------------------------
 * ClientService.cpp
 *
 * This module contains the entry point and high-level control flow for the Client
 * Service Management System. It is responsible for initializing the database,
 * handling authentication, loading inital data, and delegating all menu-related
 * operations to the Menu module. SSeparating the menu system from this file improves
 * clarity and maintainability.
 * 
 * All database-tier erros are routed to the internal diagnostics log through the
 * Database module. Any failures communicated in this layer to the UI are generic
 * messages to prevent exposure of sensitive operational details.
 * -------------------------------------------------------------------------------*/

using namespace std;

/*---------------------------------------------------------------------------------
 * CheckUserPermissionAccess
 *
 * This function retrieves the stored password hash and salt from the database, 
 * hashes the user's input, and compared the two values. This completely removes the
 * hardcoded password vulnerability. Returns 1 for success and 2 for failure. 
 * -------------------------------------------------------------------------------*/
int CheckUserPermissionAccess() {

    // Vectors hold stored Argon2id hash and salt pulled form DB
    vector<uint8_t> storedHash;
    vector<uint8_t> storedSalt;

    // Load the hash and salt from the database
    if (!GetPassword(storedHash, storedSalt)) {
        cout << "[Error] Database operation failed." << endl;
        return 2; // treat as failure
    }

    string username;
    string inputPassword;

    cout << "Enter your username: ";
    cin >> username;

    cout << "Enter your password: ";
    cin >> inputPassword;

    // Input validation to prevent malformed input from bypassing logic
    if (cin.fail()) {
        cin.clear();
        cin.ignore(1000, '\n');
        return 2;
    }

    // Hash the user input with the same stored salt as when the password was created
    vector<uint8_t> inputHash = hashPassword(inputPassword, storedSalt);

    // Compare the computed hash to the stored hash
    if (inputHash == storedHash) {
        return 1; // success
    }
    return 2; // failure
}

/*---------------------------------------------------------------------------------
 * main
 *
 * Entry point. Initializes the DB, handles user authentication, loads initial
 * client list, and calls the menu.
 * -------------------------------------------------------------------------------*/
int main()
{
    // Database initialization output
    cout << "Initializing..." << endl;

    if (!InitDatabase()) {
        cout << "[Error] Unable to initialize system. Exiting." << endl;
        return 1;
    }

    cout << "Initialization successful.\n" << endl;

    // Program output
    cout << "Hello! Welcome to our Investment Company\n";

    // Loop until password is correct
    int answer = CheckUserPermissionAccess();
    while (answer != 1) {
        cout << "[Invalid Password] Please try again.\n";
        answer = CheckUserPermissionAccess();
    }

    vector<Client> clients;
    LoadClients(clients);
    DisplayMenu(clients);

    // Close resources when done
    CloseDatabase();
    return 0;
}
