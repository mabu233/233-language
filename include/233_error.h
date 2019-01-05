//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_ERROR_H
#define INC_233_LANGUAGE_233_ERROR_H

#include <string>
#include <iostream>

namespace lang233
{
    enum error_type
    {
        E_WARNING = 0,
        E_FATAL,
    };

    static lang233_inline void error(enum error_type type, const std::string &msg,
                       const std::string &file, uint64_t offset, uint64_t line)
    {
        switch (type)
        {
            case E_WARNING:
                std::cout << "warning: ";
                break;
            case E_FATAL:
                std::cout << "fatal error: ";
                break;
        }

        std::cout << msg << " in file " << file << ", line " << line + 1 << ":" << offset + 1 << std::endl;
        if (type == E_FATAL)
        {
            exit(1);
        }
    }
}

#endif //INC_233_LANGUAGE_233_ERROR_H
