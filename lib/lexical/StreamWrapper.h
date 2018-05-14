#ifndef StreamWrapper_H
#define StreamWrapper_H

#include <iostream>
#include <string>
#include <deque>

class StreamWrapper
{
public:
    StreamWrapper(std::istream &in);

    char getNext();

    void putFront(std::string str);

    int getPos() const;

private:
    std::istream &_in;
    std::deque<char> _remaining;
    int _pos;
};

#endif // StreamWrapper_H