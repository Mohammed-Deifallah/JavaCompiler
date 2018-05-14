#include "Symbol.h"

Symbol::Symbol() : value(""), type(true) {}

Symbol::Symbol(std::string &s, bool t) : value(s), type(t) {}

Symbol::Symbol(const std::string s) : value(s), type(true) {}

std::string Symbol::getValue() const
{
    return value;
}
bool Symbol::isTerminal() const
{
    return type;
}

void Symbol::setValue(std::string v)
{
    value = v;
}
void Symbol::setType(bool t)
{
    type = t;
}

bool Symbol::operator==(const Symbol &s) const
{
    return value == s.getValue();
}
bool Symbol::operator!=(const Symbol &s) const
{
    return value != s.getValue();
}
bool Symbol::operator<(const Symbol &s) const
{
    return s.getValue() < value;
}

Symbol Symbol::operator=(const Symbol &s)
{
    this->value = s.value;
    this->type = s.type;
    return *this;
}
