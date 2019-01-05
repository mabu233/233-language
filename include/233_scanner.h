//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_SCANNER_H
#define INC_233_LANGUAGE_233_SCANNER_H

#include <string>
#include <vector>
#include "233_token.h"

namespace lang233
{
    class Scanner
    {
    public:
        TokenArray t_vector;

        bool scan(const std::string &scan_code, const std::string &file);

    private:
        std::string code;
        uint64_t offset;
        uint64_t t_start;
        uint64_t t_line;
        enum token_type t_type;

        lang233_inline void emit()
        {
            uint64_t t_length = 1 + offset - t_start;
            t_vector.emplace_back(t_type, code.substr(t_start, t_length), t_start, t_length, t_line);
            t_start = 1 + offset;
        }

        lang233_inline void check_literal()
        {
            if (t_type == T_LITERAL)
            {
                literal_handler();
                --offset;
                emit();
                ++offset;
            }
        }

        lang233_inline void literal_handler()
        {
            if (code[t_start] >= '0' && code[t_start] <= '9')
            {
                t_type = T_CONST_NUM;
                return;
            }

            uint64_t len = offset - t_start;
            switch (len)
            {
                case 6:
                    if (code.compare(t_start, len, "string") == 0)
                    {
                        t_type = T_TYPENAME;
                    }

                    break;
                case 5:
                    if (code.compare(t_start, len, "false") == 0)
                    {
                        t_type = T_BOOL;
                    }

                    break;
                case 4:
                    if (code.compare(t_start, len, "func") == 0)
                    {
                        t_type = T_FUNC;
                    }
                    else if (code.compare(t_start, len, "bool") == 0)
                    {
                        t_type = T_TYPENAME;
                    }
                    else if (code.compare(t_start, len, "true") == 0)
                    {
                        t_type = T_BOOL;
                    }

                    break;
                case 3:
                    if (code.compare(t_start, len, "int") == 0)
                    {
                        t_type = T_TYPENAME;
                    }

                    break;
                default:
                    break;
            }
        }
    };
}

#endif //INC_233_LANGUAGE_233_SCANNER_H
