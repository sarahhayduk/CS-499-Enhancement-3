#include "CryptoUtils.h"
#include <cstdio>
#include <iostream>

using namespace std;

/*------------------------------------------------------------------------
 * CryptoUtils.cpp
 * 
 * This module provides the cryptographic functionality for the Client
 * Service Management System. It implements secure password hashing using
 * the Argon2id algorithm.
 * Passwords are never stored in plaintext. Each password is combined with
 * a unique 16-byte random salt and processed through Argon2id to produce a 
 * 32-byte cryptographic hash. Only the hash and salt are stored in the 
 * database. During login, the user's input is hashed again with the same 
 * salt and compared to the stored hash.
 * This design ensure passwords cannot be reversed or recovered, identical
 * passwords produce different hashes, and the system remains secure even
 * if the database is compromised.
 * 
 * This module is intentionally isolated so that all cryptographic operations
 * are centralized, maintainable, and easy to audit.
 * -------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* generateSalt
*
* Generates a 16-byte cryptographic random salt. A salt ensures that even if
* two users choose the same password, their resulting hashes will be different.
* This is for scalability as more users can be added to the database later.
*---------------------------------------------------------------------------*/
vector<uint8_t> generateSalt()
{
    // Allocate 16 bytes for the salt
    vector<uint8_t> salt(16);

    // Fill each byte with a random value (0-255)
    for (auto& b : salt) {
        b = static_cast<uint8_t>(rand() % 256);
    }
    return salt;
}

/*----------------------------------------------------------------------------
 * hashPassword
 * 
 * Hashes a plaintext password using the Argon2id algorithm. The caller provides
 * the password and its associated salt. The function returns a 32-byte hash.
 * 
 * Argon2id parameters:
 * - Memory cost: nb_blocks = 32 KB
 * - Time cost: nb_passes = 3 passes to slow brute force attempts
 * - Parallelism: nb_lanes = 1 lane to keep implementation simple
 * 
 * The workArea buffer provides Argon2 with the memory it needs to perform
 * internal mixing operations. The resulting hash is deterministic. The same
 * password + salt is always the same hash.
 *----------------------------------------------------------------------------*/
vector<uint8_t> hashPassword(const std::string& password,
    const vector<uint8_t>& salt)
{
    // Configure Argon2id parameters
    crypto_argon2_config config = {
        CRYPTO_ARGON2_ID, // use Argon2id algorithm
        32,               // memory hardness (32 KB)
        3,                // CPU hardness
        1                 // parallelism level
    };

    // Provide Argon2 with the password and salt inputs
    crypto_argon2_inputs inputs = {
        reinterpret_cast<const uint8_t*>(password.data()),  // password bytes
        salt.data(),                                        // salt bytes
        static_cast<uint32_t>(password.size()),             // password length
        static_cast<uint32_t>(salt.size())                  // salt length
    };

    // No additional Argon2 features used
    crypto_argon2_extras extras = crypto_argon2_no_extras;

    vector<uint8_t> hash(32);            // 32-byte hash
    vector<uint8_t> workArea(32 * 1024); // 32 KB work area

    // Perform the Argon2id hashing operation
    crypto_argon2(hash.data(),
        static_cast<uint32_t>(hash.size()), // output buffer
        workArea.data(),                    // working memory
        config,                             // Argon2 parameters
        inputs,                             // password + salt
        extras);                            // none

    // Return the computed hash
    return hash;
}
