//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_TYPE_H
#define INC_233_LANGUAGE_233_TYPE_H

#include <string>

#include "233_macro.h"

namespace lang233
{
    enum type
    {
        TYPE_NONE = 0,
        TYPE_INT,
        TYPE_BOOL,
        TYPE_STRING,

        TYPE_FUNC_ARG,
    };
}

#endif //INC_233_LANGUAGE_233_TYPE_H
