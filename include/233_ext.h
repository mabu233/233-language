//
// Created by lyx on 19-1-3.
//

#ifndef INC_233_LANGUAGE_233_EXT_H
#define INC_233_LANGUAGE_233_EXT_H

#include <vector>

#include "233_lang.h"

#define LANG233_FUNC_PREFIX lang233_func_
#define LANG233_FUNC(func_name) extern "C" void LANG233_FUNC_PREFIX##func_name\
    (VarArray &lang233_func_args, Variable *return_value)
#define LANG233_GET_FUNC_ARGS(func_name) arginfo_##LANG233_FUNC_PREFIX##func_name
#define LANG233_GET_FUNC(func_name) LANG233_FUNC_PREFIX##func_name, LANG233_GET_FUNC_ARGS(func_name)
#define LANG233_GET_FUNC_EX(func_name) #func_name, LANG233_GET_FUNC(func_name)

#define LANG233_FUNC_ARGS(func_name) lang233::VarArray LANG233_GET_FUNC_ARGS(func_name);

#define LANG233_FUNC_ARG_INT(func_name, arg_name) \
    do {\
        lang233::Val arg_val(lang233::TYPE_INT);\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_FUNC_ARG_STRING(func_name, arg_name) \
    do {\
        lang233::Val arg_val(lang233::TYPE_STRING);\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_FUNC_ARG_BOOL(func_name, arg_name) \
    do {\
        lang233::Val arg_val(lang233::TYPE_BOOL);\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_FUNC_ARG_INT_EX(func_name, arg_name, default_value) \
    do {\
        lang233::Val arg_val(lang233::TYPE_INT);\
        arg_val.val.int64 = default_value;\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_FUNC_ARG_STRING_EX(func_name, arg_name, default_value) \
    do {\
        lang233::Val arg_val(lang233::TYPE_STRING);\
        arg_val.set_val(default_value);\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_FUNC_ARG_BOOL_EX(func_name, arg_name, default_value) \
    do {\
        lang233::Val arg_val(lang233::TYPE_BOOL);\
        arg_val.val.boolean = default_value;\
        LANG233_GET_FUNC_ARGS(func_name).emplace_back(#arg_name, arg_val);\
    } while (0);

#define LANG233_PARSE_ARG() \
    lang233::VarTable lang233_func_arg_table;\
    for (int i = 0; i < lang233_func_args.size(); ++i)\
    {\
        lang233_func_arg_table.insert(&lang233_func_args[i]);\
    }

#define LANG233_GET_ARG_BOOL(arg_name) \
        lang233_func_arg_table.get(#arg_name)->val.val.boolean

#define LANG233_GET_ARG_INT(arg_name) \
        lang233_func_arg_table.get(#arg_name)->val.val.int64

#define LANG233_GET_ARG_STRING(arg_name) \
        lang233_func_arg_table.get(#arg_name)->val.val.string

static lang233_inline void lang233_register_func(const std::string &func_name, lang233::lang233_func_t func, const lang233::VarArray &args)
{
    auto ext_func = new lang233::Func(func_name, func);
    ext_func->args = args;
    lang233::Lang233G::func.insert(func_name, ext_func);
}

#endif //INC_233_LANGUAGE_233_EXT_H
