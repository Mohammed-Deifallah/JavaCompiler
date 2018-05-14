#include "Production.h"

Production::Production() {}
Production::Production(Symbol left, std::vector<Symbol> right)
    : nonTerm(left), production(right) {}
std::vector<Symbol> Production::rhs() const
{
    return production;
}
Symbol Production::lhs() const
{
    return nonTerm;
}
bool Production::operator==(const Production &s) const {
    return this->production==s.production && s.nonTerm==this->nonTerm;
}
