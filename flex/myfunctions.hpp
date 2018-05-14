#ifndef myfunctions_H
#define myfunctions_H

#include <iostream>
#include <stack>
#include <string>
#include <vector>
#include <map>

using namespace std;

#define dd(x) \
    cout << x;

// #define dd(x) 


class Wrapper
{
private:
    vector<string> output;
    int index,address;
    map<string, int> op_size; // opcode, size, var num>
    stack<int> jump_stack;
    map<string, pair<string, int> > id_table; // var name, var type, var num>

    static const string INT;
    static const string FLOAT;
    void init();

public:
    Wrapper();

    //size of instruction.
    int get_size(string inst);
    
    void printAll();

    // add string to the output stream
    void addString(string bytecode);

    // when declaration is encountered
    void add_var(string id_lex, string type);

    string get_var_type(string id_lex);

    // when assignment is encountered
    string store_value(string id_lex);

    // when arithmetic operation on variable
    string load_var(string id_lex);

    string get_literal_type(string literal);

    string load_literal(string literal);

    string get_type(string first_operand, string second_operand);

    // cast expression from int float if necessary
    string cast(string type_1, string type_2);

    string add_relop(string relop, string type);

    void start_while();

    string end_while();

    string gen_goto();

    void end_else();

    string add_uniary_op(char sign, string type);

    string add_op(char op, string type);

};

#endif // myfunctions_H
