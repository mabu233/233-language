//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_LANG_H
#define INC_233_LANGUAGE_233_LANG_H

#include <unordered_map>
#include <string>
#include <vector>

#include "233_vm.h"
#include "233_scanner.h"
#include "233_parser.h"
#include "233_type.h"
#include "233_error.h"
#include "233_macro.h"

namespace lang233
{
    class Lang233G
    {
    public:
        static FuncTable func;
        static VarTable vars;
    };
}

#endif //INC_233_LANGUAGE_233_LANG_H
