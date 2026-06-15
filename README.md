# Databases: Reverse-Engineered Client Service Management System (SQLite/C++)
This repository contains the original and enhanced versions of my Reverse-Engineered Client Service Management System. The artifact began as a CS-410 Software Reverse-Engineering project focused on disassembling a legacy `.o` binary and rebuilding it into modern C++. The original implementation was a single monolithic `.cpp`  file that hardcoded all client names, service selections, and the authentication password, leaving the program vulnerable to reverse‑engineering and insecure data exposure.

## Overview
For this enhancement, I transformed the system into a modular, database‑backed application using SQLite and strengthened its security posture. All previously embedded data was migrated into a relational schema, and the authentication mechanism was redesigned to eliminate plaintext credentials. The enhanced version uses Monocypher’s Argon2id implementation to generate salted password hashes, storing only the hash and salt in the database and verifying credentials securely at runtime.

The redesign also improves reliability and maintainability. Database operations now use parameterized SQL queries, error handling avoids leaking internal details to the user interface, and SQLite statement objects are wrapped in RAII‑style smart pointer constructs to guarantee cleanup even during exceptions. These changes reflect a more disciplined engineering approach aligned with secure software design principles.

## Key Improvements
- Migration from hardcoded data to a secure SQLite database
- Salted Argon2id password hashing using Monocypher
- Parameterized SQL queries to prevent injection and reduce reverse‑engineering risk
- Defensive error silencing to avoid exposing internal database details
- Automated memory cleanup using smart‑pointer wrappers for `sqlite3_stmt*`
- Modular separation of authentication, data access, and program logic

## Repository Structure
`/original`
The initial single‑file implementation from CS-410 Software Reverse-Engineering

`/enhanced`
Refactored version featuring SQLite integration, Argon2id authentication, and modular design

`/zips`
Full project ZIPs for SNHU submission (original and enhanced)

## Related Artifacts
Additional documentation, narrative, and demo video for this enhancement are available in my ePortfolio: https://sarahhayduk.github.io/

## Technoliges Used
- C++
- SQLite
- Argon2id (Monocypher) - https://github.com/LoupVaillant/monocypher
- Visual Studio

This enhancement demonstrates my ability to modernize insecure designs and implement secure, database‑driven architectures. It highlights growth in cryptographic hardening, defensive error handling, memory‑safe resource management, and the application of sound engineering practices.
