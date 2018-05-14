#ifndef convertRulesToNFA_H
#define convertRulesToNFA_H

#include <string>
#include "NFATransitionTable.h"

NFATransitionTable convertRulesToNFA(const std::string &filename);

#endif // convertRulesToNFA_H
