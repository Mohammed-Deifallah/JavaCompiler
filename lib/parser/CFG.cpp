//
// Created by Hosam Elsafty on 4/20/18.
//
#include "CFG.h"
#include "Symbol.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <string>
#include "string_function.h"
#include "Production.h"

using namespace std;

CFG::CFG() = default;

CFG::CFG(const std::string &file_name)
{
    // read from input file .
    std::ifstream is(file_name);
    if (is)
    {
        std::string str;

        is.seekg(0, std::ios::end);
        str.reserve(is.tellg());
        is.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(is)),
            std::istreambuf_iterator<char>());

        parsing_file_into_rules(str);
    }
    else
    {
        std::cout << "error: only " << is.gcount() << " could be read";
    }
}

void CFG::setStartSymbol(Symbol symbol)
{
    start_symbol = symbol;
}

Symbol CFG::getStartSymbol()const {
    return start_symbol;
}

void CFG::addProduction(Production production) {
    allProduction.push_back(production);
    nonTerminal.insert(production.lhs());
    vector<Symbol> rhs = production.rhs();
    for (auto &rh : rhs) {
        if (rh.isTerminal())terminal.insert(rh);
    }
}

void CFG::deleteProduction(const Production production) {

    int index = static_cast<int>(distance(allProduction.begin(), find(allProduction.begin(), allProduction.end(), production)));
    allProduction.erase(allProduction.begin() + index);
}

CFG &CFG::operator=(const CFG &other) = default;

std::vector<Production> CFG::getProductions(const Symbol &symbol) const {
    std::vector<Production> result;
    for (auto &i : allProduction) {
        if (i.lhs() == symbol) result.push_back(i);
    }
    return result;
}

std::vector<Production> CFG::getAllProductions() const {
    return allProduction;
}

std::vector<Symbol> CFG::getNonTerminals() const {
    std::vector<Symbol> nonTerminalVector(nonTerminal.begin(), nonTerminal.end());
    return nonTerminalVector;
}

std::vector<Symbol> CFG::getTerminals() const {
    std::vector<Symbol> terminalVector(terminal.begin(), terminal.end());
    return terminalVector;
}

void CFG::parsing_file_into_rules(string file) {
    vector<string> rules;
    split(file, rules, '#');
    for (int i = 0; i < rules.size(); ++i) {
        replaceAll(rules[i], "\t", " ");
        replaceAll(rules[i], "\n", " ");
        removeSpaces(trim(rules[i]));
        parsing_rules_into_symbol(rules[i], i == 1);
    }

}

void CFG::parsing_rules_into_symbol(string &rule, bool start) {
    if (rule.empty())return;
    //find equal operator
    size_t equal_index = rule.find('=');
    if (equal_index == string::npos) { // did not find assign op
        cout << "Error invalid rule , it must contain '=' ." << endl;
        return;
    }

    string lhs = rule.substr(0, equal_index);
    string rhs = rule.substr(equal_index + 1);
    // check valid RHS, LHS
    lhs = trim(lhs);
    rhs = trim(rhs);
    if (lhs.size() == 0) {
        cout << "Error invalid rule , it must contain left hand side" << endl;
    }
    if (rhs.size() == 0) {
        cout << "Error invalid rule , it must contain right hand side" << endl;
    }
    if (rhs[0] == '|') {
        cout << "Error invalid rule , it must contain valid left hand side" << endl;
    }
    if (start) {
        start_symbol.setType(false);
        start_symbol.setValue(lhs);
    }
    Symbol lhs_symbol(lhs, false);
    nonTerminal.insert(lhs_symbol);
    vector<Symbol> rhs_symbols;
    vector<string> rhs_string;
    split(rhs, rhs_string, '|');
    for (auto &i : rhs_string) {
        i = trim(i);
        vector<string> inner_symbol_string;
        vector<Symbol> inner_symbol;
        split(i, inner_symbol_string, ' ');
        for (auto &j : inner_symbol_string) {
            if (j[0] == '\'' &&
                    j[j.length() - 1] == '\'') {
                j = j.substr(1, j.length() - 2);
                Symbol rhs_symbol(j, true);
                terminal.insert(rhs_symbol);
                inner_symbol.push_back(rhs_symbol);
            } else {
                Symbol rhs_symbol(j, false);
                inner_symbol.push_back(rhs_symbol);
            }
        }
        Production production(lhs_symbol, inner_symbol);
        addProduction(production);

    }

}