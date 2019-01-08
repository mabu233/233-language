//
// Created by lyx on 19-1-4.
//

#ifndef INC_233_LANGUAGE_233_OPCODE_HANDLER_H
#define INC_233_LANGUAGE_233_OPCODE_HANDLER_H

#include "233_opcode.h"
#include "233_variable.h"
#include "233_function.h"

namespace lang233
{
    class OPCodeHandler
    {
    public:
        static lang233_inline void assign_var(const OPCode &opcode, const VarScope &vars)
        {
            auto var = VM::get_variable(*opcode.op1.val.val.string, vars, opcode);
            if (opcode.op2.type == OPNODE_IMM)
            {
                var->val.set_val(opcode.op2.val);
            }
            else
            {
                auto var2 = VM::get_variable(*opcode.op2.val.val.string, vars, opcode);
                var->val.set_val(var2->val);
            }

            DEBUG_OUTPUT("assign var %s = %s", var->name.c_str(), var->val.to_string().c_str());
        }

        static lang233_inline void call_func(const OPCode &opcode, const VarScope &vars)
        {
            DEBUG_OUTPUT("run func %s", opcode.op1.val.val.string->c_str());
            VarArray call_args;

            if (opcode.op2.val.type == TYPE_FUNC_ARG)
            {
                for (auto arg = opcode.op2.val.val.func_args->begin(); arg != opcode.op2.val.val.func_args->end(); ++arg)
                {
                    if (arg->name.empty())
                    {
                        call_args.push_back(*arg);
                    }
                    else
                    {
                        DEBUG_OUTPUT("call func arg use variable %s", arg->name.c_str());
                        call_args.emplace_back(VM::get_variable(arg->name, vars, opcode)->val);
                    }
                }
            }

            if (!opcode.return_var.empty())
            {
                auto r_var = VM::get_variable(opcode.return_var, vars, opcode);
                auto return_value = VM::run(*opcode.op1.val.val.string, call_args, opcode);
                r_var->val.set_val(return_value.val);
            }
            else
            {
                VM::run(*opcode.op1.val.val.string, call_args, opcode);
            }
        }

        static lang233_inline void add(const OPCode &opcode, const VarScope &vars)
        {
            auto r_var = VM::get_variable(opcode.return_var, vars, opcode);

            auto op1_val = opcode.op1.val;
            auto op2_val = opcode.op2.val;

            if (opcode.op1.type == OPNODE_VAR)
            {
                op1_val = VM::get_variable(*op1_val.val.string, vars, opcode)->val;
            }

            if (opcode.op2.type == OPNODE_VAR)
            {
                op2_val = VM::get_variable(*op2_val.val.string, vars, opcode)->val;
            }

            Val add_val(op1_val);
            switch (add_val.type)
            {
                case TYPE_STRING:
                    add_val.val.string->append(op2_val.to_string());
                    break;

                case TYPE_INT:
                    add_val.val.int64 += op2_val.to_int();
                    break;

                case TYPE_BOOL:
                    add_val.val.boolean = ((add_val.to_int() + op2_val.to_int()) != 0);
                    break;

                case TYPE_NONE:
                    add_val.set_val(op2_val.to_int());
                    break;

                default:
                    break;
            }

            r_var->val.set_val(add_val);
            DEBUG_OUTPUT("OP_ADD: variable %s, value %s", r_var->name.c_str(), r_var->val.to_string().c_str());
        }

        static lang233_inline void mul(const OPCode &opcode, const VarScope &vars)
        {
            auto r_var = VM::get_variable(opcode.return_var, vars, opcode);

            auto op1_val = opcode.op1.val;
            auto op2_val = opcode.op2.val;

            if (opcode.op1.type == OPNODE_VAR)
            {
                op1_val = VM::get_variable(*op1_val.val.string, vars, opcode)->val;
            }

            if (opcode.op2.type == OPNODE_VAR)
            {
                op2_val = VM::get_variable(*op2_val.val.string, vars, opcode)->val;
            }

            r_var->val.set_val(op1_val.to_int() * op2_val.to_int());
            DEBUG_OUTPUT("OP_MUL: variable %s, value %s", r_var->name.c_str(), r_var->val.to_string().c_str());
        }
    };
}

#endif //INC_233_LANGUAGE_233_OPCODE_HANDLER_H
