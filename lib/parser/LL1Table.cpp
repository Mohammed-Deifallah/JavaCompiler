#include "LL1Table.h"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <assert.h>

LL1Table::LL1Table(Symbol &s) : start(s) {}

void LL1Table::setStartSymbol(Symbol s)
{
    start = s;
}

Symbol LL1Table::
    getStartSymbol() const
{
    return start;
}

void LL1Table::addProduction(const Symbol nonTerminal, const Symbol terminal,
                             const Production prod, const bool isSync)
{
    TableEntry newEntry(prod, isSync);
    table[nonTerminal][terminal] = newEntry;
}

std::map<Symbol, std::map<Symbol, TableEntry>> LL1Table::getTable()
{
    return table;
}
TableEntry *LL1Table::getProduction(Symbol nonTerminal, Symbol terminal)
{
    std::map<Symbol, TableEntry>::iterator it = table[nonTerminal].find(terminal);
    if (it == table[nonTerminal].end())
        return NULL;
    return &(it->second);
}

std::ostream & operator<<(std::ostream & out, const LL1Table &table)
{
    using namespace rapidjson;
    Document d;
    d.SetObject();
    Document::AllocatorType& allocator = d.GetAllocator();

    std::map<std::string, Symbol> symbolDictionary;

    symbolDictionary[table.start.getValue()] = table.start;

    d.AddMember("start", table.start.getValue(), allocator);

    d.AddMember("tt", Value(), allocator);
    d["tt"].SetArray();
    for (const std::pair<Symbol, std::map<Symbol, TableEntry> > & tableRowPair : table.table)
    {
        Value row;
        row.SetObject();

        row.AddMember("__currentSymbol", tableRowPair.first.getValue(), allocator);

        symbolDictionary[tableRowPair.first.getValue()] = tableRowPair.first;

        for (const std::pair<Symbol, TableEntry> &inputResultPair : tableRowPair.second)
        {
            Value tableEntryValue;
            tableEntryValue.SetObject();
            tableEntryValue.AddMember("sync", inputResultPair.second.isSync(), allocator);

            Value productionValue;
            Production p = inputResultPair.second.getProduction();
            productionValue.SetObject();
            productionValue.AddMember("nonTerminal", p.lhs().getValue(), allocator);

            Value symbolStringValue;
            symbolStringValue.SetArray();
            auto symbols = p.rhs();
            for (auto &symbol : symbols)
            {
                symbolDictionary[symbol.getValue()] = symbol;
                std::string str = symbol.getValue();
                Value strVal;
                strVal.SetString(str.c_str(), str.length(), allocator);
                symbolStringValue.PushBack(strVal, allocator);
            }
            productionValue.AddMember("symbolString", symbolStringValue, allocator);

            tableEntryValue.AddMember("production", productionValue, allocator);

            std::string nameString(inputResultPair.first.getValue());
            Value name(nameString.c_str(), nameString.size(), allocator);
            row.AddMember(name, tableEntryValue, allocator);

            symbolDictionary[inputResultPair.first.getValue()] = inputResultPair.first;
        }

        d["tt"].PushBack(row, allocator);
    }

    d.AddMember("symbols", Value(), allocator);
    d["symbols"].SetArray();
    for (auto symbolPair : symbolDictionary)
    {
        Value symbol;
        symbol.SetObject();
        symbol.AddMember("value", symbolPair.second.getValue(), allocator);
        symbol.AddMember("type", symbolPair.second.isTerminal(), allocator);
        d["symbols"].PushBack(symbol, allocator);
    }

    OStreamWrapper osw(out);
    PrettyWriter<OStreamWrapper> writer(osw);
    d.Accept(writer);

    return out;
}

std::istream & operator >> (std::istream & in, LL1Table &table)
{
    using namespace rapidjson;

    IStreamWrapper isw(in);

    Document d;
    d.ParseStream(isw);

    if (!(d.HasMember("symbols") && d.HasMember("tt") && d.HasMember("start")))
        return in;

    std::map<std::string, Symbol> symbolDictionary;

    for (auto& symbolObj : d["symbols"].GetArray())
    {
        Symbol symbol(std::string(symbolObj["value"].GetString()), symbolObj["type"].GetBool());
        symbolDictionary[symbol.getValue()] = symbol;
    }

    for (auto& row : d["tt"].GetArray())
    {
        std::string currentSymbolName(row["__currentSymbol"].GetString());

        for (auto& m : row.GetObject())
        {
            std::string terminalSymbolName(m.name.GetString());
            if (terminalSymbolName != "__currentSymbol")
            {
                Value tableEntryValue = m.value.GetObject();
                Value productionValue = tableEntryValue["production"].GetObject();
                auto symbolStringArray = productionValue["symbolString"].GetArray();
                std::vector<Symbol> symbolString;
                for (auto &value : symbolStringArray)
                {
                    symbolString.push_back(symbolDictionary[value.GetString()]);
                }


                Production p( symbolDictionary[productionValue["nonTerminal"].GetString()], symbolString);

                TableEntry tableEntry(p, tableEntryValue["sync"].GetBool());
                table.table[symbolDictionary[currentSymbolName]][symbolDictionary[terminalSymbolName]] = tableEntry;
            }
        }
    }

    table.start = symbolDictionary[d["start"].GetString()];

    return in;
}
