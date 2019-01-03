//
// Created by lyx on 19-1-2.
//

#include "233_lang.h"

using namespace lang233;
using namespace std;

void Scanner::literal_handler()
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