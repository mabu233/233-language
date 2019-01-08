//
// Created by lyx on 19-1-3.
//

#ifndef INC_233_LANGUAGE_233_EXT_H
#define INC_233_LANGUAGE_233_EXT_H

#include <vector>

#include "233_lang.h"

#define LANG233_FUNC_PREFIX lang233_func_
#define LANG233_FUNC(func_name) extern "C" void LANG233_FUNC_PREFIX##func_name\
    (lang233::VarArray &lang233_func_args, lang233::Variable *return_value)

#define LANG233_FUNC_BEGIN() \
    do {\
        bool __optional = false;\
        lang233::VarArray __args;

#define LANG233_FUNC_END(func_name) \
        lang233_register_func(#func_name, LANG233_FUNC_PREFIX##func_name, __args);\
    } while (0);

#define LANG233_FUNC_ALIAS(func_name, alias) \
    do {\
        auto func = lang233::Lang233G::func.get(#func_name);\
        lang233_register_func(#alias, LANG233_FUNC_PREFIX##func_name, func->args);\
    } while (0)

#define LANG233_FUNC_ARG_OPTIONAL() __optional = true

#define LANG233_FUNC_ARG(arg_name) \
    do {\
        lang233::Val __arg_val;\
        __args.emplace_back(#arg_name, __arg_val);\
    } while (0)

#define LANG233_PARSE_ARG_BEGIN() \
    std::vector<std::string> __lang233_arg_string_tmp;\
    do {\
        lang233::VarTable lang233_func_arg_table;\
        for (int i = 0; i < lang233_func_args.size(); ++i)\
        {\
            lang233_func_arg_table.insert(&lang233_func_args[i]);\
        }

#define LANG233_PARSE_ARG_END() \
    } while (0);

#define LANG233_GET_ARG_BOOL(arg_name, variable) \
        do {\
            auto __var = lang233_func_arg_table.get(#arg_name);\
            if (!__var)\
            {\
                printf("Error, %s doesn't exist.\n", #arg_name);\
                exit(1);\
            }\
            if (__var->val.type != TYPE_NONE)\
            {\
                variable = __var->val.to_bool();\
            }\
        } while (0)

#define LANG233_GET_ARG_INT(arg_name) \
        do {\
            auto __var = lang233_func_arg_table.get(#arg_name);\
            if (!__var)\
            {\
                printf("Error, %s doesn't exist.\n", #arg_name);\
                exit(1);\
            }\
            if (__var->val.type != TYPE_NONE)\
            {\
                variable = __var->val.to_int();\
            }\
        } while (0)

#define LANG233_GET_ARG_STRING(arg_name, variable) \
        do {\
            auto __var = lang233_func_arg_table.get(#arg_name);\
            if (!__var)\
            {\
                printf("Error, %s doesn't exist.\n", #arg_name);\
                exit(1);\
            }\
            switch (__var->val.type)\
            {\
                case TYPE_STRING:\
                    variable = __var->val.val.string;\
                    break;\
                case TYPE_INT:\
                case TYPE_BOOL:\
                    __lang233_arg_string_tmp.emplace_back(__var->val.to_string());\
                    variable = &__lang233_arg_string_tmp.back();\
                    break;\
                default:\
                    break;\
            }\
        } while (0)

#define LANG233_GET_ARG(arg_name, variable) \
        do {\
            auto __var = lang233_func_arg_table.get(#arg_name);\
            if (!__var)\
            {\
                printf("Error, %s doesn't exist.\n", #arg_name);\
                exit(1);\
            }\
            if (__var->val.type != TYPE_NONE)\
            {\
                variable = __var;\
            }\
        } while (0)

static lang233_inline void lang233_register_func(const std::string &func_name, lang233::lang233_func_t func, const lang233::VarArray &args)
{
    auto ext_func = new lang233::Func(func_name, func);
    ext_func->args = args;
    lang233::Lang233G::func.insert(func_name, ext_func);
}

#endif //INC_233_LANGUAGE_233_EXT_H
