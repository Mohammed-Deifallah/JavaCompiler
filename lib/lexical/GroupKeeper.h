#ifndef GroupKeeper_H
#define GroupKeeper_H

#include <set>
#include <memory>

#include "State.h"

typedef std::set<StateId> Group;

class GroupKeeper
{
public:
    class Iterator;

    typedef Iterator iterator;
    typedef const Iterator const_iterator;

    GroupKeeper();
    ~GroupKeeper();

    Iterator addGroup(const Group &group);

    const_iterator getUnmarkedGroupOfmultiState() const;

    Iterator begin();
    Iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator group(StateId stateId) const;

    void mark(const_iterator it);
    void partition(const_iterator it, const Group &partition);

private:
    struct impl;
    std::unique_ptr<impl> m_d;
    friend class Iterator;
};

class GroupKeeper::Iterator
{
public:
    const Group &operator*() const;
    Group &operator*();
    void operator++();
private:
    Iterator(GroupKeeper& keeper, int index);
    Iterator(const GroupKeeper& keeper, int index);


    GroupKeeper *keeper;
    int index;
    friend class GroupKeeper;
    friend bool operator!=(GroupKeeper::Iterator it1, GroupKeeper::Iterator it2);
};

bool operator!=(GroupKeeper::Iterator it1, GroupKeeper::Iterator it2);

#endif // GroupKeeper_H