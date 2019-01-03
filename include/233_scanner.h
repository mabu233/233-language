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

        inline void emit()
        {
            uint64_t t_length = 1 + offset - t_start;
            t_vector.emplace_back(t_type, code.substr(t_start, t_length), t_start, t_length, t_line);
            t_start = 1 + offset;
        }

        inline void check_literal()
        {
            if (t_type == T_LITERAL)
            {
                literal_handler();
                --offset;
                emit();
                ++offset;
            }
        }

        void literal_handler();
    };
}

#endif //INC_233_LANGUAGE_233_SCANNER_H
