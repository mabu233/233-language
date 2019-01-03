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
    };
}

#endif //INC_233_LANGUAGE_233_VM_H
