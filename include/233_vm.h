//
// Created by lyx on 19-1-3.
//

#ifndef INC_233_LANGUAGE_233_VM_H
#define INC_233_LANGUAGE_233_VM_H

#include <string>

#include "233_function.h"

namespace lang233
{
    class VM
    {
    public:
        static Variable run(const std::string &func, VarArray &args);

        static lang233_inline Variable *get_variable(const std::string &var, const VarScope &vars)
        {
            if (unlikely(var.empty() || vars.empty()))
            {
                printf("internal error in get_variable\n");
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

            printf("use undeclare variable %s\n", var.c_str());
            exit(1);
        }
    };
}

#endif //INC_233_LANGUAGE_233_VM_H
