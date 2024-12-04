#include "diff_DSL.h"

// DEF_OPER(NAME, HOW TO EVAL, HOW TO DIFF)

DEF_OPER(ADD,
    LEFT + RIGHT,
    node_diff = _ADD (dL, dR);,
    "+")

DEF_OPER(SUB,
    LEFT - RIGHT,
    node_diff = _SUB (dL, dR);,
    "-")

DEF_OPER(MUL,
    LEFT * RIGHT,
    node_diff = _ADD (_MUL (dL, cR), _MUL (cL, dR));,
    "*")

DEF_OPER(DIV,
    LEFT / RIGHT,
    node_diff = _DIV (_SUB (_MUL (dL, cR), _MUL(cL, dR)), _MUL(cR, cR));,
    "/")

DEF_OPER(SIN,
    sin(RIGHT),
    node_diff = _MUL (_COS(cR), dR);,
    "\\sin")

DEF_OPER(COS,
    cos(RIGHT),
    node_diff = _MUL (_SIN (_MUL ( _NUM(-1), cR)), dR);,
    "\\cos")

DEF_OPER(LN,
    log(RIGHT),
    node_diff = _DIV (dR, cR);,
    "\\ln")


DEF_OPER(POW,
    pow(LEFT, RIGHT),
    bool var_in_base   = check_vars(node -> left);
    bool var_in_degree = check_vars(node -> right);

    if      (var_in_base && var_in_degree) { node_diff = _MUL(_ADD(_MUL(dR, _LOG(cL)), _DIV( _MUL(dL, cR), cL)), _POW(cL, cR)); }
    else if (var_in_base)                  { node_diff = _MUL(dL, _MUL(cR, _POW(cL, _SUB(cR, _NUM(1))))); }
    else if (var_in_degree)                { node_diff = _MUL(_POW(cL, cR), _MUL(dR, _LOG(cL))); }

    else {node_diff =  0;},
    "^")

#undef LEFT
#undef RIGHT

#undef _ADD
#undef _SUB
#undef _MUL
#undef _DIV
#undef _SIN
#undef _COS
#undef _LOG

#undef dL
#undef dR
#undef cL
#undef cR
