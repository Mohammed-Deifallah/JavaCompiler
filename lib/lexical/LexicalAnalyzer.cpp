#include "LexicalAnalyzer.h"

#include <sstream>
#include <assert.h>

#include "ErrorLog.h"
#include "StreamWrapper.h"
#include "DFATransitionTable.h"

using namespace std;

vector<pair<string, int>> splitByWhitespace(const string & str);

struct LexicalAnalyzer::impl
{
    DFATransitionTable dfa;
    StreamWrapper _streamWrapper;
    ErrorLog * errorLog;
    SymbolTable * symbolTable;
    int startPos;

    impl(const DFATransitionTable &dfa,
        std::istream& in,
        ErrorLog *errorLog,
        SymbolTable *symbolTable)
        : dfa(dfa), _streamWrapper(in), errorLog(errorLog), symbolTable(symbolTable)
    {
    }
};

LexicalAnalyzer::LexicalAnalyzer(
    const DFATransitionTable &dfa,
    std::istream& in,
    ErrorLog *errorLog,
    SymbolTable *symbolTable)
    : m_d(std::make_unique<impl>(dfa, in, errorLog, symbolTable))
{
}

LexicalAnalyzer::~LexicalAnalyzer()
{

}

Token* LexicalAnalyzer::nextToken()
{
    StateId currState = m_d->dfa.getStartingState();
    StateId nextState = currState;

    char c = EOF;
    StateId lastAcceptedState;
    bool accepted = false;
    std::string lexem, lastAcceptedLexem;
    std::string undefinedString;

    m_d->startPos = m_d->_streamWrapper.getPos();

    // Loop getting single characters
    while ((c = m_d->_streamWrapper.getNext()) != EOF)
    {
        lexem += c;

        if (m_d->dfa.hasTransition(currState, c)) // a transition found.
        {
            nextState = m_d->dfa.move(currState, c);

            if (m_d->dfa.isAcceptingState(nextState))
            {
                lastAcceptedState = nextState;
                lastAcceptedLexem = lexem;
                accepted = true;
            }
            currState = nextState;
        }
        else if (accepted) // no transition found while a previous accepted input is found
        {
            break;
        }
        else // no transition found and not accepted input found
        {
            // Error: Undefined input of /lexem/
            // Recover by removing the first char and start again
            undefinedString += lexem[0];
            m_d->_streamWrapper.putFront(lexem.substr(1, lexem.size() - 1));
            lexem = "";
            currState = m_d->dfa.getStartingState();
            nextState = currState;
        }
    }

    Token *token = nullptr;

    if (accepted)
    {
        // Token Type according to lastAcceptedState
        // position = startPos + undefinedString.size()
        State acceptedState = m_d->dfa.getState(lastAcceptedState);
        token = new Token(acceptedState.getTokenType(), lastAcceptedLexem);

        if (lexem.size() > lastAcceptedLexem.size())
            m_d->_streamWrapper.putFront(lexem.substr(lastAcceptedLexem.size(), lexem.size() - lastAcceptedLexem.size()));
    }
    else // No accepted input.
    {
        // Has to arrive at EOF
        assert(c == EOF);
        assert(lexem.empty());
    }

    if (undefinedString.size())
    {
        // Error: Undefined input of /undefinedString/
        // position = startPosition
        std::vector<std::pair<std::string, int>> errorLexemes = splitByWhitespace(undefinedString);

        for (auto &pair : errorLexemes)
        {
            m_d->errorLog->add(pair.first, m_d->startPos + pair.second, m_d->startPos, "Undefined input");
        }
    }

    return token;
}

int LexicalAnalyzer::getLineNumber()
{
    return 0;
}

int LexicalAnalyzer::getTokenPos()
{
    return m_d->startPos;
}

vector<pair<string, int>> splitByWhitespace(const string & str)
{
    static const set<char> whitespace = {
        ' ',
        '\t',
        '\r',
        '\n'
    };

    vector<pair<string, int>> output;

    int lastWhitespace = -1;

    for(size_t i = 0; i < str.size(); ++i)
    {
        if (whitespace.find(str[i]) != whitespace.end())
        {
            // Whitespace
            if (i - lastWhitespace - 1 > 0)
            {
                output.push_back(pair<string, int>(str.substr(lastWhitespace + 1, i - lastWhitespace - 1), lastWhitespace + 1));
            }

            lastWhitespace = i;
        }
    }

    if (str.size() - lastWhitespace - 1 > 0)
    {
        output.push_back(pair<string, int>(str.substr(lastWhitespace + 1, str.size() - lastWhitespace - 1), lastWhitespace + 1));
    }

    return output;
}