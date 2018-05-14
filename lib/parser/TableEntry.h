#ifndef TableEntry_H
#define TableEntry_H

#include "Production.h"

class TableEntry
{
private:
  bool sync;
  Production production;

public:
  TableEntry();
  Production getProduction() const;
  bool isSync() const;
  TableEntry(Production, bool isSync);
};
#endif //TableEntry_H