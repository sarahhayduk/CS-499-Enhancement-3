/*------------------------------------------------------------------------
 * seed.sql
 *
 * This file contains the initial client records and the system password.
 * These values are intentionally stored outside the C++ source code to
 * prevent from being compiled into the binary. Keeping sensitive data
 * external ensures that reverse engineering tools cannot extract client
 * names and password for the executable.
 *
 * The program loads and executes this SQL files only when the database
 * does not yet exist.
 -------------------------------------------------------------------------*/

 INSERT INTO Clients (name, service) VALUES
 ('Bob Jones', 1),
 ('Sarah Davis', 2),
 ('Amy Friendly', 1),
 ('Johnny Smith', 1),
 ('Carol Spears', 2);

 INSERT INTO Users (id, password_hash, salt) VALUES (1, X'E3F4F900FEA4FC91F0F472B93EB58B0EB5AAF0BA702C16F8A10680EACAC9A77D', X'2923BE84E16CD6AE529049F1F1BBE9EB');


