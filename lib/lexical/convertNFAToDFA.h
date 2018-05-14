#ifndef convertNFAToDFA_H
#define convertNFAToDFA_H

#include "NFATransitionTable.h"
#include "DFATransitionTable.h"

DFATransitionTable convertNFAToDFA(const NFATransitionTable &nfa);

#endif // convertNFAToDFA_H
