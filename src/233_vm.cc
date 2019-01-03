//
// Created by lyx on 19-1-3.
//

#include <233_variable.h>
#include <cstring>
#include "233_lang.h"

using namespace lang233;
using namespace std;

Variable VM::run(const string &func_name, VarArray &args)
{
    auto func = Lang233G::func.get(func_name);
    if (!func)
    {
        //TODO: file and lineno
        cout << "call undefined function " + func_name << endl;
        exit(1);
    }

    Variable return_value;
    auto arg_num = func->args.size();
    auto input_arg_num = args.size();

    if (input_arg_num > arg_num)
    {
        //TODO: file and lineno
        cout << "function " + func_name + " needs " << arg_num << " args, but got " << input_arg_num << " args" << endl;
        exit(1);
    }

    if (func->type == FUNC_INTERNAL)
    {
        VarArray input_arg;

        for (int i = 0; i < arg_num; ++i)
        {
            if (i < input_arg_num)
            {
                input_arg.emplace_back(func->args[i].name, args[i].val);
            }
            else
            {
                input_arg.emplace_back(func->args[i].name, func->args[i].val);
            }
        }

        (*func->pointer)(input_arg, &return_value);
        return return_value;
    }

    auto var_num = func->vars.get_num();

    auto stack_var_num = (1 + arg_num + var_num);
    auto stack_size = stack_var_num * sizeof(Variable);
    DEBUG_OUTPUT("stack_size: %ld", stack_size);
    auto stack_start = new Variable[stack_var_num];
    auto stack_end = stack_start + stack_size;
    auto stack_curr = stack_start;
    DEBUG_OUTPUT("stack start: %p", stack_start);

    VarTable vars;

    for (int i = 0; i < arg_num; ++i)
    {
        stack_curr->name = func->args[i].name;

        if (i < input_arg_num)
        {
            stack_curr->val.set_val(args[i].val);
        }
        else
        {
            stack_curr->val.set_val(func->args[i].val);
        }

        vars.insert(stack_curr->name, stack_curr);
        ++stack_curr;
    }

    for (const auto &opcode : func->op_array)
    {
        auto op1 = opcode.op1;
        auto op2 = opcode.op2;
        auto op1_val = op1.val;
        auto op2_val = op2.val;

        switch (opcode.type)
        {
            case OP_DECLARE_VAR:
            {
                auto name = *op1_val.val.string;
                *stack_curr = *func->vars.get(name);
                vars.insert(stack_curr->name, stack_curr);
                DEBUG_OUTPUT("%s", ("declare var " + stack_curr->name).c_str());
                ++stack_curr;
                break;
            }

            case OP_ASSIGN_VAR:
            {
                auto var = vars.get(*op1_val.val.string);
                var->val.set_val(op2_val);
                DEBUG_OUTPUT("%s", ("assign: var " + var->name + " = " + op2_val.to_string()).c_str());
                break;
            }

            case OP_CALL_FUNC:
            {
                DEBUG_OUTPUT("%s", ("run func " + *op1_val.val.string).c_str());
                VarArray call_args;

                if (op2_val.type == TYPE_FUNC_ARG)
                {
                    //TODO:support variable arg
                    call_args = *op2_val.val.func_args;
                }

                run(*op1_val.val.string, call_args);
                break;
            }

            default:
            {
                break;
            }
        }
    }

    delete[] stack_start;
    return return_value;
}
