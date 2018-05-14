#include "DFATransitionTable.h"

#define RAPIDJSON_HAS_STDSTRING 1

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"

#include <assert.h>


struct DFATransitionTable::impl
{
    StateId startingStateId;
    std::set<StateId> endingStateIds;

    std::map < StateId, std::map<char, StateId > > table;

    std::map< StateId, State > states;
};

DFATransitionTable::DFATransitionTable()
    : m_d(std::make_unique<impl>())
{

}

DFATransitionTable::DFATransitionTable(const DFATransitionTable & t)
    : m_d(std::make_unique<impl>())
{
    m_d->table = t.m_d->table;
    m_d->startingStateId = t.m_d->startingStateId;
    m_d->endingStateIds = t.m_d->endingStateIds;
    m_d->states = t.m_d->states;
}

DFATransitionTable::~DFATransitionTable()
{

}


void DFATransitionTable::storeState(const State &s)
{
    m_d->states[s.getID()] = s;
}

State DFATransitionTable::getState(StateId stateId) const
{
    auto  it = m_d->states.find(stateId);
    assert(it != m_d->states.end());
    return it->second;
}

std::map<StateId, State> DFATransitionTable::getStates() const
{
    return m_d->states;
}

std::set<StateId> DFATransitionTable::getStateIds() const
{
    std::set<StateId> stateIds;
    for (auto & pair : m_d->states)
    {
        stateIds.insert(pair.first);
    }
    return stateIds;
}

void DFATransitionTable::setTransition(StateId fromId, char input, StateId toId)
{
    assert(m_d->states.find(fromId) != m_d->states.end());
    assert(m_d->states.find(toId) != m_d->states.end());

    m_d->table[fromId][input] = toId;
}


StateId DFATransitionTable::move(StateId stateId, char input) const
{
    auto it = m_d->table.find(stateId);
    assert(it != m_d->table.end());

    auto inputIt = it->second.find(input);
    assert(inputIt != it->second.end());

    return inputIt->second;
}

bool DFATransitionTable::hasTransition(StateId stateId, char input) const
{
    auto it = m_d->table.find(stateId);
    if (it == m_d->table.end())
    {
        return false;
    }

    auto inputIt = it->second.find(input);
    if (inputIt == it->second.end())
    {
        return false;
    }

    return true;
}


void DFATransitionTable::setStartingState(StateId stateId)
{
    assert(m_d->states.find(stateId) != m_d->states.end());

    m_d->startingStateId = stateId;
}


StateId DFATransitionTable::getStartingState() const
{
    return m_d->startingStateId;
}


void DFATransitionTable::addAcceptingState(StateId stateId)
{
    assert(m_d->states.find(stateId) != m_d->states.end());

    m_d->endingStateIds.insert(stateId);
}

void DFATransitionTable::setAcceptingStates(const std::set<StateId> &stateIds)
{
    for (auto &stateId : stateIds)
    {
        assert(m_d->states.find(stateId) != m_d->states.end());
    }

    m_d->endingStateIds = stateIds;
}

std::set<StateId> DFATransitionTable::getAcceptingStateIds() const
{
    return m_d->endingStateIds;
}


bool DFATransitionTable::isAcceptingState(StateId stateId) const
{
    return m_d->endingStateIds.find(stateId) != m_d->endingStateIds.end();
}


std::map<char, StateId> DFATransitionTable::getTransitions(StateId stateId) const
{
    assert(m_d->states.find(stateId) != m_d->states.end());
    return m_d->table[stateId];
}


std::ostream& operator<<(std::ostream& out, const DFATransitionTable &transitionTable)
{
    using namespace rapidjson;
    Document d;
    d.SetObject();
    Document::AllocatorType& allocator = d.GetAllocator();

    d.AddMember("states", Value(), allocator);
    d["states"].SetArray();
    for (auto statePair : transitionTable.m_d->states)
    {
        Value state;
        state.SetObject();
        state.AddMember("id", statePair.second.getID(), allocator);
        state.AddMember("tokenType", statePair.second.getTokenType(), allocator);
        d["states"].PushBack(state, allocator);
    }

    d.AddMember("tt", Value(), allocator);
    d["tt"].SetArray();
    for (const std::pair<StateId, std::map<char, StateId> > & transitionTableRow : transitionTable.m_d->table)
    {
        Value row;
        row.SetObject();

        row.AddMember("currentState", transitionTableRow.first, allocator);

        for (const std::pair<char, State> &inputResultPair : transitionTableRow.second)
        {
            std::string charString(1, inputResultPair.first);
            Value input(charString.c_str(), charString.size(), allocator);
            row.AddMember(input, inputResultPair.second.getID(), allocator);
        }

        d["tt"].PushBack(row, allocator);
    }

    d.AddMember("startState", transitionTable.getStartingState(), allocator);

    Value acceptingStates;
    acceptingStates.SetArray();

    for (auto & stateId : transitionTable.getAcceptingStateIds())
    {
        acceptingStates.PushBack(stateId, allocator);
    }

    d.AddMember("acceptingStates", acceptingStates, allocator);

    OStreamWrapper osw(out);
    PrettyWriter<OStreamWrapper> writer(osw);
    d.Accept(writer);

    return out;
}

std::istream& operator >> (std::istream& in, DFATransitionTable &transitionTable)
{
    using namespace rapidjson;

    IStreamWrapper isw(in);

    Document d;
    d.ParseStream(isw);

    if (!(d.HasMember("states") && d.HasMember("tt") && d.HasMember("startState") && d.HasMember("acceptingStates")))
        return in;


    for (auto& stateObj : d["states"].GetArray())
    {
        State state(stateObj["id"].GetInt());
        state.setTokenType(stateObj["tokenType"].GetString());
        transitionTable.storeState(state);
    }

    for (auto& row : d["tt"].GetArray())
    {
        StateId currentState(row["currentState"].GetInt());

        for (auto& m : row.GetObject())
        {
            std::string name(m.name.GetString());
            if (name != "currentState")
            {
                char input = name[0];
                StateId nextState(m.value.GetInt());
                transitionTable.setTransition(currentState, input, nextState);
            }
        }
    }

    transitionTable.setStartingState(d["startState"].GetInt());

    for (auto & value : d["acceptingStates"].GetArray())
    {
        transitionTable.addAcceptingState(value.GetInt());
    }

    return in;
}
