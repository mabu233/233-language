//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_VARIABLE_H
#define INC_233_LANGUAGE_233_VARIABLE_H

#include <string>
#include <unordered_map>

#include "233_type.h"
#include "233_token.h"

namespace lang233
{
    class Variable;
    typedef std::vector<Variable> VarArray;

    class Val
    {
    public:
        union
        {
            int64_t int64;
            bool boolean;
            std::string *string;
            VarArray *func_args;
        } val;

        enum type type;

        Val() : type(TYPE_NONE)
        {

        }

        explicit Val(enum type _type) : type(_type)
        {
            if (_type == TYPE_STRING)
            {
                val.string = new std::string;
            }
            else if (_type == TYPE_FUNC_ARG)
            {
                val.func_args = new VarArray;
            }
            else
            {
                val.int64 = 0;
            }
        }

        Val(const Val &_val) : type(_val.type)
        {
            if (_val.type == TYPE_STRING)
            {
                val.string = new std::string(*_val.val.string);
            }
            else if (_val.type == TYPE_FUNC_ARG)
            {
                val.func_args = new VarArray(*_val.val.func_args);
            }
            else
            {
                val = _val.val;
            }
        }

        ~Val()
        {
            if (type == TYPE_STRING)
            {
                delete val.string;
            }
            else if (type == TYPE_FUNC_ARG)
            {
                delete val.func_args;
            }
        }

        inline void set_val(const std::string &_val, enum token_type _val_token_type = T_LITERAL)
        {
            switch (type)
            {
                case TYPE_INT:
                    val.int64 = (int64_t) strtoll(_val.c_str(), nullptr, 0);
                    break;

                case TYPE_BOOL:
                    if (_val_token_type == T_BOOL)
                    {
                        val.boolean = _val == "true";
                    }
                    else if (_val_token_type == T_CONST_NUM)
                    {
                        val.boolean = _val != "0";
                    }
                    else
                    {
                        val.boolean = !_val.empty();
                    }
                    break;

                case TYPE_STRING:
                    val.string->assign(_val);
                    break;

                case TYPE_FUNC_ARG:
                    break;

                default:
                    break;
            }
        }

        inline void set_val(const VarArray &_val)
        {
            if (likely(type == TYPE_FUNC_ARG))
            {
                val.func_args->assign(_val.begin(), _val.end());
            }
        }

        inline void set_val(const Val &_val)
        {
            if (type != _val.type)
            {
                set_val(_val.to_string(), _val.type == TYPE_BOOL ? T_BOOL : T_LITERAL);
                return;
            }

            if (type == TYPE_STRING)
            {
                val.string->assign(*_val.val.string);
            }
            else if (type == TYPE_FUNC_ARG)
            {
                val.func_args->assign(_val.val.func_args->begin(), _val.val.func_args->end());
            }
            else
            {
                val = _val.val;
            }
        }

        inline std::string to_string() const
        {
            switch (type)
            {
                case TYPE_STRING:
                    return *val.string;

                case TYPE_INT:
                    return std::to_string(val.int64);

                case TYPE_BOOL:
                    return val.boolean ? "true" : "false";

                default:
                    return "";
            }
        }
    };

    class Variable
    {
    public:
        std::string name;
        Val val;

        Variable() = default;

        explicit Variable(const Val &_val) : val(_val)
        {

        }

        explicit Variable(std::string _name) : name(std::move(_name))
        {

        }

        Variable(std::string _name, const Val &_val)
                : name(std::move(_name)), val(_val)
        {

        }
    };

    class VarTable
    {
    public:
        inline Variable *get(const std::string &var_name)
        {
            auto iter = variables.find(var_name);
            if (iter == variables.end())
            {
                return nullptr;
            }
            else
            {
                return iter->second;
            }
        }

        inline bool insert(const std::string &var_name, Variable *var)
        {
            auto iter = variables.find(var_name);
            if (iter != variables.end())
            {
                return false;
            }

            variables[var_name] = var;
            ++num;
            return true;
        }

        inline size_t get_num()
        {
            return num;
        }

    private:
        size_t num = 0;
        std::unordered_map<std::string, Variable*> variables;
    };
}

#endif //INC_233_LANGUAGE_233_VARIABLE_H
