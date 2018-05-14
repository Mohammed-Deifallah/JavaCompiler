#include "minimizeDFA.h"
#include "convertNFAToDFA.h"
#include "convertRulesToNFA.h"
#include "writeTransitionTable.h"


int main(int argc, char** argv)
{
    using namespace std;

    // Skip the executable name
    argc--; argv++;

    if (argc == 0)
    {
        cout << "Please insert the path for a rules file on the command line. Example: LexicalAnalyzerGenerator ./rules.txt";
        return 0;
    }

    NFATransitionTable nfa = convertRulesToNFA(argv[0]);

    DFATransitionTable dfa = convertNFAToDFA(nfa);

    DFATransitionTable minDFA = minimizeDFA(dfa);

    writeTransitionTable(minDFA, "dfa.json");

    cout << endl << "Transition table written to dfa.json" << endl;

    return 0;
}
