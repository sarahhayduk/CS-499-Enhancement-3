#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <vector>
#include <string>
#include <cstdint>

extern "C" {
#include "monocypher.h"
}

/*---------------------------------------------------------------------------------
 * CryptoUtils.h
 *
 * This header defines the public interface for all cryptographic operations used by
 * the Client Service Management System. It exposes the functions responsible for 
 * generating secure random salts and hashing passwords using the Argon2id algorithm.
 * Centralizing cryptographic logic in this module keeps the design clean.
 * -------------------------------------------------------------------------------*/

// Generate a 16-byte random salt
std::vector<uint8_t> generateSalt();

// Hash a password using Argon2id
std::vector<uint8_t> hashPassword(const std::string& password, const std::vector<uint8_t>& salt);

/*---------------------------------------------------------------------------------
 * TEMPORARY: PrintSeedValues() - Used only to generate seed.sql values **
 * This function was placed in main to print the hash and salt hexes for the original 
 * password. The output was copied into seed.sql and the function itself has been 
 * deleted from CryptoUtils.cpp to ensure the password string was completely removed 
 * from the project.
 * -------------------------------------------------------------------------------*/
//void PrintSeedValues();

#endif