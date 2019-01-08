//
// Created by lyx on 19-1-6.
//

#include "core.h"

using namespace lang233_core;
using namespace lang233;
using namespace std;

namespace lang233_core
{
    void output_init()
    {
        LANG233_FUNC_BEGIN()
            LANG233_FUNC_ARG(var);
        LANG233_FUNC_END(var_dump)

        LANG233_FUNC_BEGIN()
            LANG233_FUNC_ARG(str);
        LANG233_FUNC_END(print)
    }

    LANG233_FUNC(var_dump)
    {
        Variable *var = nullptr;
        LANG233_PARSE_ARG_BEGIN()
            LANG233_GET_ARG(var, var);
        LANG233_PARSE_ARG_END()

        switch (var->val.type)
        {
            case TYPE_STRING:
            {
                auto str = var->val.to_string();
                cout << "string(" << str.length() << ") \"" << str << "\"";
                break;
            }

            case TYPE_INT:
                cout << "int(" << var->val.to_string() << ")";
                break;

            case TYPE_BOOL:
                cout << "bool(" << var->val.to_string() << ")";
                break;

            default:
                cout << "unknown";
                break;
        }

        cout << endl;
    }

    LANG233_FUNC(print)
    {
        string *str = nullptr;
        LANG233_PARSE_ARG_BEGIN()
            LANG233_GET_ARG_STRING(str, str);
        LANG233_PARSE_ARG_END()

        cout << *str;
    }
}