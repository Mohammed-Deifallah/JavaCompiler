#include "readTransitionTable.h"

#include <fstream>

DFATransitionTable readTransitionTable(const std::string &filename)
{
    DFATransitionTable transitionTable;
    std::ifstream file;
    file.open(filename);
    file >> transitionTable;
    file.close();
    return transitionTable;
}
