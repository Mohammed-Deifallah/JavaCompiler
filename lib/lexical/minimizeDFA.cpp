#include "minimizeDFA.h"
#include "GroupKeeper.h"

bool match(StateId s1, StateId s2, const GroupKeeper &keeper, const DFATransitionTable &dfa);

DFATransitionTable minimizeDFA(const DFATransitionTable &dfa)
{
    GroupKeeper keeper;
    
    auto allIt = keeper.addGroup(dfa.getStateIds());
    keeper.partition(allIt, dfa.getAcceptingStateIds());

    GroupKeeper::iterator it = keeper.end();
    while ((it = keeper.getUnmarkedGroupOfmultiState()) != keeper.end())
    {
        Group g = *it;
        StateId representative = *g.begin();
        Group partition;
        partition.insert(representative);
        for (auto state : g)
        {
            if (state != representative && match(representative, state, keeper, dfa))
            {
                partition.insert(state);
            }
        }

        if (partition == g)
        {
            keeper.mark(it);
        }
        else
        {
            keeper.partition(it, partition);
        }
    }

    DFATransitionTable newDFA;
    std::map<StateId, StateId> representativeMapping;
    for (auto &group : keeper)
    {
        StateId representativeId = *group.begin();
        newDFA.storeState(dfa.getState(representativeId));
        for (auto &&stateId : group)
        {
            representativeMapping[stateId] = representativeId;
        }
    }

    for(StateId representativeId : newDFA.getStateIds())
    {
        auto transitions = dfa.getTransitions(representativeId);

        for (auto &transitionPair : transitions)
        {
            newDFA.setTransition(representativeId, transitionPair.first, representativeMapping[transitionPair.second]);
        }

        if (dfa.isAcceptingState(representativeId))
        {
            newDFA.addAcceptingState(representativeId);
        }
    }

    newDFA.setStartingState(dfa.getStartingState());

    return newDFA;
}



bool match(StateId s1, StateId s2, const GroupKeeper &keeper, const DFATransitionTable &dfa)
{
    auto s1Transitions = dfa.getTransitions(s1);
    auto s2Transitions = dfa.getTransitions(s2);

    // 1. s1Transitions only have the transitions to real States
    // if sizes does not match, This means that there is an input for s1
    // that goes to the empty state while s2 goes to a real state. Or vice versa.
    // 2. The other condition is simply to have different accepting states for
    // different tokens instead of merging them.
    if (s1Transitions.size() != s2Transitions.size() || dfa.getState(s1).getTokenType() != dfa.getState(s2).getTokenType())
    {
        return false;
    }

    for (auto &transition : s1Transitions)
    {
        auto s2Transition = s2Transitions.find(transition.first);

        if (s2Transition != s2Transitions.end())
        {
            if (keeper.group(transition.second) != keeper.group(s2Transition->second))
            {
                return false;
            }
        }
    }

    return true;
}
