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

int main(int argc, char** argv)
{
    // Skip the executable name
    argc--; argv++;
    if (argc == 0)
    {
        cout << "Please insert the path for a c code file on the command line. Example: LexicalAnalyzer prog.c";
        return 1;
    }

    std::string transitionTableFilename = "dfa.json";
    DFATransitionTable dfa = readTransitionTable(transitionTableFilename);

    std::string codeFilename = argv[0];
    std::ifstream codeFile;
    codeFile.open(codeFilename);

    if (!codeFile.is_open())
    {
        cout << "Cannot open file " << codeFilename << std::endl;
        return 1;
    }

    ErrorLog lexicalErrorLog;
    LexicalAnalyzer lexicalAnalyzer(dfa, codeFile, &lexicalErrorLog);

    std::string LL1TableFilename = "LL1table.json";
    LL1Table LL1Table = readLL1Table(LL1TableFilename);

    ErrorLog syntaxErrorLog;
    ParserHandler handler;
    PredictiveParser parser (&lexicalAnalyzer, LL1Table, &syntaxErrorLog, &handler);
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
