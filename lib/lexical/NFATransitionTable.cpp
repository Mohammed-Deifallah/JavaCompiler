#include "NFATransitionTable.h"
#include <algorithm>
#include <iterator>
#include <assert.h>

struct NFATransitionTable::impl
{
    std::map < StateId, std::map<Input, std::set<StateId> > > table;
    std::map<StateId, State> states;

    std::set<StateId> startingStates;
    std::set<StateId> endingStates;

    std::set<StateId> directClosure(StateId stateId, Input input) const;
    bool hasConcreteTransition(StateId stateId) const;

};


NFATransitionTable::NFATransitionTable()
    : m_d(std::make_unique<impl>())
{

}

NFATransitionTable::NFATransitionTable(const NFATransitionTable & nfa)
    : m_d(std::make_unique<impl>())
{
    m_d->table = nfa.m_d->table;
    m_d->states = nfa.m_d->states;
    m_d->startingStates = nfa.m_d->startingStates;
    m_d->endingStates = nfa.m_d->endingStates;
}

NFATransitionTable::~NFATransitionTable()
{

}

const NFATransitionTable & NFATransitionTable::operator=(const NFATransitionTable &nfa)
{
    m_d->table = nfa.m_d->table;
    m_d->states = nfa.m_d->states;
    m_d->startingStates = nfa.m_d->startingStates;
    m_d->endingStates = nfa.m_d->endingStates;

    return *this;
}

void NFATransitionTable::storeState(const State &s)
{
    auto it = m_d->states.find(s.getID());
    if (it == m_d->states.end())
    {
        m_d->states.insert(std::pair<StateId, State>(s.getID(), s));
    }
    else
    {
        m_d->states[s.getID()] = s;
    }
}

State NFATransitionTable::getState(StateId stateId) const
{
    auto it = m_d->states.find(stateId);
    assert(it != m_d->states.end());
    return it->second;
}

std::set<StateId> NFATransitionTable::getStateIds() const
{
    std::set<StateId> stateIds;
    for (auto & pair : m_d->states)
    {
        stateIds.insert(pair.first);
    }
    return stateIds;
}

void NFATransitionTable::setTransition(StateId fromId, NFATransitionTable::Input input, StateId toId)
{
    assert(m_d->states.find(fromId) != m_d->states.end());
    assert(m_d->states.find(toId) != m_d->states.end());

    m_d->table[fromId][input].insert(toId);
}

std::set<StateId> NFATransitionTable::impl::directClosure(StateId stateId, Input input) const
{
    assert(states.find(stateId) != states.end());

    std::set<StateId> states;
    auto it = table.find(stateId);

    if (it == table.end())
    {
        return states;
    }

    auto inputIt = it->second.find(input);

    if (inputIt == it->second.end())
    {
        return states;
    }

    return inputIt->second;
}

bool NFATransitionTable::impl::hasConcreteTransition(StateId stateId) const
{
    assert(states.find(stateId) != states.end());

    auto it = table.find(stateId);

    if (it == table.end())
    {
        return false;
    }

    for (auto &transitionPair : it->second)
    {
        if (transitionPair.first != EPS)
        {
            return true;
        }
    }

    return false;
}

// OLDER VERSION
// This seems to be an optimization suggested by Youssef. But not sure about its effect.
//std::set<State> NFATransitionTable::epsClosure(const State &state) const
//{
//    std::set<State> resultColsure;
//    std::set<State> toBeProcessed;
//    std::set<State> doneProcessing;
//
//    State workingState;
//
//    toBeProcessed.insert(state);
//
//    while (!toBeProcessed.empty())
//    {
//        workingState = *toBeProcessed.begin();
//        std::set<State> directClosure = m_d->directClosure(workingState, EPS);
//
//        if (m_d->hasConcreteTransition(*toBeProcessed.begin()))
//        {
//            resultColsure.insert(*toBeProcessed.begin());
//        }
//        else if (directClosure.empty())
//        {
//            resultColsure.insert(*toBeProcessed.begin());
//        }
//
//        if (!directClosure.empty())
//        {
//            std::set<State> newInClosure;
//
//            std::set_difference(directClosure.begin(), directClosure.end(), doneProcessing.begin(), doneProcessing.end(),
//                std::inserter(newInClosure, newInClosure.begin()));
//
//            if (!newInClosure.empty())
//            {
//                toBeProcessed.insert(newInClosure.begin(), newInClosure.end());
//            }
//        }
//
//        doneProcessing.insert(workingState);
//        toBeProcessed.erase(workingState);
//    }
//
//    return resultColsure;
//}


std::set<StateId> NFATransitionTable::epsClosure(StateId stateId) const
{
    assert(m_d->states.find(stateId) != m_d->states.end());

    std::set<StateId> resultColsure;
    std::set<StateId> toBeProcessed;
    std::set<StateId> doneProcessing;

    StateId workingState;

    resultColsure.insert(stateId);
    toBeProcessed.insert(stateId);

    while (!toBeProcessed.empty())
    {
        workingState = *toBeProcessed.begin();
        std::set<StateId> directClosure = m_d->directClosure(workingState, EPS);

        resultColsure.insert(directClosure.begin(), directClosure.end());

        if (!directClosure.empty())
        {
            std::set<StateId> newInClosure;

            std::set_difference(directClosure.begin(), directClosure.end(), doneProcessing.begin(), doneProcessing.end(),
                std::inserter(newInClosure, newInClosure.begin()));

            if (!newInClosure.empty())
            {
                toBeProcessed.insert(newInClosure.begin(), newInClosure.end());
            }
        }

        doneProcessing.insert(workingState);
        toBeProcessed.erase(workingState);
    }

    return resultColsure;
}

// OLD Version
//std::set<StateId> NFATransitionTable::epsClosure(const std::set<StateId> &stateIds) const
//{
//    std::set<StateId> resultClosure;
//
//    for (auto && stateId : stateIds)
//    {
//        // This is not efficient but correct
//        std::set<StateId> closure = epsClosure(stateId);
//        resultClosure.insert(closure.begin(), closure.end());
//    }
//
//    return resultClosure;
//}

// Optimized version
std::set<StateId> NFATransitionTable::epsClosure(const std::set<StateId> &stateIds) const
{
    std::set<StateId> resultColsure;
    std::set<StateId> toBeProcessed;
    std::set<StateId> doneProcessing;

    resultColsure.insert(stateIds.begin(), stateIds.end());
    toBeProcessed.insert(stateIds.begin(), stateIds.end());

    StateId workingState;

    while (!toBeProcessed.empty())
    {
        workingState = *toBeProcessed.begin();
        std::set<StateId> directClosure = m_d->directClosure(workingState, EPS);

        resultColsure.insert(directClosure.begin(), directClosure.end());

        if (!directClosure.empty())
        {
            std::set<StateId> newInClosure;

            std::set_difference(directClosure.begin(), directClosure.end(), doneProcessing.begin(), doneProcessing.end(),
                std::inserter(newInClosure, newInClosure.begin()));

            if (!newInClosure.empty())
            {
                toBeProcessed.insert(newInClosure.begin(), newInClosure.end());
            }
        }

        doneProcessing.insert(workingState);
        toBeProcessed.erase(workingState);
    }

    return resultColsure;
}

std::set<StateId> NFATransitionTable::move(const std::set<StateId> &stateIds, Input input) const
{
    std::set<StateId> directClosureOfinput;

    for (auto && stateId : stateIds)
    {
        std::set<StateId> closure = m_d->directClosure(stateId, input);
        directClosureOfinput.insert(closure.begin(), closure.end());
    }

    return directClosureOfinput;
}

void NFATransitionTable::setStartingStates(const std::set<StateId> &stateIds)
{
    for (auto &stateId : stateIds)
    {
        assert(m_d->states.find(stateId) != m_d->states.end());
    }

    m_d->startingStates = stateIds;
}

void NFATransitionTable::addStartingState(StateId stateId)
{
    assert(m_d->states.find(stateId) != m_d->states.end());

    m_d->startingStates.insert(stateId);
}


void NFATransitionTable::setAcceptingStates(const std::set<StateId> &stateIds)
{
    for (auto &stateId : stateIds)
    {
        assert(m_d->states.find(stateId) != m_d->states.end());
    }

    m_d->endingStates = stateIds;
}

void NFATransitionTable::addAcceptingStates(StateId stateId)
{
    assert(m_d->states.find(stateId) != m_d->states.end());

    m_d->endingStates.insert(stateId);
}


std::set<StateId> NFATransitionTable::getStartingStates() const
{
    return m_d->startingStates;
}

std::set<StateId> NFATransitionTable::getAcceptingStates() const
{
    return m_d->endingStates;
}

std::set<NFATransitionTable::Input> NFATransitionTable::transitionAlphabet(const std::set<StateId> &stateIds) const
{
    std::set<Input> result;

    for (auto &stateId : stateIds)
    {
        auto it = m_d->table.find(stateId);
        if (it != m_d->table.end())
        {
            for (auto & transitionPair : it->second)
            {
                if (transitionPair.first != EPS)
                {
                    result.insert(transitionPair.first);
                }
            }
        }
    }

    return result;
}

bool NFATransitionTable::isAcceptingSet(const std::set<StateId> &stateIds) const
{
    for (auto& stateId : stateIds)
    {
        if (m_d->endingStates.find(stateId) != m_d->endingStates.end())
        {
            return true;
        }
    }
    return false;
}


void copyAllTransitions(NFATransitionTable &target, const NFATransitionTable &source)
{
    for (auto stateId : source.getStateIds())
    {
        target.storeState(source.getState(stateId));
    }

    for (auto && fromPair : source.m_d->table)
    {
        for (auto && inputPair : fromPair.second)
        {
            for (auto && toId : inputPair.second)
            {
                target.setTransition(fromPair.first, inputPair.first, toId);
            }
        }
    }
}

NFATransitionTable NFATransitionTable::opUnion(const NFATransitionTable &rhs) const
{
    NFATransitionTable result;

    // Depending on the uniqueness of State we will add the states from *this and rhs to result.
    //  Add all transition of *this to result
    copyAllTransitions(result, *this);

    //  Add all transition of rhs to result
    copyAllTransitions(result,rhs);

    //  Create resultStartingState 
    State startState;
    result.storeState(startState);
    result.addStartingState(startState.getID());

    //  Add to result: transitions from resultStartingState to all startingStates of *this
    for (auto& stateId : m_d->startingStates)
    {
        result.setTransition(startState.getID(), EPS, stateId);
    }

    //  Add to result: transitions from resultStartingState to all startingStates of rhs
    for (auto& stateId : rhs.m_d->startingStates)
    {
        result.setTransition(startState.getID(), EPS, stateId);
    }

    //  Create resultEndingState
    State endState;
    result.storeState(endState);
    result.addAcceptingStates(endState.getID());

    //  Add to result: transitions from all endingStates of *this to resultEndingState.
    for (auto& stateId : m_d->endingStates)
    {
        result.setTransition(stateId, EPS, endState.getID());
    }

    //  Add to result: transitions from all endingStates of rhs to resultEndingState.
    for (auto& stateId : rhs.m_d->endingStates)
    {
        result.setTransition(stateId, EPS, endState.getID());
    }

    return result;
}

NFATransitionTable NFATransitionTable::opConcat(const NFATransitionTable &rhs) const
{
    // *this must have one endingState and rhs must have one starting state.
    assert(m_d->endingStates.size() == 1);
    assert(rhs.m_d->startingStates.size() == 1);

    NFATransitionTable result;

    // Depending on the uniqueness of State we will add the states from *this and rhs to result.

    //  Add all transition of *this to result
    copyAllTransitions(result, *this);

    //  Add all transition of rhs to result.
    copyAllTransitions(result, rhs);

    // TODO Possible Optimization: Special case for transitions starting at the startingState of rhs.
    // The from part of these transitions will be replaced by the endingState *this when being added to result.

    // Concat
    StateId endingStateThis = *m_d->endingStates.begin();
    StateId startingStateRHS = *rhs.m_d->startingStates.begin();
    result.setTransition(endingStateThis, EPS, startingStateRHS);

    // Set startingState of result to be the starting state of *this.
    result.setStartingStates(m_d->startingStates);

    // Set endingState of result to be the ending state of rhs.
    result.setAcceptingStates(rhs.m_d->endingStates);

    return result;
}

NFATransitionTable NFATransitionTable::opPlus() const
{
    // *this must have one endingState and rhs must have one starting state.
    NFATransitionTable result = opConcat(opStar());

    return result;
}

NFATransitionTable NFATransitionTable::opStar() const
{
    // *this must have one endingState and rhs must have one starting state.
    assert(m_d->startingStates.size() == 1);
    assert(m_d->endingStates.size() == 1);

    NFATransitionTable result;
    State startingState;
    State endingState;
    result.storeState(startingState);
    result.storeState(endingState);
    result.addStartingState(startingState.getID());
    result.addAcceptingStates(endingState.getID());

    StateId oldStartingState = *m_d->startingStates.begin();
    StateId oldEndingState = *m_d->endingStates.begin();

    //  Add all transition of *this to result
    copyAllTransitions(result, *this);

    result.setTransition(
        oldEndingState,
        EPS,
        oldStartingState);


    result.setTransition(
        startingState.getID(),
        EPS,
        oldStartingState);

    result.setTransition(
        oldEndingState,
        EPS,
        endingState.getID());

    result.setTransition(
        startingState.getID(),
        EPS,
        endingState.getID());

    return result;
}


NFATransitionTable NFATransitionTable::mergeNFAs(const std::vector<NFATransitionTable>& nfas)
{
    NFATransitionTable result;
    
    // Depending on the uniqueness of State we will add the states from *this and rhs to result.

    //  Add all transition of nfas to result
    for (NFATransitionTable nfa : nfas)
    {
        copyAllTransitions(result, nfa);
    }

    //  Create resultStartingState
    State startState;
    result.storeState(startState);
    result.addStartingState(startState.getID());


    //  Add to result: transitions from resultStartingState to all startingStates of nfas
    for (NFATransitionTable nfa : nfas)
    {
        for (auto &stateId : nfa.m_d->startingStates)
        {
            result.setTransition(startState.getID(), EPS, stateId);
        }
    }

    //  Add to result: ending states of nfas to result ending states
    for (NFATransitionTable nfa : nfas)
    {
        for (auto &stateId : nfa.m_d->endingStates)
        {
            result.addAcceptingStates(stateId);
        }
    }

    return result;
}

