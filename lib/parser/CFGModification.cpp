#include "CFGModification.h"
#include <algorithm>
#include <iterator>
using namespace std;


#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

//bool is_cycle(Production prod);

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCDFAInspection"

int lastInd = 0;

CFG removeLeftFactoring(const CFG &cfg) {
    CFG new_cfg = cfg;
    bool flag = true;
    //continue the loop when at a change happened
    cout << "Left factor: " << endl;
    while (flag) {
        flag = false;
        vector<Symbol> nonTerm = new_cfg.getNonTerminals();
        for (auto &nt : nonTerm) {
            //get list of productions of the current non-terminal
            vector<Production> prods = new_cfg.getProductions(nt);
            for (int i = 0; i < prods.size() - 1; i++) {
                Production prod = prods[i];
                /*cout << prod.lhs().getValue() << " -> ";
                for (auto &sym : prod.rhs()) {
                    cout << sym.getValue() << "  ";
                }
                cout << endl;*/
                vector<int> common;
                //Try to get the number of productions with the same start as the current production
                for (int j = i + 1; j < prods.size(); j++) {
                    if (!prod.rhs().empty() && !prods[j].rhs().empty() && prod.rhs()[0] == prods[j].rhs()[0]) {
                        common.push_back(j);
                    }
                }
                //no one
                if (common.empty()) {
                    continue;
                }
                //a change happened
                flag = true;
                bool stop = true;
                //the index of the last common symbol from start
                int threshold = 0;
                for (int j = 1; j < prod.rhs().size() && stop; j++) {
                    for (int k : common) {
                        Production pr = prods[k];
                        if (pr.rhs().size() > j && pr.rhs()[j] == prod.rhs()[j]) {
                            continue;
                        }
                        //reach a different symbol, then break the loop
                        stop = false;
                        threshold = j - 1;
                        break;
                    }
                }
                //the whole current production is a common prefix
                if (stop) {
                    threshold = static_cast<int>(prod.rhs().size() - 1);
                }
                vector<Symbol> prefix;
                for (int u = 0; u <= threshold; u++) {
                    prefix.push_back(prod.rhs()[u]);
                }
                string s = nt.getValue() + to_string(lastInd++);
                Symbol new_sym(s, false);
                prefix.push_back(new_sym);
                new_cfg.addProduction(Production(nt, prefix));

                bool eps = false;
                for (int k : common) {
                    //the whole production is a prefix
                    if (threshold == prods[k].rhs().size() - 1) {
                        if (!eps) {
                            eps = true;
                            Symbol epsilon(std::string("\\\\L"), true);
                            vector<Symbol> v;
                            v.push_back(epsilon);
                            new_cfg.addProduction(Production(new_sym, v));
                        }
                        new_cfg.deleteProduction(prods[k]);
                        continue;
                    }
                    //else, assign what remains from the production to the new symbol
                    vector<Symbol> post;
                    for (int u = threshold + 1; u < prods[k].rhs().size(); u++) {
                        post.push_back(prods[k].rhs()[u]);
                    }
                    new_cfg.addProduction(Production(new_sym, post));
                    new_cfg.deleteProduction(prods[k]);
                }
                vector<Symbol> post;
                for (int u = threshold + 1; u < prod.rhs().size(); u++) {
                    post.push_back(prod.rhs()[u]);
                }
                if (post.empty())
                {
                    Symbol epsilon(std::string("\\\\L"), true);
                    post.push_back(epsilon);
                }
                new_cfg.addProduction(Production(new_sym, post));
                new_cfg.deleteProduction(prod);
                break;
            }
        }
    }
    return new_cfg;
}

#pragma clang diagnostic pop

#pragma clang diagnostic pop

CFG removeLeftRecursion( CFG &cfg) {
    CFG new_cfg = cfg;
    vector<Production> all = cfg.getAllProductions();
    for (auto &prod : all) {
        new_cfg.deleteProduction(prod);
    }
    vector<Symbol> nonTerminals = cfg.getNonTerminals();
    cout << "Left recursion: ";
    /*for(auto &nt : nonTerminals){
        cout << nt.getValue() << " ";
    }*/
    cout << endl;
    for (unsigned long long i = 0; i < nonTerminals.size(); i++) {
        Symbol cur_sym = nonTerminals[i];
        //cout << "cur_sym value: " << cur_sym.getValue() << endl;
        vector<Production> productions = cfg.getProductions(cur_sym);
        for (unsigned long long j = 0; j < i; j++) {
            Symbol prev_sym = nonTerminals[j];
            //cout << "prev_sym value :" << prev_sym.getValue() << endl;
            for (auto &production : productions) {
                if (production.lhs() == cur_sym) {
                    vector<Symbol> rhs = production.rhs();
                    if (rhs.empty() || rhs[0] != prev_sym) {
                        continue;
                    }
                    vector<Symbol> remaining_symbols;
                    vector<Production> j_productions = cfg.getProductions(prev_sym);
                    for (unsigned long long k = 1; k < rhs.size(); k++) {
                        remaining_symbols.push_back(rhs[k]);
                    }
                    for (auto &prev_prod : j_productions) {
                        vector<Symbol> new_rhs = prev_prod.rhs();
                        for (auto &sym : remaining_symbols) {
                            new_rhs.push_back(sym);
                        }
                        vector<Symbol> new_rhs2;
                        for (auto &sym : new_rhs) {
                            if (sym.getValue() == "\\\\L") {
                                continue;
                            }
                            new_rhs2.push_back(sym);
                        }
                        if (!new_rhs.empty() && new_rhs[new_rhs.size() - 1].getValue() == "\\\\L") {
                            new_rhs2.emplace_back("\\\\L");
                        }
                        Production new_prod(cur_sym, new_rhs2);
                        new_cfg.addProduction(new_prod);
                    }
                    cfg.deleteProduction(production);
                }
            }
        }

        //Immediate left-recursion
        bool recur = false;
        for (unsigned long long j = 0; j < productions.size(); j++) {
            Production prod = productions[j];
            if (prod.rhs().empty() || prod.rhs()[0] != cur_sym/* || is_cycle(prod)*/) {
                continue;
            }
            recur = true;
            string s = cur_sym.getValue() + to_string(lastInd++);
            Symbol new_sym(s, false);
            Symbol eps(std::string("\\\\L"), true);
            for (unsigned long long k = 0; k < productions.size(); k++) {
                if (k == j) {
                    continue;
                }
                vector<Symbol> product = productions[k].rhs();
                product.push_back(new_sym);
                Production new_prod(cur_sym, product);
                new_cfg.addProduction(new_prod);
            }
            vector<Symbol> new_rhs = prod.rhs();
            new_rhs.erase(new_rhs.begin());
            new_rhs.push_back(new_sym);
            new_cfg.addProduction(Production(new_sym, new_rhs));
            new_rhs.clear();
            new_rhs.push_back(eps);
            new_cfg.addProduction(Production(new_sym, new_rhs));
        }
        if (!recur) {
            for (auto &prod : cfg.getProductions(cur_sym)) {
                new_cfg.addProduction(prod);
            }
        }
    }
    return new_cfg;
}

/*bool is_cycle(Production prod) {
    for (auto &sym : prod.rhs()) {
        if (sym != prod.lhs()) {
            return false;
        }
    }
    return true;
}*/





CFG removeLeftRecursion2(const CFG &cfg)
{
    static const Symbol epsSymbol(string("\\\\L"), true);
    static const vector<Symbol> epsSymbolString{ epsSymbol };

    CFG newCFG;

    const auto && nonTerminals = cfg.getNonTerminals();

    for (size_t i = 0; i < nonTerminals.size(); ++i)
    {
        const Symbol Ai = nonTerminals[i];
        auto AiProductions = cfg.getProductions(Ai);
        decltype(AiProductions) newAiProductions;
        for (auto && production : AiProductions)
        {
            auto rhs = production.rhs();
            bool replaced = false;
            for (size_t j = 0; j < i; ++j)
            {
                const Symbol Aj = nonTerminals[j];
                if (rhs[0] == Aj)
                {
                    rhs.erase(rhs.begin());
                    auto AjProductions = newCFG.getProductions(Aj);
                    for (auto && jProduction : AjProductions)
                    {
                        decltype(rhs) newRHS = jProduction.rhs();
                        newRHS.insert(newRHS.end(), rhs.begin(), rhs.end());
                        newAiProductions.push_back(Production(Ai, newRHS));
                    }
                    replaced = true;
                    break;
                }
            }

            if (!replaced)
            {
                newAiProductions.push_back(production);
            }
        }

        // Immediate Left Recusion
        decltype(AiProductions) startWithA;
        decltype(AiProductions) notStartWithA;
        for (auto && production : newAiProductions)
        {
            if (production.rhs()[0] == Ai)
            {
                startWithA.push_back(production);
            }
            else
            {
                notStartWithA.push_back(production);
            }
        }

        if (startWithA.size())
        {
            Symbol Ai_(Ai.getValue() + "_", Ai.isTerminal());

            for (auto && production : notStartWithA)
            {
                auto rhs = production.rhs();
                rhs.push_back(Ai_);
                newCFG.addProduction(Production(Ai, rhs));
            }

            for (auto && production : startWithA)
            {
                auto rhs = production.rhs();
                rhs.erase(rhs.begin());
                rhs.push_back(Ai_);
                newCFG.addProduction(Production(Ai_, rhs));
            }

            newCFG.addProduction(Production(Ai_, epsSymbolString));
        }
        else
        {
            for (auto && production : notStartWithA)
            {
                newCFG.addProduction(production);
            }
        }
    }

    newCFG.setStartSymbol(cfg.getStartSymbol());

    return newCFG;
}

//
//CFG removeLeftFactoring(const CFG &cfg)
//{
//    CFG newCFG = cfg;
//
//    const auto && nonTerminals = cfg.getNonTerminals();
//
//    for (auto && nonTerminal : nonTerminals)
//    {
//        auto productions = cfg.getProductions(nonTerminal);
//        decltype(productions) newProductions;
//
//        for (int i = 0; i < productions.size(); ++i)
//        {
//            auto && iRHS = productions[i].rhs();
//
//            for (int j = i + 1; j < productions.size(); ++j)
//            {
//                auto && jRHS = productions[i].rhs();
//                int index = -1;
//                while (index+1 < iRHS.size() && index+1 < jRHS.size()
//                    && iRHS[index + 1] == jRHS[index + 1])
//                {
//                    index++;
//                }
//
//                if (index >= 0)
//                {
//                    // Replace
//
//                }
//            }
//
//        }
//
//
//    }
//    return newCFG;
//}
