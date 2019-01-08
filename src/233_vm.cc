//
// Created by lyx on 19-1-3.
//

#include <233_variable.h>
#include <cstring>
#include "233_lang.h"

using namespace lang233;
using namespace std;

Variable VM::run(const string &func_name, VarArray &args, const OPCode &call_func_op)
{
    auto func = Lang233G::func.get(func_name);
    if (!func)
    {
        error(E_FATAL, "call undeclared func " + func_name
                , call_func_op.in_file, call_func_op.file_offset, call_func_op.file_lineno);
        exit(1);
    }

    Variable return_value;
    auto arg_num = func->args.size();
    auto input_arg_num = args.size();

    if (input_arg_num > arg_num)
    {
        //TODO: check arg num (at least need don't have default  arg num)
        cout << "function " + func_name + " needs " << arg_num << " args, but got " << input_arg_num << " args" << endl;
        exit(1);
    }

    if (func->type == FUNC_INTERNAL)
    {
        VarArray &input_arg = args;

        for (auto i = 0; i < input_arg_num; ++i)
        {
            input_arg[i].name = func->args[i].name;
        }

        for (auto i = input_arg_num; i < arg_num; ++i)
        {
            input_arg.emplace_back(func->args[i].name, func->args[i].val);
        }

        (*func->pointer)(input_arg, &return_value);
        return return_value;
    }

    auto var_num = func_name == "@main" ? 0 : func->vars.get_num(); // main function's variables are in global VarTable

    auto stack_var_num = (1 + arg_num - input_arg_num + var_num + func->tmp_vars.get_num());
    auto stack_size = stack_var_num * sizeof(Variable);
    DEBUG_OUTPUT("stack_size: %ld", stack_size);
    auto stack_start = new Variable[stack_var_num];
    auto stack_curr = stack_start;
    DEBUG_OUTPUT("stack start: %p", stack_start);

    VarTable local_vars;
    VarScope vars{&local_vars, Lang233G::vars};

    for (auto i = 0; i < input_arg_num; ++i)
    {
        args[i].name = func->args[i].name;
        local_vars.insert(&args[i]);
    }

    for (auto i = input_arg_num; i < arg_num; ++i)
    {
        stack_curr->name = func->args[i].name;
        stack_curr->val.set_val(func->args[i].val);

        local_vars.insert(stack_curr);
        ++stack_curr;
    }

    for (const auto &_t_var : func->tmp_vars.v_vector)
    {
        *stack_curr = *_t_var->second;
        local_vars.insert(stack_curr);
        ++stack_curr;
    }

    for (int i = 0; i < var_num; ++i)
    {
        *stack_curr = *func->vars.v_vector[i]->second;
        local_vars.insert(stack_curr);
        ++stack_curr;
    }

    for (const auto &opcode : func->op_array)
    {
        switch (opcode.type)
        {
            case OP_ASSIGN_VAR:
                OPCodeHandler::assign_var(opcode, vars);
                break;

            case OP_CALL_FUNC:
                OPCodeHandler::call_func(opcode, vars);
                break;

            case OP_ADD:
                OPCodeHandler::add(opcode, vars);
                break;

            case OP_MUL:
                OPCodeHandler::mul(opcode, vars);
                break;

            case OP_MOD:
                break;

            case OP_RETURN:
                if (opcode.op1.type == OPNODE_VAR)
                {
                    return *get_variable(*opcode.op1.val.val.string, vars, opcode);
                }
                else if (opcode.op1.type == OPNODE_NONE)
                {
                    return return_value;
                }
                else
                {
                    return_value.val.set_val(opcode.op1.val);
                    return return_value;
                }
                break;

            default:
                //TODO:fatal error
                break;
        }
    }

    delete[] stack_start;
    return return_value;
}
