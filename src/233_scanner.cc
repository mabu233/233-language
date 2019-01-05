//
// Created by lyx on 19-1-2.
//

#include "233_lang.h"

using namespace lang233;
using namespace std;

bool Scanner::scan(const string &scan_code, const string &file)
{
    if (unlikely(!code.empty()))
    {
        error(E_WARNING, "A scanner can\'t scan two file at the same time.", file, 0, 0);
        return false;
    }

    offset = 0;
    t_start = 0;
    t_line = 0;
    t_type = T_WHITESPACE;
    t_vector.clear();
    code = scan_code;
    auto max_offset = code.length() - 1;

    for (const auto &c : code)
    {
        switch (c)
        {
            case ' ':
            case '\t':
                check_literal();
                t_type = T_WHITESPACE;
                emit();
                break;
            case '+':
                check_literal();
                t_type = T_ADD;
                emit();
                break;
            case '-':
                check_literal();
                t_type = T_SUB;
                emit();
                break;
            case '*':
                check_literal();
                t_type = T_MUL;
                emit();
                break;
            case '/':
                check_literal();
                t_type = T_DIV;
                emit();
                break;
            case '%':
                check_literal();
                t_type = T_MOD;
                emit();
                break;
            case '=':
                check_literal();
                if (likely(offset < max_offset) && code[offset + 1] == '=')
                {
                    t_type = T_EQUAL;
                    ++offset;
                    emit();
                    break;
                }

                t_type = T_ASSIGN;
                emit();
                break;
            case '\n':
                check_literal();
                t_type = T_NEWLINE;
                emit();
                ++t_line;
                break;
            case '(':
                check_literal();
                t_type = T_LPARENTHESIS;
                emit();
                break;
            case '[':
                check_literal();
                t_type = T_LBRACKET;
                emit();
                break;
            case '{':
                check_literal();
                t_type = T_LBRACE;
                emit();
                break;
            case ')':
                check_literal();
                t_type = T_RPARENTHESIS;
                emit();
                break;
            case ']':
                check_literal();
                t_type = T_RBRACKET;
                emit();
                break;
            case '}':
                check_literal();
                t_type = T_RBRACE;
                emit();
                break;
            case '\'':
                check_literal();
                t_type = T_QUOTE;
                emit();
                break;
            case '"':
                check_literal();
                t_type = T_DQUOTE;
                emit();
                break;
            case ';':
                check_literal();
                t_type = T_CODELINEEND;
                emit();
                break;
            case ',':
                check_literal();
                t_type = T_COMMA;
                emit();
                break;
            case '\\':
                check_literal();
                t_type = T_SLASH;
                emit();
                break;
            default:
                t_type = T_LITERAL;
                break;
        }

        ++offset;
    }

    if (t_type == T_LITERAL)
    {
        --offset;
        emit();
        ++offset;
    }

    code = "";
    t_vector.emplace_back(TOKEN_END, " ", offset, 1, t_line);
    return true;
}