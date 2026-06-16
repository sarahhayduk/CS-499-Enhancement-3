#include "Log.h"
#include <fstream>
#include <ctime>

using namespace std;

/*---------------------------------------------------------------------------------
 * Log.cpp
 *
 * This module implements the internal diagnostic logging for the program. It routes 
 * timestamped database-tier and system-tier failures to an internal diagnostics 
 * log file, presenting generic, non-descriptive error responses to the UI layer to
 * prevent information disclosure weaknesses. Note* a db_errors.log file will only
 * be created if an error occurs; during normal execution, this fille will not exist.
 * -------------------------------------------------------------------------------*/

void LogError(const string& message) {
	ofstream log("db_errors.log", ios::app);

	if (!log.is_open()) {
		return; // fail silently
	}

	// Add timestamp for log file. ctime is deprecated -> using ctime_s
	time_t now = time(nullptr);
	char buffer[26]; // ctime_s requires 26 bytes
	ctime_s(buffer, sizeof(buffer), &now);

	log << buffer << " - " << message << "\n";
}