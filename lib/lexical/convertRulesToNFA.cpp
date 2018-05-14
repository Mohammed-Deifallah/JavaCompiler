#include "convertRulesToNFA.h"
#include "RulesReader.h"

#include <stack>
#include <fstream>
#include <assert.h>

using namespace std;

NFATransitionTable nfaOfRule(const Rule &rule, int priority);
NFATransitionTable nfaOfRegex(const string &regex);

NFATransitionTable convertRulesToNFA(const std::string &filename)
{
    RulesReader rulesReader;
    std::vector<Rule> rules = rulesReader.process(filename);

    vector<NFATransitionTable> nfas;
    for (int i=0; i<rules.size(); ++i)
    {
        NFATransitionTable nfa = nfaOfRule(rules[i], rules.size() - i);
        nfas.push_back(nfa);
    }

    return NFATransitionTable::mergeNFAs(nfas);
}

NFATransitionTable nfaOfRule(const Rule &rule, int priority)
{
    NFATransitionTable nfa = nfaOfRegex(rule.regex);

    std::set<StateId> states = nfa.getAcceptingStates();
    for (auto &stateId : states)
    {
        State acceptingState(stateId);
        acceptingState.setTokenType(rule.type);
        acceptingState.setPriority(priority);
        nfa.storeState(acceptingState);
    }

    return nfa;
}


struct node
{
    bool isOp;
    NFATransitionTable nfa;
    char op;

    node(const NFATransitionTable &n) : isOp(false), nfa(n) {}
    node(char o) : isOp(true), op(o) {}
};

NFATransitionTable charNFA(char c);
void windUpLastUnion(stack<node> &nodeStack);
void windupPreviousConcats(stack<node> &nodeStack);

NFATransitionTable nfaOfRegex(const string &regex)
{
    std::stack<node> nodeStack;

    bool backslash = false;
    for (size_t i = 0; i < regex.size(); ++i)
    {
        backslash = false;
        switch (regex[i])
        {
        case ' ':
            break;
        case '\\':
            backslash = true;
            ++i;
        default:
        {
            char ch = regex[i];
            if (backslash && ch == 'L')
            {
                ch = EPS;
            }

            NFATransitionTable tempNFA = charNFA(ch);
            //if (nodeStack.size())
            //{
            //    if (!nodeStack.top().isOp)
            //    {
            //        tempNFA = nodeStack.top().nfa.opConcat(tempNFA);
            //        nodeStack.pop();
            //    }
            //}
            nodeStack.push(node(tempNFA));
            break;
        }
        case '|':
            assert(nodeStack.size() && !nodeStack.top().isOp);

            windupPreviousConcats(nodeStack);
            windUpLastUnion(nodeStack); // find previous '|' and process it

            nodeStack.push(node('|'));
            break;
        case '*':
        {
            assert(nodeStack.size() && !nodeStack.top().isOp);

            NFATransitionTable temp = nodeStack.top().nfa.opStar();
            nodeStack.pop();
            nodeStack.push(node(temp));
            break;
        }
        case '+':
        {
            assert(nodeStack.size() && !nodeStack.top().isOp);

            NFATransitionTable temp = nodeStack.top().nfa.opPlus();
            nodeStack.pop();
            nodeStack.push(node(temp));
            break;
        }
        case '(':
            nodeStack.push(node('('));
            break;
        case ')':
            assert(nodeStack.size() && !nodeStack.top().isOp);

            windupPreviousConcats(nodeStack);
            windUpLastUnion(nodeStack); // find previous '|' and process it

            if (nodeStack.size() >= 2)
            {
                node temp = nodeStack.top();
                nodeStack.pop();
                assert(nodeStack.top().isOp && nodeStack.top().op == '(');
                nodeStack.pop();

                //if (nodeStack.size())
                //{
                //    if (!nodeStack.top().isOp)
                //    {
                //        temp.nfa = nodeStack.top().nfa.opConcat(temp.nfa);
                //        nodeStack.pop();
                //    }
                //}

                nodeStack.push(temp);
            }
            break;
        }
    }

    windupPreviousConcats(nodeStack);
    windUpLastUnion(nodeStack);

    if (nodeStack.empty())
    {
        return NFATransitionTable();
    }
    else
    {
        assert(nodeStack.size() == 1 && !nodeStack.top().isOp);
        return nodeStack.top().nfa;
    }
}


NFATransitionTable charNFA(char c)
{
    NFATransitionTable nfa;
    State s, f;
    nfa.storeState(s);
    nfa.storeState(f);
    nfa.setTransition(s.getID(), c, f.getID());
    nfa.addStartingState(s.getID());
    nfa.addAcceptingStates(f.getID());
    return nfa;
}


void windupPreviousConcats(stack<node> &nodeStack)
{
    assert(nodeStack.size() && !nodeStack.top().isOp);
    while (nodeStack.size() >= 2)
    {
        node top = nodeStack.top();
        nodeStack.pop();
        if (!nodeStack.top().isOp)
        {
            node prevTop = nodeStack.top();
            nodeStack.pop();
            NFATransitionTable tempConcat = prevTop.nfa.opConcat(top.nfa);
            nodeStack.push(node(tempConcat));
        }
        else
        {
            nodeStack.push(top);
            break;
        }
    }
}


void windUpLastUnion(stack<node> &nodeStack)
{
    assert(nodeStack.size() && !nodeStack.top().isOp);
    if (nodeStack.size() >= 3)
    {
        node top = nodeStack.top();
        nodeStack.pop();
        if (nodeStack.top().isOp && nodeStack.top().op == '|')
        {
            nodeStack.pop();
            NFATransitionTable unionNFA = nodeStack.top().nfa.opUnion(top.nfa);
            nodeStack.pop();
            nodeStack.push(unionNFA);
        }
        else
        {
            nodeStack.push(top);
        }
    }
}





