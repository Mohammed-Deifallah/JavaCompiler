#include "RulesReader.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <iterator>
#include <sstream>
#include <cctype>

using namespace std;

RulesReader::RulesReader()
{
}

RulesReader::~RulesReader()
{
}

string trim(const string &str);
string regexOf(string str, std::map<string, string> definitions);
string keywordRegex(const string &str, std::map<string, string> definitions);
string punctuationRegex(const string &str, std::map<string, string> definitions);


std::vector<Rule> RulesReader::process(const std::string & filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Unable to open file " << filename;
        return std::vector<Rule>();
    }

    vector<Rule> rules;
    map<string, string> definitions;

    string line;

    while (getline(file, line))
    {
        if (line.at(0) == '[')
        {
            // Punctuation
            string regex = punctuationRegex(line, definitions);
            rules.push_back(Rule("punctuation", regex));
        }
        else if (line.at(0) == '{')
        {
            // Keyword
            string regex = keywordRegex(line, definitions);
            rules.push_back(Rule("keyword", regex));
        }
        else
        {
            auto delimeter = find_if(line.begin(), line.end(), [](char c) {
                return !(std::isalpha(c) | std::isspace(c));
            });

            if (delimeter != line.end())
            {
                string lhs(line.begin(), delimeter);
                lhs = trim(lhs);
                string rhs(delimeter + 1, line.end());
                string regex = regexOf(rhs, definitions);
                regex = trim(regex);
                switch (*delimeter)
                {
                case ':': // Regular Expression
                    rules.push_back(Rule(lhs, regex));
                    break;
                case '=': // Regular Definition
                    definitions[lhs] = regex;
                    break;
                default:
                    // Unrecognized rule
                    break;
                }
            }
        }
    }

    file.close();

    return rules;
}


bool expandRange(string &str)
{
    size_t dashPos = -1;
    while ((dashPos = str.find('-', dashPos + 1)) != string::npos)
    {
        if (dashPos > 0 && dashPos < str.size() - 1)
        {
            if (isalnum(str[dashPos - 1]) && isalnum(str[dashPos + 1]))
            {
                if ((dashPos == 1 || !isalnum(str[dashPos - 2])) &&
                    (dashPos == str.size() - 2 || !isalnum(str[dashPos + 2])))
                {
                    if (str[dashPos - 1] < str[dashPos + 1])
                    {
                        string simpleRegex;
                        for (char c = str[dashPos - 1]; c <= str[dashPos + 1]; ++c)
                        {
                            simpleRegex += c;
                            simpleRegex += '|';
                        }

                        if (simpleRegex.size())
                        {
                            simpleRegex.erase(simpleRegex.size() - 1, 1);
                        }

                        str.replace(dashPos - 1, 3, '(' + simpleRegex + ')');
                    }
                    else
                    {
                        // Error:
                        // Return empty regex
                        return false;
                    }
                }
            }
        }
    }

    return true;
}



void find_and_replace_whole_word(string& source, string const& find, string const& replace)
{
    for (string::size_type i = 0; (i = source.find(find, i)) != string::npos;)
    {
        if ((i == 0 || !isalpha(source[i-1])) &&
            (i + find.size() == source.size() || !isalpha(source[i + find.size()])))
        {
            source.replace(i, find.length(), replace);
            i += replace.length();
        }
        else
        {
            i += find.length();
        }
    }
}

string regexOf(string str, std::map<string, string> definitions)
{
    for (auto &definition : definitions)
    {
        //size_t pos = str.find(definition.first, 0);
        find_and_replace_whole_word(str, definition.first, '(' + definition.second + ')');
    }

    if (!expandRange(str))
    {
        // Error in Range format
        // Return empty regex
        return "";
    }

    return str;
}

string keywordRegex(const string &str, std::map<string, string> definitions)
{
    int startIndex = str.find_first_of('{');
    int endIndex = str.find_last_of('}');

    if (startIndex == string::npos || endIndex == string::npos || endIndex < startIndex)
    {
        // Error: ill-formatted keyword rule
        // Return empty regex
        return "";
    }

    string inside(str.begin() + startIndex + 1, str.begin() + endIndex);

    // Splitting string by spaces
    std::istringstream iss(inside);
    std::vector<std::string> keywords(std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>());

    string regex;

    for (auto &keyword : keywords)
    {
        regex += keyword + '|';
    }

    if (regex.size()) // Remove last '|'
    {
        regex.erase(regex.size() - 1, 1);
    }

    return regex;
}

string punctuationRegex(const string &str, std::map<string, string> definitions)
{
    int startIndex = str.find_first_of('[');
    int endIndex = str.find_last_of(']');

    if (startIndex == string::npos || endIndex == string::npos || endIndex < startIndex)
    {
        // Error: ill-formatted punctuation rule
        // Return empty regex
        return "";
    }

    string inside(str.begin() + startIndex + 1, str.begin() + endIndex);

    string regex;
    bool backslash = false;

    for (char c : inside)
    {
        switch (c)
        {
        case '\\':
            backslash = true;
            break;
        case ' ':
            backslash = false;
            break;
        default:
            if (backslash)
            {
                regex += '\\';
            }
            regex += c;
            regex += '|';
            backslash = false;
        }
    }

    if (regex.size()) // Remove last '|'
    {
        regex.erase(regex.size() - 1, 1);
    }

    return regex;
}

string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

