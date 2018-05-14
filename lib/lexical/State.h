#ifndef State_H
#define State_H

#include <string>

typedef int StateId;

class State
{
    static StateId new_id;
    StateId id;

    std::string tokenType;
    int priority;

public:
    State();
    State(StateId number);

    StateId getID() const;

    void setTokenType(std::string str);
    std::string getTokenType() const;

    void setPriority(int p);
    int getPriority() const;

    bool operator==(const State & s) const;

    static StateId newID();
};

bool operator<(const State &lhs, const State &rhs);

#endif // State_H
