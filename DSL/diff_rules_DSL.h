#include "diff_DSL.h"

// DEF_OPER(NAME, HOW TO EVAL, HOW TO DIFF)

DEF_OPER(ADD,
    LEFT + RIGHT,
    _ADD (dL, dR))

DEF_OPER(SUB,
    LEFT - RIGHT,
    _SUB (dL, dR))

DEF_OPER(MUL,
    LEFT * RIGHT,
    _ADD (_MUL (dL, cR), _MUL (cL, dR)))

DEF_OPER(DIV,
    LEFT / RIGHT,
    _DIV (_SUB (_MUL (dL, cR), _MUL(cL, dR)), _MUL(cR, cR)))

DEF_OPER(SIN,
    sin(LEFT),
    _MUL (_COS(cL), dL))

DEF_OPER(COS,
    cos(LEFT),
    _MUL (_SIN (_MUL ( _NUM(-1), cL)), dL))

DEF_OPER(LOG,
    log(LEFT),
    _DIV (dL, cL))

/*
DEF_OPER(POW,
    pow(LEFT, RIGHT),
    )*/

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
