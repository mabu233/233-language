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
        bool parse(t_iterator t, const t_iterator &end, Func *func, bool in_function = false);
        bool parse(const TokenArray &t_vector, Func *func, bool in_function = false);

    private:
        lang233_inline bool check_func_or_var_name(const std::string &name);
        lang233_inline bool find(t_iterator &token, enum token_type t_type);
        lang233_inline bool require_find(t_iterator &token, enum token_type t_type);
        lang233_inline std::string get_quote_string(t_iterator &token, bool dquote = false);

        lang233_inline void func_declare_handler(t_iterator &token);
        lang233_inline void var_declare_handler(t_iterator &token, Func *func);
        lang233_inline void var_assign_handler(t_iterator &token, Func *func, bool not_in_declare = true, Variable *var = nullptr);
        lang233_inline OPNode call_func_handler(t_iterator &token, const std::string &name, Func *func, bool use_return = true);
        lang233_inline OPNode parse_express(t_iterator &token, Func *func, std::string assign_var_name = "");
        lang233_inline OPNode get_variable_or_func_return(t_iterator &token, Func *func);
    };
}

#endif //INC_233_LANGUAGE_233_PARSER_H
