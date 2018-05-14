#include "State.h"
#include <iostream>

StateId State::new_id = 0;

StateId State::newID()
{
    return new_id++;
}

State::State()
    : id(State::newID()), priority(0)
{
}

State::State(StateId number)
    : id(number), priority(0)
{
}

int State::getID() const
{
    return id;
}

void State::setTokenType(std::string str)
{
    tokenType = str;
}

std::string State::getTokenType() const
{
    return tokenType;
}

void State::setPriority(int p)
{
    priority = p;
}

int State::getPriority() const
{
    return priority;
}

bool State::operator==(const State & s) const
{
    return id == s.getID();
}

bool operator<(const State &lhs, const State &rhs)
{
    return lhs.getID() < rhs.getID();
}
