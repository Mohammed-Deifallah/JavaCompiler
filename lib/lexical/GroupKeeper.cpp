#include "GroupKeeper.h"

#include <algorithm>
#include <iterator>
#include <vector>
#include <assert.h>

struct GroupKeeper::impl
{
    std::vector<std::pair<Group, bool>> groups;
};

GroupKeeper::GroupKeeper()
    : m_d(std::make_unique<impl>())
{

}

GroupKeeper::~GroupKeeper()
{

}

GroupKeeper::Iterator GroupKeeper::addGroup(const Group &group)
{
    if (group.empty())
        return Iterator(*this, -1);

    for (auto &prevGroupPair : m_d->groups)
    {
        std::set<StateId> intersection;
        std::set_intersection(group.begin(), group.end(), prevGroupPair.first.begin(), prevGroupPair.first.end(),
            std::inserter(intersection, intersection.begin()));

        if (intersection.size())
            return Iterator(*this, -1);
    }

    m_d->groups.push_back(std::pair<Group,bool>(group,false));

    return Iterator(*this, m_d->groups.size() - 1);
}

GroupKeeper::const_iterator GroupKeeper::getUnmarkedGroupOfmultiState() const
{
    for (int i = 0; i < m_d->groups.size(); ++i)
    {
        auto & groupPair = m_d->groups[i];
        if (!groupPair.second && groupPair.first.size() > 1)
        {
            return Iterator(*this, i);
        }
    }

    return Iterator(*this, -1);
}


GroupKeeper::Iterator GroupKeeper::begin()
{
    if (m_d->groups.empty())
        return end();
    else
        return Iterator(*this, 0);
}

GroupKeeper::Iterator GroupKeeper::end()
{
    return Iterator(*this, -1);
}

GroupKeeper::const_iterator GroupKeeper::begin() const
{
    if (m_d->groups.empty())
        return end();
    else
        return Iterator(*this, 0);
}

GroupKeeper::const_iterator GroupKeeper::end() const
{
    return Iterator(*this, -1);
}


GroupKeeper::const_iterator GroupKeeper::group(StateId stateId) const
{
    for (int i = 0; i < m_d->groups.size(); ++i)
    {
        auto &groupPair = m_d->groups[i];
        if (groupPair.first.find(stateId) != groupPair.first.end())
        {
            return Iterator(*this, i);
        }
    }

    return Iterator(*this, -1);
}

void GroupKeeper::mark(GroupKeeper::const_iterator it)
{
    assert(it.index >= 0 && it.index < m_d->groups.size());
    m_d->groups[it.index].second = true;
}

void GroupKeeper::partition(GroupKeeper::const_iterator it, const Group &partition)
{
    const Group &group = m_d->groups[it.index].first;

    std::set<StateId> partitionOnly;
    std::set_difference(partition.begin(), partition.end(), group.begin(), group.end(),
        std::inserter(partitionOnly, partitionOnly.begin()));

    assert(partitionOnly.empty());

    std::set<StateId> difference;
    std::set_difference(group.begin(), group.end(), partition.begin(), partition.end(),
        std::inserter(difference, difference.begin()));

    m_d->groups[it.index] = std::pair<Group, bool>(difference, false);
    m_d->groups.push_back(std::pair<Group, bool>(partition, false));
}

GroupKeeper::Iterator::Iterator(GroupKeeper& keeper, int index)
    : keeper(&keeper), index(index)
{

}

GroupKeeper::Iterator::Iterator(const GroupKeeper& keeper, int index)
    : keeper(const_cast<GroupKeeper*>(&keeper)), index(index)
{

}

void GroupKeeper::Iterator::operator++()
{
    if (index + 1 < keeper->m_d->groups.size())
    {
        ++index;
    }
    else
    {
        index = -1;
    }
}

const Group &GroupKeeper::Iterator::operator*() const
{
    assert(index >= 0 && index < keeper->m_d->groups.size());
    return keeper->m_d->groups[index].first;
}

Group &GroupKeeper::Iterator::operator*()
{
    assert(index >= 0 && index < keeper->m_d->groups.size());
    return keeper->m_d->groups[index].first;
}


bool operator!=(GroupKeeper::Iterator it1, GroupKeeper::Iterator it2)
{
    return it1.keeper != it2.keeper || it1.index != it2.index;
}
