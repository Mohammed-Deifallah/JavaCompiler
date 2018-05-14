#ifndef LexicalAnalyzer_H
#define LexicalAnalyzer_H

#include <memory>
#include <iostream>

#include "Token.h"

class ErrorLog;
class SymbolTable;
class DFATransitionTable;

class LexicalAnalyzer
{
public:
    LexicalAnalyzer();
    LexicalAnalyzer(const DFATransitionTable &dfa, std::istream& in, ErrorLog *errorLog = nullptr, SymbolTable *symbolTable = nullptr);
    ~LexicalAnalyzer();

    Token* nextToken();
    int getLineNumber();
    int getTokenPos();

private:
    struct impl;
    std::unique_ptr<impl> m_d;
};

#endif // LexicalAnalyzer_H
