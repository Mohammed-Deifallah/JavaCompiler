#include "ErrorLog.h"
#include "NFATransitionTable.h"
#include "LexicalAnalyzer.h"
#include "readTransitionTable.h"
#include "writeErrorLogToFile.h"
#include "printTokensToScreen.h"
#include "PredictiveParser.h"
#include "ParserHandler.h"
#include "TableCreator.h"
#include "readLL1Table.h"
#include "writeLL1Table.h"
#include "CFGModification.h"

int main(int argc, char** argv)
{
    // Skip the executable name
    argc--; argv++;
    //argc++; argv[0] =  "../bin/files/sample.txt";
    if (argc == 0)
    {
        cout << "Please insert the path for a c code file on the command line. Example: LexicalAnalyzer prog.c";
        return 0;
    }

    std::string transitionTableFilename = "dfa.json";

    // Read TransitionTable from file
    // input: filename
    // output: TransitionTable
    DFATransitionTable dfa = readTransitionTable(transitionTableFilename);

    // Create ErrorLogs
    ErrorLog lexicalErrorLog;
    ErrorLog syntaxErrorLog;

    std::string codeFilename = argv[0];
    std::ifstream codeFile;

    // Create LexicalAnalyzer for the given code filename
    // input: TransitionTable, filename

    // TODO Check for return value
    codeFile.open(codeFilename);

    LexicalAnalyzer lexicalAnalyzer(dfa, codeFile, &lexicalErrorLog);


    // Execute the lexical analyzer to process the whole file
    // input: LexicalAnalyzer, ErrorLog
    // output: print tokens on screen
    //printTokensToScreen(lexicalAnalyzer);

    std::string LL1TableFilename = "LL1table.json";

    // Read LL1Table from file
    // input: filename
    // output: LL1Table
    //LL1Table LL1Table = readLL1Table(LL1TableFilename);
    std::string cfgFilename = "../bin/files/text.txt";
    CFG cfg(cfgFilename);


    // Modify Grammer
    CFG modified = removeLeftFactoring(cfg);
    CFG tempCFG = modified;
    CFG result = removeLeftRecursion(tempCFG);

    // Unmodified grammer
    //CFG result = cfg;

    TableCreator tableCreator(result);



    LL1Table ll1Table = tableCreator.createTable();
/*
    writeLL1Table(ll1Table, "LL1Table.json");

    LL1Table t2 = readLL1Table("LL1Table.json");
*/
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

    //create parser handler
    ParserHandler handler;
    // create parser
    PredictiveParser parser (&lexicalAnalyzer, ll1Table, &syntaxErrorLog, &handler);
    parser.parse();


    // Write ErrorLog somewhere (in a file for example)
    // input: ErrorLog, filename
    // output: file containing error messages one per line.
    std::string lexicalErrorsFileName = "lexicalErrors.txt";
    std::string syntaxErrorsFileName = "syntaxErrors.txt";
    writeErrorLogToFile(lexicalErrorLog, lexicalErrorsFileName);
    writeErrorLogToFile(syntaxErrorLog, syntaxErrorsFileName);

    return 0;
}
