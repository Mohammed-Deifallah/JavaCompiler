#ifndef CFGMODIFICATION_H
#define CFGMODIFICATION_H

#include "CFG.h"

CFG removeLeftFactoring(const CFG &cfg);
CFG removeLeftRecursion(CFG &cfg);


CFG removeLeftRecursion2(const CFG &cfg);

#endif //CFGMODIFICATION_H
