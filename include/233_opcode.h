//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_OPCODE_H
#define INC_233_LANGUAGE_233_OPCODE_H

#include "233_variable.h"

namespace lang233
{
    enum opcode_type
    {
        OP_ASSIGN_VAR = 0,
        OP_CALL_FUNC,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_MOD,
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

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2) : type(_type), op1(_op1), op2(_op2)
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2, std::string _return_var)
                : type(_type), op1(_op1), op2(_op2), return_var(std::move(_return_var))
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2, size_t _tmp_var_id)
                : type(_type), op1(_op1), op2(_op2), return_var(std::move("~" + std::to_string(_tmp_var_id)))
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1) : type(_type), op1(_op1)
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1, std::string _return_var)
                : type(_type), op1(_op1), return_var(std::move(_return_var))
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1, size_t _tmp_var_id)
                : type(_type), op1(_op1), return_var(std::move("~" + std::to_string(_tmp_var_id)))
        {

        }
    };
    
    typedef std::vector<OPCode> OPArray;
}

#endif //INC_233_LANGUAGE_233_OPCODE_H
