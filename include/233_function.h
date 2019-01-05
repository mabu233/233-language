//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_FUNCTION_H
#define INC_233_LANGUAGE_233_FUNCTION_H

#include <string>
#include <vector>

#include "233_opcode.h"
#include "233_variable.h"
#include "233_type.h"
#include "233_debug.h"

namespace lang233
{
    enum func_type
    {
        FUNC_INTERNAL = 0,
        FUNC_USER,
    };

    typedef void (*lang233_func_t)(VarArray &, Variable *return_value);

    class Func
    {
    public:
        uint8_t type;

        std::string name;
        OPArray op_array;
        VarArray args;
        VarTable vars;
        VarTable tmp_vars;
        lang233_func_t pointer;
        size_t tmp_var_id;

        explicit Func(std::string f_name)
            : name(std::move(f_name)), type(FUNC_USER), pointer(nullptr), tmp_var_id(0)
        {
            DEBUG_OUTPUT("Create a function %s", name.c_str());
        }

        Func(std::string f_name, lang233_func_t f_pointer)
            : name(std::move(f_name)), pointer(f_pointer), type(FUNC_INTERNAL), tmp_var_id(0)
        {

        }

        lang233_inline size_t get_tmp_var_id()
        {
            return tmp_var_id++;
        }

        /**
         * this function will insert a tmp variable to func->tmp_vars
         */
        lang233_inline std::string get_tmp_var_name()
        {
            auto name = "~" + std::to_string(get_tmp_var_id());
            auto var = new Variable(name);
            tmp_vars.insert(var);
            return name;
        }
    };

    class FuncTable
    {
    public:
        lang233_inline Func *get(const std::string &func_name)
        {
            auto iter = functions.find(func_name);
            if (iter == functions.end())
            {
                return nullptr;
            }
            else
            {
                return iter->second;
            }
        }

        lang233_inline bool insert(const std::string &func_name, Func *func)
        {
            auto iter = functions.find(func_name);
            if (iter != functions.end())
            {
                return false;
            }

            functions[func_name] = func;
            return true;
        }

    private:
        std::unordered_map<std::string, Func*> functions;
    };
}

#endif //INC_233_LANGUAGE_233_FUNCTION_H
