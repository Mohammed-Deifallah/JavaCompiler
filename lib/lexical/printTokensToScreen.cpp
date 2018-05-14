#include "printTokensToScreen.h"
#include <iomanip>

void printTokensToScreen(LexicalAnalyzer &lexicalAnalyzer)
{
    Token * token = nullptr;
    while (token = lexicalAnalyzer.nextToken())
    {
        std::cout << std::setw(12) << std::left << token->type << " " << token->value << std::endl;
    }
}
