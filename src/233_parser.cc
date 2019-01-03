//
// Created by lyx on 19-1-2.
//

#include <233_variable.h>
#include "233_lang.h"

using namespace lang233;
using namespace std;

static inline string parse_slash(string str)
{
    switch (str[0])
    {
        case 'n':
            str[0] = '\n';
            break;
        case 't':
            str[0] = '\t';
            break;
        default:
            break; // TODO: more special char
    }

    return str;
}

inline string Parser::get_quote_string(t_iterator &token, bool dquote)
{
    string text;
    ++token;
    auto type = dquote ? T_DQUOTE : T_QUOTE;

    while (token->type != type)
    {
        if (token->type == T_SLASH)
        {
            ++token;
            text.append(parse_slash(token->text));
        }
        else
        {
            text.append(token->text);
        }

        ++token;
    }

    return text;
}

inline bool Parser::check_func_or_var_name(const string &name)
{
    for (const auto &c : name)
    {
        if (unlikely(c < '0' || (c > '9' && c < 'A') || (c > 'Z' && c < 'a' && c != '_') || c > 'z'))
        {
            return false;
        }
    }

    return true;
}

inline bool Parser::find(t_iterator &token, enum token_type t_type)
{
    if (token->type == TOKEN_END)
    {
        return false;
    }

    do
    {
        ++token;
    } while (token->type == T_WHITESPACE || token->type == T_NEWLINE);

    return token->type == t_type;
}

inline bool Parser::require_find(t_iterator &token, enum token_type t_type)
{
    if (unlikely(!find(token, t_type)))
    {
        error(E_FATAL, "expect " + get_token_name(t_type) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }

    return true;
}

inline void Parser::func_declare_handler(t_iterator &token)
{
    require_find(token, T_LITERAL);
    if (!check_func_or_var_name(token->text))
    {
        error(E_FATAL, "function name only can use [a-zA-Z0-9_], but got " + token->text, file, token->start, token->line);
    }

    auto func_name = token->text;
    auto func = new Func(func_name);
    require_find(token, T_LPARENTHESIS);

    bool require_type = false;
    while (token->type != T_RPARENTHESIS)
    {
        //TODO:arg name can't be same
        if ((require_type && require_find(token, T_TYPENAME)) || find(token, T_TYPENAME))
        {
            require_type = false;
            auto type = get_type(token->text);
            require_find(token, T_LITERAL);
            auto name = token->text;
            Val val(type);

            if (find(token, T_ASSIGN))
            {

                if (find(token, T_QUOTE))
                {
                    val.set_val(get_quote_string(token, false));
                }
                else if (token->type == T_DQUOTE)
                {
                    val.set_val(get_quote_string(token, true));
                }
                else if (token->type == T_CONST_NUM || token->type == T_BOOL)
                {
                    val.set_val(token->text, token->type);
                }
                else
                {
                    DEBUG_OUTPUT("unexpect token");
                    error(E_FATAL, "unexpect token " + get_token_name(token->type), file, token->start, token->line);
                }
            }

            func->args.emplace_back(name, val);
        }

        if (token->type == T_COMMA)
        {
            require_type = true;
        }
    }

    require_find(token, T_LBRACE);
    auto func_begin = ++token;
    size_t brace = 1;

    for (;;)
    {
        if (token->type == TOKEN_END)
        {
            break;
        }

        if (token->type == T_LBRACE)
        {
            ++brace;
        }

        if (token->type == T_RBRACE)
        {
            if (--brace == 0)
            {
                break;
            }
        }

        ++token;
    }

    if (brace != 0)
    {
        error(E_FATAL, "expect " + get_token_name(T_RBRACE) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }

    if (func_begin != token)
    {
        parse(func_begin, token, func, "");
    }

    Lang233G::func.insert(func_name, func);
}

inline void Parser::var_declare_handler(t_iterator &token, Func *func)
{
    //TODO:can't use name same as function arg
    auto &op_array = func->op_array;

    auto type = get_type(token->text);
    require_find(token, T_LITERAL);
    auto name = token->text;
    if (!check_func_or_var_name(token->text))
    {
        error(E_FATAL, "variable name only can use [a-zA-Z0-9_], but got " + token->text, file, token->start, token->line);
    }
    Val val(type);

    Val op_val(TYPE_STRING);
    op_val.set_val(name);
    OPNode op1(OPNODE_VAR, op_val);
    op_array.emplace_back(OP_DECLARE_VAR, op1);

    if (find(token, T_ASSIGN))
    {
        Val op2_val(type);
        if (find(token, T_QUOTE))
        {
            op2_val.set_val(get_quote_string(token, false));
        }
        else if (token->type == T_DQUOTE)
        {
            op2_val.set_val(get_quote_string(token, true));
        }
        else if (token->type == T_CONST_NUM || token->type == T_BOOL)
        {
            op2_val.set_val(token->text, token->type);
        }
        else if (token->type == T_LITERAL)
        {
            auto func_name = token->text;
            require_find(token, T_LBRACE);
            //TODO: call func.
        }
        else
        {
            DEBUG_OUTPUT("unexpect token");
            error(E_FATAL, "unexpect token " + get_token_name(token->type), file, token->start, token->line);
        }

        //TODO: support express like 'int a = 1 + 2 + 3;'
        require_find(token, T_CODELINEEND);

        OPNode op2(OPNODE_IMM, op2_val);
        op_array.emplace_back(OP_ASSIGN_VAR, op1, op2);
    }
    else if (token->type != T_CODELINEEND)
    {
        error(E_FATAL, "expect " + get_token_name(T_CODELINEEND) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }

    auto var = new Variable(name, val);
    if (!func->vars.insert(name, var))
    {
        error(E_FATAL, "can\'t redeclare variable " + name, file, token->start, token->line);
    }
}

inline void Parser::call_func_handler(t_iterator &token, const string &name, Func *func)
{
    //TODO:support arg
    Val op1_val(TYPE_STRING);
    op1_val.set_val(name);
    OPNode op1(OPNODE_IMM, op1_val);

    if (find(token, T_RPARENTHESIS))
    {
        func->op_array.emplace_back(OP_CALL_FUNC, op1);
        require_find(token, T_CODELINEEND);
        return;
    }

    Val op2_val(TYPE_FUNC_ARG);

    for (;;)
    {
        //TODO:support express. func(1+2+3); add a 233_express.h to parse it.
        Val arg_val;

        switch (token->type)
        {
            case T_BOOL:
                arg_val.type = TYPE_BOOL;
                arg_val.set_val(token->text);
                break;

            case T_CONST_NUM:
                arg_val.type = TYPE_INT;
                arg_val.set_val(token->text);
                break;

            case T_QUOTE:
                arg_val.type = TYPE_STRING;
                arg_val.val.string = new std::string(get_quote_string(token, false));
                break;

            case T_DQUOTE:
                arg_val.type = TYPE_STRING;
                arg_val.val.string = new std::string(get_quote_string(token, true));
                break;

            case T_LITERAL:
                // call func or variable, it's a express. parse to more opcode.use temp variable.use type none and runtime get type.(in vm.cc)
                break;

            default:
                goto end_loop;
                break;
        }

        op2_val.val.func_args->emplace_back(arg_val);
        if (!find(token, T_COMMA))
        {
            goto end_loop;
        }

        if (find(token, T_RPARENTHESIS))
        {
            // ) after ,
            error(E_FATAL, "unexpect token " + get_token_name(T_RPARENTHESIS), file, token->start, token->line);
        }
    }

    end_loop:
    if (token->type != T_RPARENTHESIS)
    {
        error(E_FATAL, "expect " + get_token_name(T_RPARENTHESIS) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }

    require_find(token, T_CODELINEEND);

    OPNode op2(OPNODE_ARG, op2_val);
    func->op_array.emplace_back(OP_CALL_FUNC, op1, op2);
}

bool Parser::parse(t_iterator t, const t_iterator &end, Func *func, const string &p_file)
{
    if (!p_file.empty())
    {
        if (!file.empty())
        {
            error(E_WARNING, "A parser can\'t parse two file at the same time.", p_file, 0, 0);
            return false;
        }

        file = p_file;
    }

    while (t != end && t->type != TOKEN_END)
    {
        switch (t->type)
        {
            case T_FUNC:
            {
                // new function
                func_declare_handler(t);
                break;
            }

            case T_TYPENAME:
            {
                var_declare_handler(t, func);
                break;
            }

            case T_LITERAL:
            {
                //TODO:support use arg like call_func(1, 2, 3);
                auto name = t->text;
                if (find(t, T_LPARENTHESIS))
                {
                    // call_func()
                    call_func_handler(t, name, func);
                }
                else if (t->type == T_ASSIGN)
                {
                    // a = 1

                }
                else
                {

                }
                break;
            }

            case T_WHITESPACE:
            case T_NEWLINE:
            {
                // no effect
                break;
            }

            default:
            {
                DEBUG_OUTPUT("unexpect token");
                error(E_FATAL, "unexpect token " + get_token_name(t->type), file, t->start, t->line);
                break;
            }
        }

        ++t;
    }

    if (!p_file.empty())
    {
        file = "";
    }

    return true;
}

bool Parser::parse(const TokenArray &t_vector, Func *func, const string &p_file)
{
    return parse(t_vector.cbegin(), t_vector.cend(), func, p_file);
}