#ifndef TableCreator_H
#define TableCreator_H

#include "CFG.h"
#include "Symbol.h"
#include <set>
#include "LL1Table.h"

class TableCreator {
private:
  CFG cfg;
  std::map<Symbol, std::set<Symbol>> firstMap;
  std::map<Symbol, std::set<Symbol>> fellowMap;
  void findFirstSet();
  void findFellowSet();
  std::set<Symbol> findFirstRec(Symbol);
  std::set<Symbol> findFellowRec(Symbol, std::map<Symbol, bool> visit);
  std::set<Symbol> findRhsFirst(std::vector<Symbol> rhs);

public:
  TableCreator(CFG);
  LL1Table createTable();
};

#endif // TableCreator_H