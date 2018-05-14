#include "convertNFAToDFA.h"
#include <queue>
#include <iterator>
#include <assert.h>
#include <algorithm>

using namespace std;

DFATransitionTable convertNFAToDFA(const NFATransitionTable &nfa)
{
    DFATransitionTable dfa;

    // map between NFA set of states and DFA State
    map<set<StateId>, StateId> Dstates;
    std::vector<set<StateId>> toBeProcessed;

    set<StateId> startClosure = nfa.epsClosure(nfa.getStartingStates());
    State s;
    dfa.storeState(s);
    Dstates[startClosure] = s.getID();
    toBeProcessed.push_back(startClosure);

    dfa.setStartingState(Dstates[startClosure]);

    const set<StateId> nfaAcceptingStates = nfa.getAcceptingStates();

    while (!toBeProcessed.empty())
    {
        set<StateId> workingSet = *toBeProcessed.begin();
        StateId fromState = Dstates[workingSet];

        set<char> alphabet = nfa.transitionAlphabet(workingSet);

        for (char input : alphabet)
        {
            set<StateId> states = nfa.epsClosure(nfa.move(workingSet, input));
            auto dStateIt = Dstates.find(states);
            if (dStateIt == Dstates.end())
            {
                State s;
                dfa.storeState(s);
                Dstates[states] = s.getID();

                vector<StateId> acceptingStates;
                set_intersection(nfaAcceptingStates.begin(), nfaAcceptingStates.end(), states.begin(), states.end(),
                    std::inserter(acceptingStates, acceptingStates.begin()));

                if (!acceptingStates.empty())
                {
                    // Take the one with the highest priority
                    auto chosenAcceptingState = std::max_element(acceptingStates.begin(), acceptingStates.end(),
                        [&](StateId rhs, StateId lhs) {
                            return nfa.getState(rhs).getPriority() < nfa.getState(lhs).getPriority();
                    });

                    StateId acceptingStateId = *chosenAcceptingState;
                    State acceptingState = nfa.getState(acceptingStateId);
                    assert(acceptingState.getTokenType().size());

                    s.setTokenType(acceptingState.getTokenType());
                    dfa.storeState(s);

                    dfa.addAcceptingState(s.getID());
                }

                toBeProcessed.push_back(states);
            }

            StateId toState = Dstates[states];
            dfa.setTransition(fromState, input, toState);
        }

        toBeProcessed.erase(toBeProcessed.begin());
    }

    return dfa;
}
