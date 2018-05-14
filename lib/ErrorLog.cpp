#include "ErrorLog.h"
#include <iostream>
#include <vector>

struct Error
{
    int line;
    int position;
    std::string input;
    std::string message;
};

struct ErrorLog::impl
{
    std::vector<Error> errors;
};

ErrorLog::ErrorLog()
    : m_d(std::make_unique<impl>())
{

}

ErrorLog::~ErrorLog()
{

}

void ErrorLog::add(const std::string &lexeme, int lineNumber, int positionInLine, const std::string &message)
{
    Error e;
    e.line = lineNumber;
    e.position = positionInLine;
    e.input = lexeme;
    e.message = message;
    m_d->errors.push_back(e);
}


std::ostream& operator<<(std::ostream& out, const ErrorLog &errorLog)
{
    for (auto &e : errorLog.m_d->errors)
    {
        out << "Input: \"" << e.input << "\""
            << " line:" << e.line
            << " pos:"  << e.position
            << " msg:"  << e.message
            << std::endl;
    }

    return out;
}
