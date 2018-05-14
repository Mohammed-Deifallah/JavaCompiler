#include "ParserHandler.h"
#include <iostream>
#include <string>
#include "string_function.h"
#define EPS "\\\\L"
ParserHandler::ParserHandler()
{

}
ParserHandler::~ParserHandler()
{

}

std::string derivation = "";

void printProduction(Production &p)
{
    // print production to screen
    std::cout << p.lhs().getValue() << " -> ";
    for (int i = 0; i < p.rhs().size(); ++i) {
        std::cout<< p.rhs()[i].getValue() << " ";
    }
    std::cout << std::endl;
}

void expandDerivation(Production &p)
{
    std::string oldSymbol = p.lhs().getValue();
    std::string newSymbols = "";

    int size = p.rhs().size();
    for (int i = 0; i < size ; ++i) {
        newSymbols += p.rhs()[i].getValue();
        if(i != size-1)
            newSymbols += " ";
    }
    if(derivation == "")
        derivation = newSymbols;
    else
        replace(derivation,oldSymbol,newSymbols);

    // remove excesses
    replaceAll(derivation,EPS,"");
    replaceAll(derivation,"  "," ");

    std::cout << derivation << std::endl;
}
void ParserHandler::handleProduction(Production &p)
{
    expandDerivation(p);
    //printProduction(p);

}
