#ifndef RulesReader_H
#define RulesReader_H

#include <string>
#include <vector>

#include "Rule.h"

class RulesReader
{
public:
    RulesReader();
    ~RulesReader();

    std::vector<Rule> process(const std::string &filename);
};

#endif // RulesReader_H