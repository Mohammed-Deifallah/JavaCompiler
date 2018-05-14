#ifndef CFG_H
#define CFG_H

#include "Symbol.h"
#include <map>
#include <vector>
#include "Production.h"
#include <set>

class CFG
{
public:
    Symbol getStartSymbol()const;
    void setStartSymbol(Symbol symbol);

  CFG();

  std::vector<Symbol> getTerminals() const;

  std::vector<Symbol> getNonTerminals() const;

  explicit CFG(const std::string &file_name);

  // return all occurrences of the symbol.
  std::vector<Production> getProductions(const Symbol &) const;

  std::vector<Production> getAllProductions() const;

  void addProduction(Production);

  void deleteProduction(Production);

  CFG &operator=(const CFG &other);

private:
  void parsing_file_into_rules(std::string file);

  std::vector<Symbol> symbols;
  Symbol start_symbol;
  std::vector<Production> allProduction;
  std::set<Symbol> terminal;
  std::set<Symbol> nonTerminal;

  void parsing_rules_into_symbol(std::string &rule, bool start);
};

#endif //CFG_H
