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
            val.int64 = 0;
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
            type = TYPE_NONE;
            copy(_val);
        }

        ~Val()
        {
            if (type == TYPE_STRING)
            {
                delete val.string;
                val.string = nullptr;
            }
            else if (type == TYPE_FUNC_ARG)
            {
                delete val.func_args;
                val.func_args = nullptr;
            }
        }

        Val &operator=(const Val &_val)
        {
            copy(_val);
            return *this;
        }

        lang233_inline void set_val(int64_t _val)
        {
            switch (type)
            {
                case TYPE_INT:
                    val.int64 = _val;
                    break;

                case TYPE_BOOL:
                    val.boolean = _val != 0;
                    break;

                case TYPE_STRING:
                    val.string->assign(std::to_string(_val));
                    break;

                case TYPE_NONE:
                    type = TYPE_INT;
                    val.int64 = _val;
                    break;

                default:
                    break;
            }
        }

        lang233_inline void set_val(const std::string &_val, enum token_type _val_token_type = T_LITERAL)
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

                case TYPE_NONE:
                    break;

                default:
                    break;
            }
        }

        lang233_inline void set_val(const VarArray &_val)
        {
            if (likely(type == TYPE_FUNC_ARG))
            {
                val.func_args->assign(_val.begin(), _val.end());
            }
        }

        lang233_inline void set_val(const Val &_val)
        {
            if (type == TYPE_NONE)
            {
                if (_val.type == TYPE_NONE)
                {
                    return;
                }

                copy(_val);
                return;
            }

            if (type != _val.type)
            {
                if (type == TYPE_STRING)
                {
                    val.string->assign(_val.to_string());
                }
                else if (type == TYPE_INT)
                {
                    val.int64 = _val.to_int();
                }
                else if (type == TYPE_BOOL)
                {
                    val.boolean = _val.to_bool();
                }

                // can't convert TYPE_FUNC_ARG

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

        lang233_inline std::string to_string() const
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

        lang233_inline int64_t to_int() const
        {
            switch (type)
            {
                case TYPE_STRING:
                    return (int64_t) strtoll(val.string->c_str(), nullptr, 0);

                case TYPE_INT:
                    return val.int64;

                case TYPE_BOOL:
                    return val.boolean ? 1 : 0;

                default:
                    return 0;
            }
        }

        lang233_inline bool to_bool() const
        {
            switch (type)
            {
                case TYPE_STRING:
                    return !val.string->empty();

                case TYPE_INT:
                    return val.int64 != 0;

                case TYPE_BOOL:
                    return val.boolean;

                default:
                    return false;
            }
        }

    private:
        lang233_inline void copy(const Val &_val)
        {
            this->~Val();

            type = _val.type;
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
        std::unordered_map<std::string, Variable*> variables;
        std::vector<std::unordered_map<std::string, Variable*>::iterator> v_vector;

        lang233_inline Variable *get(const std::string &var_name) const
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

        lang233_inline bool insert(Variable *var)
        {
            const auto &var_name = var->name;

            auto iter = variables.find(var_name);
            if (iter != variables.end())
            {
                return false;
            }

            v_vector.push_back(variables.emplace(var_name, var).first);
            return true;
        }

        lang233_inline void clear()
        {
            variables.clear();
            v_vector.clear();
        }

        lang233_inline size_t get_num()
        {
            return v_vector.size();
        }
    };

    typedef std::vector<VarTable*> VarScope;
}

#endif //INC_233_LANGUAGE_233_VARIABLE_H
