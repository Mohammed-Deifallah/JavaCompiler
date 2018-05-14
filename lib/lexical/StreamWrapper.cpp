#include "StreamWrapper.h"
#include <algorithm>
#include <iterator>

StreamWrapper::StreamWrapper(std::istream & in)
    : _in(in), _pos(0)
{
}

char StreamWrapper::getNext()
{
    char ch = EOF;

    if (_remaining.size())
    {
        ch = _remaining.front();
        _remaining.pop_front();
    }
    else if(_in.peek() != EOF)
    {
        _in.get(ch);
    }

    ++_pos;
    return ch;
}

void StreamWrapper::putFront(std::string str)
{
    _pos -= str.size();
    std::move(str.begin(), str.end(), std::front_inserter(_remaining));
}

int StreamWrapper::getPos() const
{
    return _pos;
}
