#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "sqlite3.h"

/*---------------------------------------------------------------------------------
 * Database.h
 *
 * This header defines the public interface for all database operations by the
 * program. It exposes the Client struct, which mirrors the schema of the Clients
 * table, and declares the functions responsible for database initialization, data
 * loading, record insertion, updates, and cleanup.
 * -------------------------------------------------------------------------------*/
struct Client {
    int id;
    std::string name;
    int service;
};

bool InitDatabase();
bool ExecuteSqlFile(const std::string& filename);
bool GetPassword(std::vector<uint8_t>& hash, std::vector<uint8_t>& salt);
bool LoadClients(std::vector<Client>& clients);
bool AddClient(const std::string& name, int service);
bool UpdateClient(int id, const std::string& name, int service);
void CloseDatabase();

#endif
