#ifndef Rule_H
#define Rule_H


struct Rule
{
    std::string type;
    std::string regex;


    Rule(std::string t, const std::string &re)
        : type(t), regex(re)
    {
    }
};

#endif // Rule_H