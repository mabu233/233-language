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
        OP_DECLARE_VAR = 0,
        OP_ASSIGN_VAR,
        OP_CALL_FUNC,
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
    };

    class OPCode
    {
    public:
        enum opcode_type type;
        OPNode op1;
        OPNode op2;

        OPCode(enum opcode_type _type, const OPNode &_op1, const OPNode &_op2) : type(_type), op1(_op1), op2(_op2)
        {

        }

        OPCode(enum opcode_type _type, const OPNode &_op1) : type(_type), op1(_op1)
        {

        }
    };

    typedef std::vector<OPCode> OPArray;
}

#endif //INC_233_LANGUAGE_233_OPCODE_H
