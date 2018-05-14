#ifndef LL1Table_H
#define LL1Table_H

#include <map>
#include <iostream>
#include <memory>
#include "Production.h"
#include "Symbol.h"
#include "CFG.h"
#include "TableEntry.h"

/**
 * An entry in table which specify the production and its state (syncronized or not).
 */

class LL1Table
{
public:
  LL1Table() {}
  /**
   * @arg the starting symbol in CFG.
   */
  LL1Table(Symbol &);

  void setStartSymbol(Symbol);
  Symbol getStartSymbol() const;
  /** 
  *   Returning the production corresponding to the giving nonterminal under the input (terminal).
  *   @return a pointer to a TableEntry struct. If Null then it means error.
  */
  TableEntry *getProduction(Symbol nonTerminal, Symbol terminal);
  std::map<Symbol, std::map<Symbol, TableEntry>> getTable();
  void addProduction(const Symbol nonTerminal, const Symbol terminal, const Production prod, const bool isSync);

private:
  Symbol start;
  std::map<Symbol, std::map<Symbol, TableEntry>> table;
  friend std::ostream &operator<<(std::ostream &out, const LL1Table &);
  friend std::istream &operator>>(std::istream &in, LL1Table &);
};

std::ostream &operator<<(std::ostream &out, const LL1Table &);
std::istream &operator>>(std::istream &in, LL1Table &);

#endif // LL1Tabel_H
