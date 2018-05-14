#include "writeLL1Table.h"
#include <fstream>

void writeLL1Table(const LL1Table & table, const std::string & filename)
{
    std::ofstream file;
    file.open(filename);
    file << table;
    file.flush();
    file.close();
}
