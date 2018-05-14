#include "TableCreator.h"
#include <algorithm>
#include <iostream>
#include <iterator>

std::string EPS = "\\\\L";
Symbol epsilon(EPS);
std::string END = "$";

/*debug.*/
void printSet(std::set<Symbol> set)
{
    std::cout << "{";
    for (auto &s : set)
    {
        std::cout << s.getValue() << ", ";
    }
    std::cout << "}" << std::endl;
}
void printVector(std::vector<Symbol> set)
{
    std::cout << "{";
    for (auto &s : set)
    {
        std::cout << s.getValue() << ", ";
    }
    std::cout << "}" << std::endl;
}
void printSetMap(std::map<Symbol, std::set<Symbol>> setMap)
{
    std::map<Symbol, std::set<Symbol>>::iterator it1;
    for (it1 = setMap.begin(); it1 != setMap.end(); it1++)
    {
        std::cout << it1->first.getValue() << ":";
        printSet(it1->second);
    }
}

TableCreator::TableCreator(CFG c) : cfg(c) {}

void merge(std::set<Symbol> &to, std::set<Symbol> &from, const Symbol &exclude)
{
    // std::cout << "merge and exc " << exclude.getValue() << std::endl;
    for (auto &s : from)
    {
        // std::cout << s.getValue() << " ";
        if (s != exclude)
        {
            to.insert(s);
        }
    }
    // std::cout << std::endl;
}

std::set<Symbol> TableCreator::findFirstRec(Symbol sym)
{
    std::map<Symbol, std::set<Symbol>>::iterator mapIt = firstMap.find(sym);
    if (mapIt != firstMap.end())
    {
        // std::cout << sym.getValue() << " is found " << mapIt->first.getValue() << std::endl;
        // printSet(mapIt->second);
        return mapIt->second;
    }
    std::set<Symbol> newSet;
    if (sym.isTerminal())
    {
        // std::cout << sym.getValue() << " is terminal" << std::endl;
        newSet.insert(sym);
        return newSet;
    }
    std::vector<Production> all = cfg.getProductions(sym);
    std::vector<Production>::iterator proIt;
    for (proIt = all.begin(); proIt != all.end(); proIt++) // looping on all productions.
    {
        std::vector<Symbol> nextPro = proIt->rhs();
        // std::cout << sym.getValue() << " prod ";
        // printVector(nextPro);
        std::set<Symbol> ans;
        int i;
        for (i = 0; i < nextPro.size(); i++)
        {
            // std::cout << sym.getValue() << " calling " << nextPro[i].getValue() << std::endl;
            ans = findFirstRec(nextPro[i]);
            // printSet(ans);
            std::set<Symbol>::iterator ansIt = ans.find(epsilon);
            if (ansIt == ans.end()) //eps is not in Yi
            {
                merge(newSet, ans, *(new Symbol()));
                // std::cout << "newSet of " << sym.getValue();
                // printSet(newSet);
                break; //a production has finished.
            }
            // std::cout << "EPS FOund" << std::endl;
            merge(newSet, ans, *ansIt); //merge all except epsolin.
        }
        /*
        *  Here i am adding epsolin if we reach the last symbol and its first set contain epsolin.
        *  because it means that all previous symbols must derive epsolin.
        */
        if (i == nextPro.size())
        {
            // std::cout << "adding eps to " << sym.getValue() << std::endl;
            std::set<Symbol>::iterator ansIt = ans.find(epsilon);
            if (ansIt != ans.end())
            {
                newSet.insert(*ansIt); //inserting epsolin.
            }
        }
    }
    return newSet;
}

std::set<Symbol> TableCreator::findFellowRec(Symbol sym, std::map<Symbol, bool> visit)
{
    std::map<Symbol, std::set<Symbol>>::iterator fellowIt = fellowMap.find(sym);
    if (fellowIt != fellowMap.end() && fellowIt->second.size() > 0)
    {
        // std::cout << sym.getValue() << " is found " << fellowIt->first.getValue() << std::endl;
        return fellowIt->second;
    }
    visit[sym] = true;
    std::set<Symbol> newSet;
    if (sym.isTerminal())
    {
        newSet.insert(sym);
        return newSet;
    }
    std::vector<Production> all = cfg.getAllProductions();
    std::vector<Production>::iterator productionIt;
    for (productionIt = all.begin(); productionIt != all.end(); productionIt++) // looping on all productions.
    {
        std::vector<Symbol> rhs = productionIt->rhs();
        std::vector<Symbol>::iterator symbolIt = find(rhs.begin(), rhs.end(), sym);
        if (symbolIt != rhs.end())
        {
            Symbol nonTerm = productionIt->lhs();
            if ((symbolIt + 1) == rhs.end())
            {
                if (!visit[nonTerm] && nonTerm != sym) //to avoid recursive calls incase of E >> aE .
                {
                    std::set<Symbol> ans = findFellowRec(nonTerm, visit);
                    merge(newSet, ans, *(new Symbol())); //merging fellow of lhs.
                }
                continue;
            }
            Symbol followingSymbol = *(++symbolIt);
            std::set<Symbol> firstOfFellow = firstMap[followingSymbol];
            // std::cout << sym.getValue() << " followed by " << followingSymbol.getValue() << " under "
            //           << nonTerm.getValue() << " ";
            // printSet(firstOfFellow);
            std::set<Symbol>::iterator it = firstOfFellow.find(epsilon);
            if (it != firstOfFellow.end())
            {
                // std::cout << "EPS found " << std::endl;
                std::set<Symbol> ans = findFellowRec(nonTerm, visit);
                merge(newSet, ans, *(new Symbol())); //merging fellow of lhs.
            }
            // std::cout << "EPS not found " << std::endl;
            merge(newSet, firstOfFellow, epsilon); //merging first of the following symbol.
        }
    }
    if (sym == cfg.getStartSymbol())
        newSet.insert(END);
    return newSet;
}

void TableCreator::findFirstSet()
{
    for (auto &s : cfg.getTerminals())
    {
        firstMap[s] = findFirstRec(s);
    }
    for (auto &s : cfg.getNonTerminals())
    {
        firstMap[s] = findFirstRec(s);
    }
}

void TableCreator::findFellowSet()
{
    for (auto &s : cfg.getNonTerminals())
    {
        std::map<Symbol, bool> visit;
        fellowMap[s] = findFellowRec(s, visit);
    }
}
std::set<Symbol> TableCreator::findRhsFirst(std::vector<Symbol> rhs)
{
    std::set<Symbol> ans;

    for (auto &sym : rhs)
    {
        ans.insert(firstMap[sym].begin(), firstMap[sym].end());
        if (firstMap[sym].find(epsilon) == firstMap[sym].end()) //no epsilon in the firt of this symbol
            break;                                              //stop adding.
    }
    return ans;
}
LL1Table TableCreator::createTable()
{
    findFirstSet();

    //std::cout << "printing first set." << std::endl;
    //printSetMap(firstMap);

    findFellowSet();

    //std::cout << "printing fellow set." << std::endl;
    //printSetMap(fellowMap);

    LL1Table ll1Table;
    ll1Table.setStartSymbol(cfg.getStartSymbol());

    for (auto &production : cfg.getAllProductions())
    {
        Symbol nonTerm = production.lhs();
        std::set<Symbol> first = firstMap[nonTerm];
        std::set<Symbol> fellow = fellowMap[nonTerm];
        std::set<Symbol> difference;
        std::set_difference(fellow.begin(), fellow.end(), first.begin(),
                            first.end(), std::inserter(difference, difference.end()));
        bool hasEPS = (first.find(epsilon) != first.end());
        std::set<Symbol> rhsFirst = findRhsFirst(production.rhs());
        for (auto &term : rhsFirst)
        {
            if (term.getValue() == EPS)
                continue;
            ll1Table.addProduction(nonTerm, term, production, false);
        }
        if (hasEPS && production.rhs().size() != 1 && production.rhs()[0] != epsilon)
            continue;
        for (auto &term : difference)
            ll1Table.addProduction(nonTerm, term, production, !hasEPS);
    }
    return ll1Table;
}
