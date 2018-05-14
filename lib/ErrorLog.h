#ifndef ErrorLog_H
#define ErrorLog_H

#include <memory>
#include <string>
#include <iostream>

class ErrorLog
{
public:
    ErrorLog();
    ~ErrorLog();

    void add(const std::string &lexeme, int lineNumber, int positionInLine, const std::string &message);

private:
    struct impl;
    std::unique_ptr<impl> m_d;

    friend std::ostream& operator<<(std::ostream& out, const ErrorLog &errorLog);
};

std::ostream& operator<<(std::ostream& out, const ErrorLog &errorLog);

#endif // ErrorLog_H
