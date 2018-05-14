#include "TableEntry.h"

TableEntry::TableEntry() = default;

TableEntry::TableEntry(Production prod, bool isSync)
    : production(prod), sync(isSync) {}

Production TableEntry::getProduction() const
{
    return production;
}
bool TableEntry::isSync() const
{
    return sync;
}
