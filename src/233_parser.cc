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
            error(E_FATAL, "expect " + get_token_name(type) + " but got " + get_token_name(TOKEN_END)
                    , token->file, token->start, token->line);
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
                , token->file, token->start, token->line);
    }

    return true;
}

lang233_inline void Parser::func_declare_handler(t_iterator &token)
{
    require_find(token, T_LITERAL);
    if (!check_func_or_var_name(token->text))
    {
        error(E_FATAL, "function name only can use [a-zA-Z0-9_], but got " + token->text, token->file, token->start, token->line);
    }

    auto func_name = token->text;
    auto func = new Func(func_name);
    require_find(token, T_LPARENTHESIS);

    //TODO:arg name can't be same
    while (find(token, T_LITERAL))
    {
        auto name = token->text;
        Val val;

        if (find(token, T_ASSIGN))
        {

            if (find(token, T_QUOTE))
            {
                val.set_val(get_quote_string(token, false), TYPE_STRING);
            }
            else if (token->type == T_DQUOTE)
            {
                val.set_val(get_quote_string(token, true), TYPE_STRING);
            }
            else if (token->type == T_CONST_NUM)
            {
                val.set_val(token->text, TYPE_INT);
            }
            else if (token->type == T_BOOL)
            {
                val.set_val(token->text, TYPE_BOOL);
            }
            else
            {
                error(E_FATAL, "unexpect " + get_token_name(token->type), token->file, token->start, token->line);
            }

            find(token, T_COMMA);
        }

        func->args.emplace_back(name, val);

        if (token->type != T_COMMA)
        {
            break;
        }
    }

    if (token->type != T_RPARENTHESIS)
    {
        error(E_FATAL, "expect " + get_token_name(T_RPARENTHESIS) + " but got " + get_token_name(token->type)
                , token->file, token->start, token->line);
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
                , token->file, token->start, token->line);
    }

    if (func_begin != token)
    {
        parse(func_begin, token, func, true);
    }

    Lang233G::func.insert(func_name, func);
}

lang233_inline void Parser::var_declare_handler(t_iterator &token, Func *func)
{
    auto &op_array = func->op_array;

    require_find(token, T_LITERAL);
    const auto &name = token->text;
    if (unlikely(!check_func_or_var_name(token->text)))
    {
        error(E_FATAL, "variable name only can use [a-zA-Z0-9_], but got " + token->text, token->file, token->start, token->line);
    }

    for (const auto &arg : func->args)
    {
        if (unlikely(arg.name == name))
        {
            error(E_FATAL, "variable name can't be same as function arg name", token->file, token->start, token->line);
        }
    }

    Val val;
    auto var = new Variable(name, val);
    if (!func->vars.insert(var))
    {
        error(E_FATAL, "can\'t redeclare variable " + name, token->file, token->start, token->line);
    }

    var_assign_handler(token, func, false, var);
}

lang233_inline void Parser::var_assign_handler(t_iterator &token, Func *func, bool not_in_declare, Variable *var)
{
    const auto &name = token->text;

    if (unlikely(!func->vars.get(name) && !Lang233G::vars->get(name)))
    {
        error(E_FATAL, "use undeclared variable " + name, token->file, token->start, token->line);
    }

    if ((not_in_declare && require_find(token, T_ASSIGN)) || find(token, T_ASSIGN))
    {
        Val op_val(TYPE_STRING);
        op_val.set_val(name, TYPE_STRING);
        OPNode op1(OPNODE_VAR, op_val);

        find(token, T_LITERAL); // move to express
        auto op2 = parse_express(token, func, name);
        if (op2.type != OPNODE_NONE)
        {
            if (op2.type == OPNODE_IMM && !not_in_declare)
            {
                var->val.set_val(op2.val);
            }
            else
            {
                func->op_array.emplace_back(OP_ASSIGN_VAR, op1, op2, token);
            }
        }
    }

    if (token->type != T_CODELINEEND)
    {
        error(E_FATAL, "expect " + get_token_name(T_CODELINEEND) + " but got " + get_token_name(token->type)
                , token->file, token->start, token->line);
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
                val.val.set_val(token->text, TYPE_INT);
                break;

            case T_BOOL:
                val.type = OPNODE_IMM;
                val.val.set_val(token->text, TYPE_BOOL);
                break;

            case T_LITERAL:
                val = get_variable_or_func_return(token, func);
                break;

            case T_QUOTE:
                val.type = OPNODE_IMM;
                val.val.set_val(get_quote_string(token, false), TYPE_STRING);
                break;

            case T_DQUOTE:
                val.type = OPNODE_IMM;
                val.val.set_val(get_quote_string(token, true), TYPE_STRING);
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
            internal_error("priority = -1", __FILE__, __LINE__);
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
                        goto end_loop; // maybe in call func end
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
            enum opcode_type op_type = OP_NONE;

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
                    internal_error("unknown operator " + get_token_name(op->token->type), __FILE__, __LINE__);
                    break;
            }

            auto operand2 = op;
            --operand2;
            auto operand1 = op;
            ----operand1;

            if (unlikely(operand2 == end_iter || operand1 == end_iter || operand2->type == 1 || operand1->type == 1))
            {
                error(E_FATAL, "express parse error", token->file, token->start, token->line);
            }

            if (operand1->val.type == OPNODE_IMM && operand2->val.type == OPNODE_IMM)
            {
                VarTable _tmp_vars;
                VarScope _tmp_var_scope{&_tmp_vars};
                Variable _tmp_var("@tmp");
                _tmp_vars.insert(&_tmp_var);

                OPCode _tmp_opcode(op_type, operand1->val, operand2->val, _tmp_var.name, token);
                switch (op_type)
                {
                    case OP_ADD:
                        OPCodeHandler::add(_tmp_opcode, _tmp_var_scope);
                        break;

                    case OP_SUB:
                        //TODO
                        break;

                    case OP_MUL:
                        OPCodeHandler::mul(_tmp_opcode, _tmp_var_scope);
                        break;

                    case OP_DIV:
                        break;

                    case OP_MOD:
                        break;

                    default:
                        internal_error("unknown opcode", __FILE__, __LINE__);
                        break;
                }

                rpn.erase(operand2);
                rpn.erase(operand1);
                op->val.type = OPNODE_IMM;
                op->val.val.set_val(_tmp_var.val);
                op->type = 0;

                continue;
            }

            if (op == --rpn.end())
            {
                if (rpn.size() != 3)
                {
                    error(E_FATAL, "express parse error", token->file, token->start, token->line);
                }

                if (assign_var_name.empty())
                {
                    auto tmp_var = func->get_tmp_var_name();
                    func->op_array.emplace_back(op_type, operand1->val, operand2->val, tmp_var, token);
                    operand2->val.type = OPNODE_VAR;
                    operand2->val.val.set_val(tmp_var, TYPE_STRING);
                    return operand2->val;
                }
                else
                {
                    func->op_array.emplace_back(op_type, operand1->val, operand2->val, move(assign_var_name), token);
                    OPNode r_op;
                    return r_op;
                }
            }
            else
            {
                auto tmp_var = func->get_tmp_var_name();
                func->op_array.emplace_back(op_type, operand1->val, operand2->val, tmp_var, token);
                rpn.erase(operand2);
                rpn.erase(operand1);
                op->val.type = OPNODE_VAR;
                op->val.val.set_val(tmp_var, TYPE_STRING);
                op->type = 0;
            }
        }
    }

    auto op = rpn.begin();
    if (unlikely(rpn.size() != 1 || op->type != 0))
    {
        error(E_FATAL, "express parse error", token->file, token->start, token->line);
    }

    return op->val;
}

lang233_inline OPNode Parser::get_variable_or_func_return(t_iterator &token, Func *func)
{
    const auto &name = token->text;

    if (find(token, T_LPARENTHESIS))
    {
        // call_func
        return call_func_handler(token, name, func);
    }
    else
    {
        --token;
        Val val;
        val.set_val(name, TYPE_STRING);
        OPNode op(OPNODE_VAR, val);
        return op;
    }
}

lang233_inline OPNode Parser::call_func_handler(t_iterator &token, const string &name, Func *func, bool use_return)
{
    Val op1_val;
    op1_val.set_val(name, TYPE_STRING);
    OPNode op1(OPNODE_IMM, op1_val);

    if (find(token, T_RPARENTHESIS))
    {
        if (use_return)
        {
            auto tmp_var = func->get_tmp_var_name();
            Val val;
            val.set_val(tmp_var, TYPE_STRING);
            OPNode r_op(OPNODE_VAR, val);
            func->op_array.emplace_back(OP_CALL_FUNC, op1, move(tmp_var), token);
            return r_op;
        }
        else
        {
            func->op_array.emplace_back(OP_CALL_FUNC, op1, token);
            OPNode r_op;
            return r_op;
        }
    }

    Val op2_val(TYPE_FUNC_ARG);

    for (;;)
    {
        Val arg_val;

        auto a = parse_express(token, func);
        if (a.type == OPNODE_IMM)
        {
            DEBUG_OUTPUT("func arg imm: %s", a.val.to_string().c_str());
            arg_val.set_val(a.val);
            op2_val.val.func_args->emplace_back(arg_val);
        }
        else if (a.type == OPNODE_VAR)
        {
            DEBUG_OUTPUT("func arg var: %s", a.val.val.string->c_str());
            op2_val.val.func_args->emplace_back(*a.val.val.string, arg_val);
        }
        else
        {
            error(E_FATAL, "internal error", token->file, token->start, token->line);
        }

        if (token->type != T_COMMA)
        {
            goto end_loop;
        }

        if (find(token, T_RPARENTHESIS))
        {
            // ) after ,
            error(E_FATAL, "unexpect " + get_token_name(T_RPARENTHESIS), token->file, token->start, token->line);
        }
    }

    end_loop:
    if (token->type != T_RPARENTHESIS)
    {
        error(E_FATAL, "expect " + get_token_name(T_RPARENTHESIS) + " but got " + get_token_name(token->type)
                , token->file, token->start, token->line);
    }

    OPNode op2(OPNODE_ARG, op2_val);
    if (use_return)
    {
        auto tmp_var = func->get_tmp_var_name();
        Val val;
        val.set_val(tmp_var, TYPE_STRING);
        OPNode r_op(OPNODE_VAR, val);
        func->op_array.emplace_back(OP_CALL_FUNC, op1, op2, move(tmp_var), token);
        return r_op;
    }
    else
    {
        func->op_array.emplace_back(OP_CALL_FUNC, op1, op2, token);
        OPNode r_op;
        return r_op;
    }
}

bool Parser::parse(t_iterator t, const t_iterator &end, Func *func, bool in_function)
{
    while (t != end && t->type != TOKEN_END)
    {
        switch (t->type)
        {
            case T_FUNC:
            {
                // new function
                if (in_function)
                {
                    // not in main function
                    error(E_FATAL, "Can't declare function in other function", t->file, t->start, t->line);
                }

                func_declare_handler(t);
                break;
            }

            case T_VAR:
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
                    call_func_handler(t, name, func, false);
                    require_find(t, T_CODELINEEND);
                }
                else if (t->type == T_ASSIGN)
                {
                    for (; t->type != T_LITERAL; --t)
                    {

                    }

                    var_assign_handler(t, func);
                }
                else
                {

                }
                break;
            }

            case T_RETURN:
            {
                if (find(t, T_CODELINEEND))
                {
                    OPNode return_value;
                    func->op_array.emplace_back(OP_RETURN, return_value, t);
                }
                else
                {
                    auto return_value = parse_express(t, func);

                    if (t->type != T_CODELINEEND)
                    {
                        error(E_FATAL,
                              "expect " + get_token_name(T_CODELINEEND) + " but got " + get_token_name(t->type), t->file,
                              t->start, t->line);
                    }

                    func->op_array.emplace_back(OP_RETURN, return_value, t);
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
                error(E_FATAL, "unexpect " + get_token_name(t->type), t->file, t->start, t->line);
                break;
            }
        }

        ++t;
    }

    return true;
}

bool Parser::parse(const TokenArray &t_vector, Func *func, bool in_function)
{
    return parse(t_vector.cbegin(), t_vector.cend(), func, in_function);
}
