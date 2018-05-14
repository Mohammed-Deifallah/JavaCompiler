#ifndef TransitionTable_H
#define TransitionTable_H

#include <string>
#include <set>
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <tuple>

#include "State.h"

// Epsilon symbol need to be defined
// (device control 1) This is a control character that is not likely to exist in a file
enum
{
    EPS = '\17'
};

class NFATransitionTable
{
public:
    // This is better be implemented as std::optional (C++17) or Boost::optional
    typedef char Input;
    typedef std::tuple<State, Input, State> Transition;

    NFATransitionTable();
    NFATransitionTable(const NFATransitionTable &nfa);
    ~NFATransitionTable();

    const NFATransitionTable &operator=(const NFATransitionTable &nfa);

    void storeState(const State &s);
    State getState(StateId stateId) const;
    std::set<StateId> getStateIds() const;

    void setTransition(StateId fromState, Input input, StateId toState);

    std::set<StateId> epsClosure(StateId stateId) const;
    std::set<StateId> epsClosure(const std::set<StateId> &stateIds) const;
    std::set<StateId> move(const std::set<StateId> &stateIds, Input input) const;

    void setStartingStates(const std::set<StateId> &stateIds);
    void addStartingState(StateId stateId);

    void setAcceptingStates(const std::set<StateId> &stateIds);
    void addAcceptingStates(StateId stateId);

    std::set<StateId> getStartingStates() const;
    std::set<StateId> getAcceptingStates() const;

    std::set<Input> transitionAlphabet(const std::set<StateId> &stateIds) const;
    bool isAcceptingSet(const std::set<StateId> &stateIds) const;

    NFATransitionTable opUnion(const NFATransitionTable &rhs) const;
    NFATransitionTable opConcat(const NFATransitionTable &rhs) const;
    NFATransitionTable opPlus() const;
    NFATransitionTable opStar() const;

    static NFATransitionTable mergeNFAs(const std::vector<NFATransitionTable>& nfas);

private:
    struct impl;
    std::unique_ptr<impl> m_d;
    friend void copyAllTransitions(NFATransitionTable &target, const NFATransitionTable &source);
};

#endif // NFATransitionTable_H
