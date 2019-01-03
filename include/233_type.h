//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_TYPE_H
#define INC_233_LANGUAGE_233_TYPE_H

#include <string>

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

    inline enum type get_type(const std::string &type)
    {
        if (type == "int")
        {
            return TYPE_INT;
        }
        else if (type == "bool")
        {
            return TYPE_BOOL;
        }
        else if (type == "string")
        {
            return TYPE_STRING;
        }
        else
        {
            return TYPE_INT;
        }
    }
}

#endif //INC_233_LANGUAGE_233_TYPE_H
