/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Numeric Procedures
   Copyright(C) 2014 Cheryl Natsu

   This file is part of multiple - Multiple Paradigm Language Interpreter

   multiple is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   multiple is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. 
   */

#ifndef _MLS_ICG_PROC_STDNUM_H_
#define _MLS_ICG_PROC_STDNUM_H_

#include "mls_icg_proc_lut.h"

enum 
{
    MLS_ICG_PROC_TYPE_ARITHMETIC = 1,
    MLS_ICG_PROC_TYPE_RATIONAL,
    MLS_ICG_PROC_TYPE_APPROXIMATION,
    MLS_ICG_PROC_TYPE_EXACTNESS,
    MLS_ICG_PROC_TYPE_INEQU,
    MLS_ICG_PROC_TYPE_MISC,
    MLS_ICG_PROC_TYPE_MAXMIN,
    MLS_ICG_PROC_TYPE_TRIGONOMETRY,
    MLS_ICG_PROC_TYPE_EXPONENTIALS,
    MLS_ICG_PROC_TYPE_COMPLEX,
    MLS_ICG_PROC_TYPE_STR_NUM,
};

enum 
{
    /* Basic arithmetic operators */
    MLS_ICG_ADD = 0, MLS_ICG_SUB, MLS_ICG_MUL, MLS_ICG_DIV,
    MLS_ICG_ABS, MLS_ICG_QUOTIENT, MLS_ICG_REMAINDER, MLS_ICG_MODULO, 
    MLS_ICG_GCD, MLS_ICG_LCM, MLS_ICG_EXPT, MLS_ICG_SQRT,
    /* Rational numbers */
    MLS_ICG_NUMERATOR, MLS_ICG_DENOMINATOR, MLS_ICG_RATIONALP, MLS_ICG_RATIONALIZE,
    /* Approximation */
    MLS_ICG_FLOOR, MLS_ICG_CEILING, MLS_ICG_TRUNCATE, MLS_ICG_ROUND,
    /* Exactness */
    MLS_ICG_INEXACT_TO_EXACT, MLS_ICG_EXACT_TO_INEXACT, EXACTP, INEXACTP,
    /* Inequalities */
    MLS_ICG_L, MLS_ICG_LE, MLS_ICG_G, MLS_ICG_GE, MLS_ICG_EQ, MLS_ICG_NE,
    /* Miscellaneous predicates */
    MLS_ICG_ZEROP, MLS_ICG_NEGATIVEP, MLS_ICG_POSITIVEP, MLS_ICG_ODDP, MLS_ICG_EVENP,
    /* Maximum and minimum */
    MLS_ICG_MAX, MLS_ICG_MIN,
    /* Trigonometry */
    MLS_ICG_SIN, MLS_ICG_COS, MLS_ICG_TAN, MLS_ICG_ASIN, MLS_ICG_ACOS, MLS_ICG_ATAN,
    /* Exponentials */
    MLS_ICG_EXP, MLS_ICG_LOG,
    /* Complex numbers */
    MLS_ICG_MAKE_RECTANGULAR, MLS_ICG_MAKE_POLAR, MLS_ICG_REAL_PART, MLS_ICG_IMAG_PART, MLS_ICG_MAGNITUDE, MLS_ICG_ANGLE, MLS_ICG_COMPLEXP,
    /* Input-output */
    MLS_ICG_NUMBER_TO_STRING, MLS_ICG_STRING_TO_NUMBER, 
};

static const struct mls_icg_proc_lut_item mls_icg_proc_lut_items_stdnum[] =
{
    /* Basic arithmetic operators */
    { MLS_ICG_ADD       , "+"         , 1 , 0 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_SUB       , "-"         , 1 , 1 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_MUL       , "*"         , 1 , 0 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_DIV       , "/"         , 1 , 1 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_ABS       , "abs"       , 3 , 1 , 1  , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_QUOTIENT  , "quotient"  , 8 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_REMAINDER , "remainder" , 9 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_MODULO    , "modulo"    , 6 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_GCD       , "gcd"       , 3 , 0 , -1 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_LCM       , "lcm"       , 3 , 0 , -1 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_ARITHMETIC } ,
    { MLS_ICG_EXPT      , "expt"      , 4 , 2 , 2  , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_ARITHMETIC}  ,
    { MLS_ICG_SQRT      , "sqrt"      , 4 , 1 , 1  , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_ARITHMETIC}  ,
    /* Rational numbers */
    { MLS_ICG_NUMERATOR   , "numerator"   , 9  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_RATIONAL } ,
    { MLS_ICG_DENOMINATOR , "denominator" , 11 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_RATIONAL } ,
    { MLS_ICG_RATIONALP   , "rational?"   , 9  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_RATIONAL } ,
    { MLS_ICG_RATIONALIZE , "rationalize" , 11 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_RATIONAL } ,
    /* Approximation */
    { MLS_ICG_FLOOR    , "floor"    , 5 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_APPROXIMATION } ,
    { MLS_ICG_CEILING  , "celing"   , 6 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_APPROXIMATION } ,
    { MLS_ICG_TRUNCATE , "truncate" , 8 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_APPROXIMATION } ,
    { MLS_ICG_ROUND    , "round"    , 5 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_APPROXIMATION } ,
    /* Exactness */
    { MLS_ICG_INEXACT_TO_EXACT , "inexact->exact" , 14 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_EXACTNESS } ,
    { MLS_ICG_EXACT_TO_INEXACT , "inexact->exact" , 14 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_EXACTNESS } ,
    { EXACTP                   , "exact?"         , 6  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_EXACTNESS } ,
    { INEXACTP                 , "inexact?"       , 8  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_EXACTNESS } ,
    /* Inequalities */
    { MLS_ICG_L  , "<"  , 1 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    { MLS_ICG_LE , "<=" , 2 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    { MLS_ICG_G  , ">"  , 1 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    { MLS_ICG_GE , ">=" , 2 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    { MLS_ICG_EQ , "="  , 1 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    { MLS_ICG_NE , "!=" , 2 , 2 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_INEQU } ,
    /* Miscellaneous predicates */
    { MLS_ICG_ZEROP     , "zero?"     , 5 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_MISC } ,
    { MLS_ICG_NEGATIVEP , "negative?" , 9 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_MISC } ,
    { MLS_ICG_POSITIVEP , "positive?" , 9 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_MISC } ,
    { MLS_ICG_ODDP      , "odd?"      , 4 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_MISC } ,
    { MLS_ICG_EVENP     , "even?"     , 5 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_MISC } ,
    /* Maximum and minimum */
    { MLS_ICG_MAX , "max" , 3 , 1 , -1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_MAXMIN } ,
    { MLS_ICG_MIN , "min" , 3 , 1 , -1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_MAXMIN } ,
    /* Trigonometry */
    { MLS_ICG_SIN  , "sin"  , 3 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    { MLS_ICG_COS  , "cos"  , 3 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    { MLS_ICG_TAN  , "tan"  , 3 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    { MLS_ICG_ASIN , "asin" , 4 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    { MLS_ICG_ACOS , "acos" , 4 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    { MLS_ICG_ATAN , "atan" , 4 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_TRIGONOMETRY } ,
    /* Exponentials */
    { MLS_ICG_EXP , "exp" , 3 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_EXPONENTIALS } ,
    { MLS_ICG_LOG , "log" , 3 , 1 , 1 , MLS_ICG_PROC_OP_CALL , MLS_ICG_PROC_TYPE_EXPONENTIALS } ,
    /* Complex numbers */
    { MLS_ICG_MAKE_RECTANGULAR , "make-rectangular" , 16 , 2 , 2 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_MAKE_POLAR       , "make-polar"       , 10 , 2 , 2 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_REAL_PART        , "real-part"        , 9  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_IMAG_PART        , "imag-part"        , 9  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_MAGNITUDE        , "magnitude"        , 9  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_ANGLE            , "angle"            , 5  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    { MLS_ICG_COMPLEXP         , "complex?"         , 8  , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_COMPLEX } ,
    /* Input-output */
    { MLS_ICG_NUMBER_TO_STRING , "number->string" , 14 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_STR_NUM } ,
    { MLS_ICG_STRING_TO_NUMBER , "string->number" , 14 , 1 , 1 , MLS_ICG_PROC_OP_UNHANDLED , MLS_ICG_PROC_TYPE_STR_NUM }  ,
};
#define MLS_PROC_LUT_ITEMS_STDNUM_COUNT ((sizeof(mls_icg_proc_lut_items_stdnum))/(sizeof(struct mls_icg_proc_lut_item)))

int mls_icodegen_node_proc_stdnum(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc, \
        struct mls_ast_node *ast_node_args, \
        const int procedure_idx);

#endif

