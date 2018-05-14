#ifndef JAVACOMPILER_PREDECTIVEPARSER_H
#define JAVACOMPILER_PREDECTIVEPARSER_H

#include "LL1Table.h"
#include "Production.h"
#include "ParserHandler.h"
#include <LexicalAnalyzer.h>
#include <ErrorLog.h>
#include <memory>


class PredictiveParser {
    struct impl;
    std::unique_ptr<impl> m_d;
public:
    PredictiveParser(LexicalAnalyzer *, LL1Table &, ErrorLog *, ParserHandler *);
    ~PredictiveParser();
    void parse();
private:
    void stackPush(Production &);
    Symbol stackPop();
    Symbol stackTop();
};

#endif