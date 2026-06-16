#ifndef LOG_H
#define LOG_H

#include <string>

/*---------------------------------------------------------------------------------
 * Log.h
 *
 * This header defines the logging interface used by the program to record internal
 * diagnostic information. It exposes the function, LogError, which writes error 
 * details to an internal log file. This supports defensive error silencing by 
 * ensuring sensitive operational information is never displayed to the user 
 * interface while still preserving a record for debugging and maintenance.
 * -------------------------------------------------------------------------------*/
void LogError(const std::string& message);

#endif
