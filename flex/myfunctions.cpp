#include "myfunctions.hpp"

#include <iostream>
#include <stack>
#include <string>
#include <map>

using namespace std;

const string Wrapper::INT = "int";
const string Wrapper::FLOAT = "float";

Wrapper::Wrapper() : index(0), address(0)
{
    init();
}

void Wrapper::init()
{
    op_size.insert(make_pair("goto", 3));
    op_size.insert(make_pair("ifgt", 3));
    op_size.insert(make_pair("ifge", 3));
    op_size.insert(make_pair("iflt", 3));
    op_size.insert(make_pair("ifle", 3));
    op_size.insert(make_pair("if_icmplt", 3));
    op_size.insert(make_pair("if_icmple", 3));
    op_size.insert(make_pair("if_icmpgt", 3));
    op_size.insert(make_pair("if_icmpge", 3));
    op_size.insert(make_pair("if_icmpeq", 3));
    op_size.insert(make_pair("if_icmpne", 3));
    op_size.insert(make_pair("ldc", 2));
}

void Wrapper::printAll()
{
    for (int i = 0; i < output.size(); i++)
    {
        cout << output[i] << endl;
    }
}
int Wrapper::get_size(string inst)
{
    if (op_size.find(inst) != op_size.end())
        return op_size.find(inst)->second;
    return 1;
}

// add string to the output stream
void Wrapper::addString(string bytecode)
{
    output.push_back(to_string(address) + ": " + bytecode);
    index++;
    address += get_size(bytecode.erase(bytecode.find_last_not_of(" \n\r\t") + 1));
}

// when declaration is encountered
void Wrapper::add_var(string id_lex, string type)
{
    static int local_var_num = 0;
    id_table.insert(make_pair(id_lex, make_pair(type, local_var_num)));
    local_var_num++;
}

string Wrapper::get_var_type(string id_lex)
{
    return id_table.find(id_lex)->second.first;
}

// when assignment is encountered
string Wrapper::store_value(string id_lex)
{
    dd("store_value" << endl);
    string Type = id_table.find(id_lex)->second.first;
    int local_var_num = id_table.find(id_lex)->second.second;
    dd("Type=" << Type << endl);
    dd("local_var_num=" << local_var_num << endl);
    if (Type == "int")
    {
        // optimize result
        if (local_var_num <= 3)
            addString("istore_" + to_string(local_var_num));
        else
            addString("istore " + to_string(local_var_num));
    }
    else if (Type == "float")
    {
        // optimize result
        if (local_var_num <= 3)
            addString("fstore_" + to_string(local_var_num));
        else
            addString("fstore " + to_string(local_var_num));
    }
    return output[index - 1];
}

// when arithmetic operation on variable
string Wrapper::load_var(string id_lex)
{
    string Type = id_table.find(id_lex)->second.first;
    int local_var_num = id_table.find(id_lex)->second.second;
    if (Type == INT)
    {
        // optimize result
        if (local_var_num <= 3)
            addString("iload_" + to_string(local_var_num));
        else
            addString("iload " + to_string(local_var_num));
    }
    else if (Type == FLOAT)
    {
        // optimize result
        if (local_var_num <= 3)
            addString("fload_" + to_string(local_var_num));
        else
            addString("fload " + to_string(local_var_num));
    }
    return output[index - 1];
}

string Wrapper::get_literal_type(string literal)
{
    if (literal.find(".") == std::string::npos)
        return INT;
    return FLOAT;
}

string Wrapper::load_literal(string literal)
{
    dd("load_literal" << endl);

    string Type = get_literal_type(literal);
    dd("Type=" << Type << endl);
    if (Type == INT)
    {
        // optimize
        int value = stoi(literal);
        dd("value=" << value << endl);
        if (value >= 0 && value <= 5)
            addString("iconst_" + literal);
        else if (value >= -128 && value <= 127)
            addString("bipush " + literal);
        else
            addString("sipush " + literal);
    }
    else if (Type == FLOAT)
    {
        addString("ldc " + literal);
    }
    else
    {
        dd("Unknown type" << endl);
    }

    dd("output.size = " << output.size()
                        << " index = " << index << endl);
    return output[index - 1];
}

string Wrapper::get_type(string first_operand, string second_operand)
{
    if (first_operand == INT && second_operand == INT)
        return INT;
    return FLOAT;
}

// cast expression from int float if necessary
string Wrapper::cast(string type_1, string type_2)
{
    if (type_1 == INT && type_2 == FLOAT)
    {
        dd("CASTING " << endl);
        addString("i2f");
        return output[index - 1];
    }
    return "";
}

// string Wrapper::add_relop(string relop, string type) {
//    if (type == INT) {
//        if (relop == "<")
//            addString("if_icmpge ");
//        else if (relop == ">")
//            addString("if_icmple ");
//        else if (relop == ">=")
//            addString("if_icmplt ");
//        else if (relop == "<=")
//            addString("if_icmpgt ");

//        jump_stack.push(index - 1);
//        return output[index - 1];
//    }
//    if (type == FLOAT) {
//        if (relop == "<") {
//            addString("fcmpg ");
//            addString("ifge ");
//        } else if (relop == ">") {
//            addString("fcmpl ");
//            addString("ifle ");
//        } else if (relop == ">=") {
//            addString("fcmpl ");
//            addString("iflt ");
//        } else if (relop == "<=") {
//            addString("fcmpg ");
//            addString("ifgt ");
//        }

//        jump_stack.push(index - 1);
//        return output[index - 2] + "\n" + output[index - 1];
//    }

// }

string Wrapper::add_relop(string relop, string type)
{
    if (type == INT)
    {
        if (relop == "<")
            addString("if_icmpge ");
        else if (relop == ">")
            addString("if_icmple ");
        else if (relop == ">=")
            addString("if_icmplt ");
        else if (relop == "<=")
            addString("if_icmpgt ");
        else if (relop == "==")
            addString("if_icmpne ");
        else if (relop == "!=")
            addString("if_icmpeq ");

        jump_stack.push(index - 1);
        return output[index - 1];
    }
    if (type == FLOAT)
    {
        if (relop == "<")
        {
            addString("fcmpg ");
            addString("ifge ");
        }
        else if (relop == ">")
        {
            addString("fcmpl ");
            addString("ifle ");
        }
        else if (relop == ">=")
        {
            addString("fcmpl ");
            addString("iflt ");
        }
        else if (relop == "<=")
        {
            addString("fcmpg ");
            addString("ifgt ");
        }
        else if (relop == "==")
        {
            addString("fcmpl ");
            addString("ifne ");
        }
        else if (relop == "!=")
        {
            addString("fcmpl ");
            addString("ifeq ");
        }

        jump_stack.push(index - 1);
        return output[index - 2] + "\n" + output[index - 1];
    }
}

void Wrapper::start_while()
{
    dd("start_while = " << index << endl);
    jump_stack.push(index);
}

string Wrapper::end_while()
{
    dd("end_while()" << endl);
    dd("jump_stack.size() = " << jump_stack.size() << endl);
    int condition_index = jump_stack.top();
    dd("condition_index = " << condition_index << endl);
    jump_stack.pop();
    int while_start = jump_stack.top();
    dd("while_start = " << while_start << endl);
    jump_stack.pop();
    int split_indx = output[while_start].find(":");
    addString("goto " + output[while_start].substr(0,split_indx));
    dd("output[condition_index] = " << output[condition_index] << endl);
    output[condition_index] = output[condition_index] + to_string(address);
    dd("index = " << index << endl);
    dd("output[index - 1] = " << output[index - 1] << endl);
    return output[index - 1];
}

string Wrapper::gen_goto()
{
    dd("gen_goto" << endl);
    addString("goto ");
    dd("jump_stack.size() = " << jump_stack.size() << endl);
    int i = jump_stack.top();
    dd("jump_stack.top() = " << i << endl);
    dd("output[i] " << output[i] << endl);
    jump_stack.pop();
    output[i] = output[i] + to_string(address);
    dd("output[i] again " << output[i] << endl);
    dd("index " << index << endl);
    dd("output[index - 1] " << output[index - 1] << endl);
    printAll();
    dd("After printAll " << index << endl);
    jump_stack.push(index - 1);
    dd("END gen_goto" << endl);

    return output[index - 1];
}

void Wrapper::end_else()
{
    dd("end_else" << endl);
    dd("jump_stack.size() = " << jump_stack.size() << endl);
    int i = jump_stack.top();
    dd("jump_stack.top() = " << i << endl);
    dd("output.size() = " << output.size() << endl);
    jump_stack.pop();
    dd("done jump_stack.pop() " << endl);
    // dd("output[i] " << output[i] << endl);
    output[i] = output[i] + to_string(address);
    dd("END end_else" << endl);
}

string Wrapper::add_uniary_op(char sign, string type)
{
    if (type == INT && sign == '-')
    {
        addString("ineg");
        return output[index - 1];
    }
    if (type == FLOAT && sign == '-')
    {
        addString("fneg");
        return output[index - 1];
    }

    return "";
}

string Wrapper::add_op(char op, string type)
{
    if (type == INT)
    {
        if (op == '+')
            addString("iadd");
        if (op == '-')
            addString("isub");
        if (op == '*')
            addString("imul");
        if (op == '/')
            addString("idiv");
    }
    if (type == FLOAT)
    {
        if (op == '+')
            addString("fadd");
        if (op == '-')
            addString("fsub");
        if (op == '*')
            addString("fmul");
        if (op == '/')
            addString("fdiv");
    }
    return output[index - 1];
}
