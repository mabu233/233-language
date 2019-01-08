//
// Created by lyx on 19-1-3.
//

#ifndef INC_233_LANGUAGE_233_VM_H
#define INC_233_LANGUAGE_233_VM_H

#include <string>

#include "233_function.h"
#include "233_error.h"

namespace lang233
{
    class VM
    {
    public:
        static Variable run(const std::string &func, VarArray &args, const OPCode &call_func_op);

        static lang233_inline Variable *get_variable(const std::string &var, const VarScope &vars, const OPCode &op)
        {
            if (unlikely(var.empty() || vars.empty()))
            {
                internal_error("get_variable error", __FILE__, __LINE__);
                exit(1);
            }

            for (const auto &v : vars)
            {
                auto r_var = v->get(var);
                if (r_var)
                {
                    return r_var;
                }
            }

            error(E_FATAL, "use undeclare variable " + var, op.in_file, op.file_offset, op.file_lineno);
            exit(1);
        }
    };
}

#endif //INC_233_LANGUAGE_233_VM_H
