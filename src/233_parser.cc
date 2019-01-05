//
// Created by lyx on 19-1-2.
//

#include <stack>
#include <list>
#include <233_variable.h>

#include "233_variable.h"
#include "233_lang.h"

using namespace lang233;
using namespace std;

static lang233_inline string parse_slash(string str)
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

lang233_inline string Parser::get_quote_string(t_iterator &token, bool dquote)
{
    string text;
    ++token;
    auto type = dquote ? T_DQUOTE : T_QUOTE;

    while (token->type != type)
    {
        if (token->type == TOKEN_END)
        {
            error(E_FATAL, "expect token " + get_token_name(type) + " but got " + get_token_name(TOKEN_END)
                    , file, token->start, token->line);
        }

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

lang233_inline bool Parser::check_func_or_var_name(const string &name)
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

lang233_inline bool Parser::find(t_iterator &token, enum token_type t_type)
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

lang233_inline bool Parser::require_find(t_iterator &token, enum token_type t_type)
{
    if (unlikely(!find(token, t_type)))
    {
        error(E_FATAL, "expect " + get_token_name(t_type) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }

    return true;
}

lang233_inline void Parser::func_declare_handler(t_iterator &token)
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

lang233_inline void Parser::var_declare_handler(t_iterator &token, Func *func)
{
    auto &op_array = func->op_array;

    auto type = get_type(token->text);
    require_find(token, T_LITERAL);
    const auto &name = token->text;
    if (unlikely(!check_func_or_var_name(token->text)))
    {
        error(E_FATAL, "variable name only can use [a-zA-Z0-9_], but got " + token->text, file, token->start, token->line);
    }

    for (const auto &arg : func->args)
    {
        if (unlikely(arg.name == name))
        {
            error(E_FATAL, "variable name can't be same as function arg name", file, token->start, token->line);
        }
    }

    Val val(type);
    auto var = new Variable(name, val);
    if (!func->vars.insert(var))
    {
        error(E_FATAL, "can\'t redeclare variable " + name, file, token->start, token->line);
    }

    var_assign_handler(token, func, false);
}

lang233_inline void Parser::var_assign_handler(t_iterator &token, Func *func, bool require)
{
    const auto &name = token->text;

    if (unlikely(!func->vars.get(name) && !Lang233G::vars->get(name)))
    {
        error(E_FATAL, "use undeclare variable " + name, file, token->start, token->line);
    }

    if ((require && require_find(token, T_ASSIGN)) || find(token, T_ASSIGN))
    {
        Val op_val(TYPE_STRING);
        op_val.set_val(name);
        OPNode op1(OPNODE_VAR, op_val);

        find(token, T_LITERAL); // move to express
        auto op2 = parse_express(token, func, name);
        if (op2.type != OPNODE_NONE)
        {
            func->op_array.emplace_back(OP_ASSIGN_VAR, op1, op2);
        }
    }

    if (token->type != T_CODELINEEND)
    {
        error(E_FATAL, "expect " + get_token_name(T_CODELINEEND) + " but got " + get_token_name(token->type)
                , file, token->start, token->line);
    }
}

lang233_inline OPNode Parser::parse_express(t_iterator &token, Func *func, string assign_var_name)
{
    struct express_op
    {
        t_iterator token;
        OPNode val;
        uint8_t type; // 0: operand, 1: operator

        explicit express_op(const OPNode &_val) : type(0), val(_val)
        {

        }

        explicit express_op(const t_iterator &_token) : type(1), token(_token)
        {

        }
    };

    struct e_operator
    {
        t_iterator token;
        int8_t priority;

        e_operator(const t_iterator &_token, int8_t _priority) : token(_token), priority(_priority)
        {

        }

        explicit e_operator(int8_t _priority) : priority(_priority)
        {

        }
    };

    std::list<express_op> rpn;
    std::stack<e_operator> operators;
    operators.emplace(-1); // min priority element

    for (; ; ++token)
    {
        int8_t priority = -1;
        OPNode val;

        switch (token->type)
        {
            case T_ADD:
            case T_SUB:
                priority = 3;
                break;

            case T_MUL:
            case T_DIV:
            case T_MOD:
                priority = 4;
                break;

            case T_LPARENTHESIS:
            case T_RPARENTHESIS:
                priority = 0;
                break;

            case T_CONST_NUM:
                val.type = OPNODE_IMM;
                val.val.type = TYPE_INT;
                val.val.set_val(token->text);
                break;

            case T_BOOL:
                val.type = OPNODE_IMM;
                val.val.type = TYPE_BOOL;
                val.val.set_val(token->text, T_BOOL);
                break;

            case T_LITERAL:
                val.type = OPNODE_VAR;
                break;

            case T_QUOTE:
                val.type = OPNODE_IMM;
                val.val.type = TYPE_STRING;
                val.val.val.string = new string(get_quote_string(token, false));
                break;

            case T_DQUOTE:
                val.type = OPNODE_IMM;
                val.val.type = TYPE_STRING;
                val.val.val.string = new string(get_quote_string(token, true));
                break;

            case T_WHITESPACE:
            case T_NEWLINE:
                continue;

            default:
                goto end_loop;
        }

        if (val.type != OPNODE_NONE)
        {
            rpn.emplace_back(val);
            continue;
        }

        if (unlikely(priority == -1))
        {
            error(E_FATAL, "internal error", file, token->start, token->line);
            continue;
        }

        if (priority == 0)
        {
            if (token->type == T_LPARENTHESIS)
            {
                operators.emplace(token, priority);
            }
            else
            {
                // T_RPARENTHESIS
                for (;;)
                {
                    auto top = operators.top();
                    if (unlikely(top.priority == -1))
                    {
                        error(E_FATAL, "expect " + get_token_name(T_LPARENTHESIS), file, token->start, token->line);
                    }

                    const auto &t = top.token;
                    if (t->type == T_LPARENTHESIS)
                    {
                        operators.pop();
                        break;
                    }

                    rpn.emplace_back(t);
                    operators.pop();
                }
            }
        }
        else
        {
            for (;;)
            {
                auto top = operators.top();
                if (top.priority != -1 && top.token->type == T_LPARENTHESIS)
                {
                    operators.emplace(token, priority);
                    break;
                }
                else
                {
                    if (priority > top.priority)
                    {
                        operators.emplace(token, priority);
                        break;
                    }
                    else
                    {
                        rpn.emplace_back(top.token);
                        operators.pop();
                    }
                }
            }
        }
    }

    end_loop:
    if (operators.size() > 1)
    {
        for (;;)
        {
            auto top = operators.top();
            if (top.priority == -1)
            {
                break;
            }

            rpn.emplace_back(top.token);
            operators.pop();
        }
    }

    for (auto op = rpn.begin(); op != rpn.end(); ++op)
    {
        if (op->type == 1)
        {
            // operator
            auto end_iter = --rpn.begin();
            enum opcode_type op_type;

            switch (op->token->type)
            {
                case T_ADD:
                    op_type = OP_ADD;
                    break;

                case T_SUB:
                    op_type = OP_SUB;
                    break;

                case T_MUL:
                    op_type = OP_MUL;
                    break;

                case T_DIV:
                    op_type = OP_DIV;
                    break;

                case T_MOD:
                    op_type = OP_MOD;
                    break;

                default:
                    error(E_FATAL, "internal error", __FILE__, 0, __LINE__);
                    break;
            }

            auto operand2 = op;
            --operand2;
            auto operand1 = op;
            ----operand1;

            if (unlikely(operand2 == end_iter || operand1 == end_iter || operand2->type == 1 || operand1->type == 1))
            {
                error(E_FATAL, "express parse error", file, token->start, token->line);
            }

            if (op == --rpn.end())
            {
                if (rpn.size() != 3)
                {
                    error(E_FATAL, "express parse error", file, token->start, token->line);
                }

                if (assign_var_name.empty())
                {
                    auto tmp_var = func->get_tmp_var_name();
                    func->op_array.emplace_back(op_type, operand1->val, operand2->val, tmp_var);
                    operand2->val.type = OPNODE_VAR;
                    operand2->val.val.~Val();
                    operand2->val.val.type = TYPE_STRING;
                    operand2->val.val.val.string = new string(move(tmp_var));
                    return operand2->val;
                }
                else
                {
                    func->op_array.emplace_back(op_type, operand1->val, operand2->val, move(assign_var_name));
                    OPNode r_op;
                    return r_op;
                }
            }
            else
            {
                auto tmp_var = func->get_tmp_var_name();
                func->op_array.emplace_back(op_type, operand1->val, operand2->val, tmp_var);
                rpn.erase(op);
                rpn.erase(operand1);
                operand2->val.type = OPNODE_VAR;
                operand2->val.val.~Val();
                operand2->val.val.type = TYPE_STRING;
                operand2->val.val.val.string = new string(move(tmp_var));
                op = operand2;
            }
        }
    }

    auto op = rpn.begin();
    if (unlikely(rpn.size() != 1 || op->type != 0))
    {
        error(E_FATAL, "express parse error", file, token->start, token->line);
    }

    return op->val;
}

lang233_inline void Parser::call_func_handler(t_iterator &token, const string &name, Func *func)
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
                if (p_file.empty())
                {
                    // not in main function
                    error(E_FATAL, "Can't declare function in other function", file, t->start, t->line);
                }

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
