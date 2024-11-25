#include "diff_DSL.h"

// DEF_OPER(NAME, HOW TO EVAL, HOW TO DIFF)

DEF_OPER(ADD,
    LEFT + RIGHT,
    return _ADD (dL, dR);,
    "+")

DEF_OPER(SUB,
    LEFT - RIGHT,
    return _SUB (dL, dR);,
    "-")

DEF_OPER(MUL,
    LEFT * RIGHT,
    return _ADD (_MUL (dL, cR), _MUL (cL, dR));,
    "*")

DEF_OPER(DIV,
    LEFT / RIGHT,
    return _DIV (_SUB (_MUL (dL, cR), _MUL(cL, dR)), _MUL(cR, cR));,
    "/")

DEF_OPER(SIN,
    sin(RIGHT),
    return _MUL (_COS(cR), dR);,
    "sin")

DEF_OPER(COS,
    cos(RIGHT),
    return _MUL (_SIN (_MUL ( _NUM(-1), cR)), dR);,
    "cos")

DEF_OPER(LOG,
    log(RIGHT),
    return _DIV (dR, cR);,
    "ln")


DEF_OPER(POW,
    pow(LEFT, RIGHT),
    bool var_in_base   = check_vars(node -> left);
    bool var_in_degree = check_vars(node -> right);

    if (var_in_base && var_in_degree) { return _MUL(_ADD(_MUL(dR, _LOG(cL)), _DIV( _MUL(dL, cR), cL)), _POW(cL, cR)); }
    if (var_in_base)                  { return _MUL(dL, _MUL(cR, _POW(cL, _SUB(cR, _NUM(1))))); }
    if (var_in_degree)                { return _MUL(_POW(cL, cR), _MUL(dR, _LOG(cL))); }

    return 0;,
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
