#include "PredictiveParser.h"
#include "TableEntry.h"
#include <stack>
#define EPS "\\\\L"
using namespace std;

struct PredictiveParser::impl
{
    LexicalAnalyzer *lexer;
    LL1Table parseTable;
    ErrorLog * errorLog;
    ParserHandler *handler;

    string marker = "$";
    Symbol endMarker;
    stack<Symbol> stack;

    impl(LexicalAnalyzer *lexer,
         LL1Table &table,
         ErrorLog *errorLog,
         ParserHandler *handler)
         : lexer(lexer), parseTable(table), errorLog(errorLog), handler(handler)
    {
        if(lexer == nullptr || errorLog == nullptr || handler == nullptr)
            throw runtime_error("Error: Some Arguments are null pointers");
        endMarker = Symbol(marker, true);
    }
};

PredictiveParser::PredictiveParser(LexicalAnalyzer *lexer, LL1Table &table,
                                   ErrorLog *errorLog, ParserHandler *handler )
        :m_d(std::make_unique<impl>(lexer,table, errorLog,handler))
{

    //m_d->endMarker = Symbol(m_d->marker, true);
}

PredictiveParser::~PredictiveParser()
{

}
void PredictiveParser::stackPush(Production &production)
{
    for (int i = production.rhs().size()-1; i >=0 ; i--) {
        m_d->stack.push(production.rhs()[i]);
    }
}
Symbol PredictiveParser::stackPop()
{
    Symbol temp;
    if(!m_d->stack.empty())
    {
        temp =  m_d->stack.top();
        m_d->stack.pop();
    }
    return temp;
}
Symbol PredictiveParser::stackTop()
{
    if(!m_d->stack.empty())
        return  m_d->stack.top();
}

void PredictiveParser::parse()
{
    //push end marker and the start symbol S
    m_d->stack.push(m_d->endMarker);
    m_d->stack.push(m_d->parseTable.getStartSymbol());

    Token end (m_d->marker,m_d->marker);
    bool input_consumed = false;
    //cout << "Start Symbol: " <<stackTop().getValue() <<endl;
    Token *input = m_d->lexer->nextToken();
    if (input == nullptr)
        return; // empty input

    while(stackTop().getValue() != m_d->endMarker.getValue()
            && input != nullptr)
    {
        //cout << "TYPE: " << input->type << " ,VALUE: " << input->value << endl;
        if(stackTop().isTerminal()) {
            // if the top of the stack = input then it's a match
            if (stackTop().getValue() == input->type || // eg. id == id
                stackTop().getValue() == input->value)  // eg. '+' == '+'
            {
                stackPop();
                input = m_d->lexer->nextToken();

            } else
            {
                cout << "unmathced:" <<stackTop().getValue() << " VS " << input->value << endl;
                // state an error and pop stack
                string msg = " Error: Expected \"" + stackTop().getValue() + "\".";
                m_d->errorLog->add(input->value,m_d->lexer->getLineNumber(),m_d->lexer->getTokenPos(),
                                   msg);
                stackPop();
            }
        } else
        {
            string Nstring = stackTop().getValue();
            Symbol N (Nstring, false);
            Symbol T (input->type, true);
            TableEntry *entry = m_d->parseTable.getProduction(N, T);
            if(entry == nullptr)
            {
                T = Symbol (input->value, true);
                entry = m_d->parseTable.getProduction(N, T);
            }

            if(entry == nullptr)
            {
                // state an error and discard input
                string msg = " Error: Unexpected \"" + input->value + "\".";
                m_d->errorLog->add(input->value,m_d->lexer->getLineNumber(),m_d->lexer->getTokenPos(),
                                   msg);
                input = m_d->lexer->nextToken();

            } else if (entry->isSync()){
                // state an error and pop stack
                string msg = " Error: Unexpected2 \"" + input->value + "\".";
                m_d->errorLog->add(input->value,m_d->lexer->getLineNumber(),m_d->lexer->getTokenPos(),
                                   msg);
                stackPop();
            } else
            {
                // A new production occurs
                Production p = entry->getProduction();
                stackPop();
                if(!(p.rhs()[0].getValue() == EPS)){
                    stackPush(p);
                }
                // call the parser handler to handle the production
                m_d->handler->handleProduction(p);
            }
        }
        if( !input_consumed && input == nullptr){
            input_consumed = true;
            input = &end;
        }
    }
    // input is not consumed or stack isn't empty
    if( input != nullptr && input->value != m_d->endMarker.getValue())
    {
        // State Error: Excess input
        string msg = " Error: Excess input starting at\"" + input->value + "\"";
        m_d->errorLog->add(input->value,m_d->lexer->getLineNumber(),m_d->lexer->getTokenPos(),
                           msg);
    }
}