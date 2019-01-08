//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_OPCODE_H
#define INC_233_LANGUAGE_233_OPCODE_H

#include "233_variable.h"
#include "233_token.h"

namespace lang233
{
    enum opcode_type
    {
        OP_NONE = 0,
        OP_ASSIGN_VAR,
        OP_CALL_FUNC,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,
        OP_RETURN,
    };

    enum opnode_type
    {
        OPNODE_NONE = 0,
        OPNODE_IMM,
        OPNODE_VAR,
        OPNODE_ARG,
    };

    class OPNode
    {
    public:
        enum opnode_type type;
        Val val;

        OPNode() : type(OPNODE_NONE), val(TYPE_NONE)
        {

        }

        OPNode(enum opnode_type _type, const Val &_val) : type(_type), val(_val)
        {

        }

        OPNode(const OPNode &_op_node) = default;
    };

    class OPCode
    {
    public:
        enum opcode_type type;
        OPNode op1;
        OPNode op2;
        std::string return_var;
        std::string in_file;
        uint64_t file_offset;
        uint64_t file_lineno;

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2, const t_iterator &_token)
                : type(_type), op1(_op1), op2(_op2)
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2, std::string _return_var, const t_iterator &_token)
                : type(_type), op1(_op1), op2(_op2), return_var(std::move(_return_var))
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2, size_t _tmp_var_id, const t_iterator &_token)
                : type(_type), op1(_op1), op2(_op2), return_var(std::move("~" + std::to_string(_tmp_var_id)))
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }

        OPCode(enum opcode_type _type, const OPNode &_op1, const t_iterator &_token) : type(_type), op1(_op1)
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }

        OPCode(enum opcode_type _type, const OPNode &_op1, std::string _return_var, const t_iterator &_token)
                : type(_type), op1(_op1), return_var(std::move(_return_var))
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }

        OPCode(enum opcode_type _type, const OPNode &_op1, size_t _tmp_var_id, const t_iterator &_token)
                : type(_type), op1(_op1), return_var(std::move("~" + std::to_string(_tmp_var_id)))
        {
            in_file = _token->file;
            file_offset = _token->start;
            file_lineno = _token->line;
        }
    };
    
    typedef std::vector<OPCode> OPArray;
}

#endif //INC_233_LANGUAGE_233_OPCODE_H
