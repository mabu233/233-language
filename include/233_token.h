//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_TOKEN_H
#define INC_233_LANGUAGE_233_TOKEN_H

#include "233_macro.h"
#include <vector>

namespace lang233
{
    enum token_type
    {
        TOKEN_END = 0,
        T_WHITESPACE,
        T_LITERAL,
        T_TYPENAME, // int, bool...
        T_ASSIGN, // =
        T_EQUAL, // ==
        T_FUNC, // func
        T_LPARENTHESIS, // (
        T_LBRACKET, // [
        T_LBRACE, // {
        T_RPARENTHESIS, // )
        T_RBRACKET, // ]
        T_RBRACE, // }
        T_ADD, // +
        T_SUB, // -
        T_MUL, // *
        T_DIV, // /
        T_MOD, // %
        T_NEWLINE, // \n
        T_CODELINEEND, // ;
        T_QUOTE, // '
        T_DQUOTE, // "
        T_COMMA, // ,
        T_CONST_NUM, // 0-9
        T_BOOL, // true or false
        T_SLASH, // '\'
    };

    class Token
    {
    public:
        enum token_type type; // token type
        std::string text; // token text
        uint64_t start; // token start offset
        uint64_t length; // token text length
        uint64_t line; // file line number

        Token(enum token_type t_type, std::string t_text, uint64_t t_start, uint64_t t_length, uint64_t t_line)
                : type(t_type), text(std::move(t_text)), start(t_start), length(t_length), line(t_line)
        {

        }
    };

    static lang233_inline std::string get_token_name(enum token_type t_type)
    {
        ENUM_TO_STRING(t_type)
        {
            ENUM_TO_STRING_CASE(TOKEN_END);
            ENUM_TO_STRING_CASE(T_WHITESPACE);
            ENUM_TO_STRING_CASE(T_LITERAL);
            ENUM_TO_STRING_CASE(T_TYPENAME);
            ENUM_TO_STRING_CASE(T_ASSIGN);
            ENUM_TO_STRING_CASE(T_EQUAL);
            ENUM_TO_STRING_CASE(T_FUNC);
            ENUM_TO_STRING_CASE(T_LPARENTHESIS);
            ENUM_TO_STRING_CASE(T_LBRACKET);
            ENUM_TO_STRING_CASE(T_LBRACE);
            ENUM_TO_STRING_CASE(T_RPARENTHESIS);
            ENUM_TO_STRING_CASE(T_RBRACKET);
            ENUM_TO_STRING_CASE(T_RBRACE);
            ENUM_TO_STRING_CASE(T_ADD);
            ENUM_TO_STRING_CASE(T_SUB);
            ENUM_TO_STRING_CASE(T_MUL);
            ENUM_TO_STRING_CASE(T_DIV);
            ENUM_TO_STRING_CASE(T_MOD);
            ENUM_TO_STRING_CASE(T_NEWLINE);
            ENUM_TO_STRING_CASE(T_CODELINEEND);
            ENUM_TO_STRING_CASE(T_QUOTE);
            ENUM_TO_STRING_CASE(T_DQUOTE);
            ENUM_TO_STRING_CASE(T_COMMA);
            ENUM_TO_STRING_CASE(T_CONST_NUM);
            ENUM_TO_STRING_CASE(T_BOOL);
            ENUM_TO_STRING_CASE(T_SLASH);
            default: return "T_UNKNOWN";
        }
    }

    typedef std::vector<Token> TokenArray;
}

#endif //INC_233_LANGUAGE_233_TOKEN_H
