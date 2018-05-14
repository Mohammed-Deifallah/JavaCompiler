#ifndef writeErrorLogToFile_H
#define writeErrorLogToFile_H

#include <string>
#include <fstream>
#include "ErrorLog.h"

void writeErrorLogToFile(const ErrorLog &errorLog, const std::string &filename);

#endif // writeErrorLogToFile_H
