#ifndef Production_H
#define Production_H

#include "Symbol.h"
#include <vector>
#include <iostream>

class Production
{
private:
  Symbol nonTerm;
  std::vector<Symbol> production;

public:
  Production();
  Production(Symbol, std::vector<Symbol>);
  std::vector<Symbol> rhs() const;
  Symbol lhs() const;
  bool operator==(const Production &s) const;

};
#endif //Production_H
