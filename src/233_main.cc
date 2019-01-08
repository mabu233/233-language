//
// Created by lyx on 19-1-2.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <233_variable.h>

#include "core/core.h"

using namespace lang233;
using namespace std;

FuncTable Lang233G::func;
VarTable *Lang233G::vars = nullptr;

LANG233_FUNC(load_ext)
{
    string *ext_name = nullptr;
    LANG233_PARSE_ARG_BEGIN()
        LANG233_GET_ARG_STRING(ext_name, ext_name);
    LANG233_PARSE_ARG_END()

    if (!ext_name)
    {
        error(E_WARNING, "load_ext expect a variable", "", 0, 0);
        return;
    }

    DEBUG_OUTPUT("load ext: %s", ext_name->c_str());
}

int main(int argc, char **argv)
{
    lang233_core_init();

    LANG233_FUNC_BEGIN()
        LANG233_FUNC_ARG(ext_name);
    LANG233_FUNC_END(load_ext)

    if (argc != 2)
    {
        cout << "Invalid arguments" << endl;
        exit(1);
    }

    char *file_i = argv[1];
    char file[PATH_MAX];
    if (!realpath(file_i, file))
    {
        cout << "No such file" << endl;
        exit(1);
    }

    ifstream fs(file, ifstream::in);
    stringstream buffer;
    buffer << fs.rdbuf();
    string code = buffer.str();
    if (code.empty())
    {
        cout << "No code" << endl;
        exit(1);
    }

    Scanner scanner;
    if (!scanner.scan(code, file))
    {
        exit(1);
    }

    Parser parser;
    string root_func("@main");
    if (Lang233G::func.get(root_func))
    {
        error(E_FATAL, "root function " + root_func + " already exists.", file, 0, 0);
    }

    auto func = new Func(root_func);
    Lang233G::func.insert(root_func, func);
    Lang233G::vars = &func->vars;
    if (!parser.parse(scanner.t_vector, func))
    {
        exit(1);
    }

    VarArray root_arg;
    Val op1_val;
    op1_val.set_val(root_func, TYPE_STRING);
    OPNode op1(OPNODE_IMM, op1_val);
    Token call_func_token(T_LITERAL, root_func, 0, root_func.length(), 0, file);
    TokenArray call_func_token_l{call_func_token};
    OPCode call_func_op(OP_CALL_FUNC, op1, call_func_token_l.begin());
    auto ret = VM::run(root_func, root_arg, call_func_op);

    return (int) ret.val.val.int64;
}