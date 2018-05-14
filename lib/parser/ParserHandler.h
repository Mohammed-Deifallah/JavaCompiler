#ifndef JAVACOMPILER_PARSERHANDLER_H
#define JAVACOMPILER_PARSERHANDLER_H

#include "Production.h"



class ParserHandler {

    public:
        ParserHandler();
        ~ParserHandler();
        void handleProduction(Production &);
};


#endif