//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_PARSER_H
#define INC_233_LANGUAGE_233_PARSER_H

#include <vector>
#include "233_opcode.h"
#include "233_function.h"
#include "233_token.h"

namespace lang233
{
    class Parser
    {
    public:
        typedef TokenArray::const_iterator t_iterator;

        bool parse(t_iterator t, const t_iterator &end, Func *func, const std::string &p_file);
        bool parse(const TokenArray &t_vector, Func *func, const std::string &p_file);

    private:
        std::string file;

        inline bool check_func_or_var_name(const std::string &name);
        inline bool find(t_iterator &token, enum token_type t_type);
        inline bool require_find(t_iterator &token, enum token_type t_type);
        inline std::string get_quote_string(t_iterator &token, bool dquote = false);

        inline void func_declare_handler(t_iterator &token);
        inline void var_declare_handler(t_iterator &token, Func *func);
        inline void call_func_handler(t_iterator &token, const std::string &name, Func *func);
    };
}

#endif //INC_233_LANGUAGE_233_PARSER_H
