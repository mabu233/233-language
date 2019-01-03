//
// Created by lyx on 19-1-2.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <233_variable.h>

#include "233_ext.h"

using namespace lang233;
using namespace std;

FuncTable Lang233G::func;
VarTable Lang233G::vars;

LANG233_FUNC(load_ext)
{
    LANG233_PARSE_ARG()
    auto ext_name = LANG233_GET_ARG_STRING(ext_name);
    if (ext_name->empty())
    {
        return;
    }

    DEBUG_OUTPUT("load ext: %s", ext_name->c_str());
}

int main(int argc, char **argv)
{
    LANG233_FUNC_ARGS(load_ext)
    LANG233_FUNC_ARG_STRING(load_ext, ext_name)

    lang233_register_func(LANG233_GET_FUNC_EX(load_ext));

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
    string root_func(file);
    root_func.append("@main");
    if (Lang233G::func.get(root_func))
    {
        error(E_FATAL, "root function " + root_func + " already exists.", file, 0, 0);
    }

    auto func = new Func(root_func);
    if (!parser.parse(scanner.t_vector, func, file))
    {
        exit(1);
    }

    Lang233G::func.insert(root_func, func);
    VarArray root_arg;
    auto ret = VM::run(root_func, root_arg);

    return (int) ret.val.val.int64;
}