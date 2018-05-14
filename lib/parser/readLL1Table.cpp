#include "readLL1Table.h"
#include <fstream>

LL1Table readLL1Table(const std::string filename)
{
    LL1Table ll1Table;
    std::ifstream file;
    file.open(filename);
    file >> ll1Table;
    file.close();
    return ll1Table;
}
