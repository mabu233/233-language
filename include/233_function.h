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
        lang233_func_t pointer;

        explicit Func(std::string f_name) : name(std::move(f_name)), type(FUNC_USER), pointer(nullptr)
        {
            DEBUG_OUTPUT("Create a function %s", name.c_str());
        }

        Func(std::string f_name, lang233_func_t f_pointer) : name(std::move(f_name)), pointer(f_pointer), type(FUNC_INTERNAL)
        {

        }
    };

    class FuncTable
    {
    public:
        inline Func *get(const std::string &func_name)
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

        inline bool insert(const std::string &func_name, Func *func)
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
