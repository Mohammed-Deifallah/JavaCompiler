#ifndef Token_H
#define Token_H

#include <string>
using namespace std;

class Token
{
public:
    string type;
    string value;

    Token(string t, string v)
    {
        type = t;
        value = v;
    };

    virtual ~Token()
    {

    };
};

#endif // Token_H