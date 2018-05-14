#include "writeErrorLogToFile.h"

#include <iostream>

void writeErrorLogToFile(const ErrorLog &errorLog, const std::string &filename)
{
    std::ofstream file;
    file.open(filename);
    if (file.is_open())
    {
        file << errorLog;
    }
    else
    {
        std::cout << "Cannot open file " << filename << std::endl;
    }
}
