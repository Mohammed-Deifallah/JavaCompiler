#include <iostream>
#include "CFG.h"
#include "TableCreator.h"
#include "writeLL1Table.h"
#include "CFGModification.h"

using namespace std;

int main(int argc, char **argv)
{
    // Skip the executable name
    argc--;
    argv++;

    if (argc == 0)
    {
        cout << "Please insert the path for a cfg file on the command line. Example: ParserGenerator grammar.txt";
        return 0;
    }

    std::string cfgFilename = argv[0];

    CFG cfg(cfgFilename);

    // Modify Grammer
    CFG modified = removeLeftFactoring(cfg);
    CFG tempCFG = modified;
    CFG result = removeLeftRecursion2(tempCFG);

    // Unmodified grammer
    //CFG result = cfg;

    cout << "Printing Productions......" << endl;
    for (auto &prod : result.getAllProductions())
    {
        cout << prod.lhs().getValue() << " >> ";
        for (auto &s : prod.rhs())
        {
            cout << s.getValue() << " ";
        }
        cout << endl;
    }

    TableCreator tableCreator(result);
    LL1Table ll1Table = tableCreator.createTable();

    cout << "Printing Table......" << endl;
    std::map<Symbol, std::map<Symbol, TableEntry>> table = ll1Table.getTable();
    std::map<Symbol, std::map<Symbol, TableEntry>>::iterator it;
    for (it = table.begin(); it != table.end(); it++)
    {
        cout << it->first.getValue() << "\n";
        std::map<Symbol, TableEntry>::iterator it2;
        std::map<Symbol, TableEntry> row = it->second;
        for (it2 = row.begin(); it2 != row.end(); it2++)
        {
            cout << it2->first.getValue() << ": isSync= " << it2->second.isSync() << " production= ";
            Production prod = it2->second.getProduction();
            cout << prod.lhs().getValue() << " >> ";
            for (auto &s : prod.rhs())
            {
                cout << s.getValue() << " ";
            }
            cout << "\n";
        }
    }

    writeLL1Table(ll1Table, "LL1table.json");

    cout << endl << "LL(1) table written to LL1table.json" << endl;

    return 0;
}
