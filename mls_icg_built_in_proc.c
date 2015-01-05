/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Built-in Procedures
   Copyright(C) 2013-2014 Cheryl Natsu

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "multiple_ir.h"

#include "multiply.h"
#include "multiply_assembler.h"

#include "vm_opcode.h"
#include "vm_types.h"
#include "vm_predef.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_aux.h"

#include "mls_icg_built_in_proc.h"

/* Y-Combinator
 * (define y-combinator
 * (lambda (f)
 *   ((lambda (x) (f (lambda (y) ((x x) y))))
 *    (lambda (x) (f (lambda (y) ((x x) y))))))) */

static int mls_icg_add_built_in_procs_y_combinator( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_1 = 0, LBL_2 = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "f"        ,
                    MULTIPLY_ASM_OP_LBL , OP_LAMBDAMK, LBL_2, 
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_LBL , OP_LAMBDAMK, LBL_2, 
                    MULTIPLY_ASM_OP     , OP_SLV     ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_LABEL  , LBL_1 ,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "y"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "y"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "x"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "x"        ,
                    MULTIPLY_ASM_OP     , OP_SLV     ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,
                    MULTIPLY_ASM_OP     , OP_SLV     ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_LABEL  , LBL_2 ,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "x"        ,
                    MULTIPLY_ASM_OP_LBL , OP_LAMBDAMK, LBL_1, 
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "f"        ,
                    MULTIPLY_ASM_OP     , OP_SLV     ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,
                    MULTIPLY_ASM_FINISH
                    )) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* abs (Absolute Value)
 * (define (abs x)
 *  (if (> x 0)
 *    x
 *    (- x))) */
static int mls_icg_add_built_in_procs_abs( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_PUSH = 0, LBL_RETURN = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "x"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "x"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_L       ,
                    MULTIPLY_ASM_OP     , OP_NOTL    ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_PUSH   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "x"        ,
                    MULTIPLY_ASM_OP     , OP_NEG     ,
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_RETURN ,
                    MULTIPLY_ASM_LABEL  , LBL_PUSH ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "x"        , 
                    MULTIPLY_ASM_LABEL  , LBL_RETURN ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* gcd2 (Greatest Common Divisor)
 * (define (gcd2 a b) */
static int mls_icg_add_built_in_procs_gcd2( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "a"        ,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "b"        ,

                    /* c = 0 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* label head */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if (a == 0) goto tail; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_NE      ,
                    MULTIPLY_ASM_OP     , OP_NOTL    ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL   ,

                    /* c = a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* a = b % a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP     , OP_MOD     ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "a"        ,

                    /* b = c */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "c"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "b"        ,

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD   ,

                    /* label tail */
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* gcd (Greatest Common Divisor)
 * (define (gcd <arguments>) */
static int mls_icg_add_built_in_procs_gcd( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_37 = 0, LBL_47 = 1, LBL_53 = 2, LBL_72 = 3;
    const uint32_t LBL_HEAD = 5, LBL_TAIL = 6, LBL_GCD2 = 7, LBL_BACK = 8;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr"      ,

                    /* sz = size(arr) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz"       ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"       ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_NE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_37     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_LABEL  , LBL_37     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"       ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP     , OP_NE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_47     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_LABEL  , LBL_47     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"        ,

                    MULTIPLY_ASM_LABEL  , LBL_53     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"        ,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_72     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    /*
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 2          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "gcd2"     ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  , 
                    MULTIPLY_ASM_OP     , OP_CALL    , 
                    */

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_GCD2   ,
                    MULTIPLY_ASM_LABEL  , LBL_BACK     ,

                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP     , OP_ADD    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"        ,
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_53     ,

                    MULTIPLY_ASM_LABEL  , LBL_72     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"        ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 


                    MULTIPLY_ASM_LABEL  , LBL_GCD2   ,

                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "b"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "a"        ,

                    /* c = 0 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* label head */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if (a == 0) goto tail; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_NE      ,
                    MULTIPLY_ASM_OP     , OP_NOTL    ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL   ,

                    /* c = a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* a = b % a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP     , OP_MOD     ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "a"        ,

                    /* b = c */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "c"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "b"        ,

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD   ,

                    /* label tail */
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_BACK   ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* lcm (Least Common Multiple)
 * (define (lcm <arguments>) */
static int mls_icg_add_built_in_procs_lcm( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_37 = 0, LBL_47 = 1, LBL_53 = 2, LBL_72 = 3;
    const uint32_t LBL_HEAD = 5, LBL_TAIL = 6, LBL_GCD2 = 7, LBL_BACK = 8;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr"      ,

                    /* sz = size(arr) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz"       ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"       ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_NE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_37     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_LABEL  , LBL_37     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"       ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP     , OP_NE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_47     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    /* r = arr[0]; i = 1 */
                    MULTIPLY_ASM_LABEL  , LBL_47     ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"        ,

                    /* if i == sz goto LBL_72 */
                    MULTIPLY_ASM_LABEL  , LBL_53     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"        ,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_72     ,

                    /* arr[i] */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 

                    /* gcd2(r, arr[i]) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_GCD2   ,
                    MULTIPLY_ASM_LABEL  , LBL_BACK   ,

                    /* / */
                    MULTIPLY_ASM_OP     , OP_DIV     , 

                    /* r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"        ,

                    /* * */
                    MULTIPLY_ASM_OP     , OP_MUL     , 

                    /* r = arr[i] / gcd2(arr[i], r) * r */
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"        ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1          ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"        ,
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_53     ,

                    MULTIPLY_ASM_LABEL  , LBL_72     ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"        ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 


                    MULTIPLY_ASM_LABEL  , LBL_GCD2   ,

                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "b"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "a"        ,

                    /* c = 0 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* label head */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if (a == 0) goto tail; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP     , OP_NE      ,
                    MULTIPLY_ASM_OP     , OP_NOTL    ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL   ,

                    /* c = a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c"        ,

                    /* a = b % a */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "a"        ,
                    MULTIPLY_ASM_OP     , OP_MOD     ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "a"        ,

                    /* b = c */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "c"        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "b"        ,

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD   ,

                    /* label tail */
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"        ,
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_BACK   ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 

    goto done;
fail:
done:
    return ret;
}

/* expt (Exponent)
 * (expt base power) */
static int mls_icg_add_built_in_procs_expt( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_SKIP2 = 1, LBL_HEAD2 = 2;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "b"      ,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "p"      ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "p"      ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0        ,
                    MULTIPLY_ASM_OP     , OP_GE      , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "s"     ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "p"      ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "s"     ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP , 
                    MULTIPLY_ASM_OP     , OP_NEG     , 
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "p"      ,

                    /* r = 1 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1        ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"      ,
                    
                    MULTIPLY_ASM_LABEL  , LBL_HEAD2  , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "p"      ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0        ,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP2, 

                    /* p += 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "p"      ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1        ,
                    MULTIPLY_ASM_OP     , OP_SUB     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "p"      ,

                    /* r *= b */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"      ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "b"      ,
                    MULTIPLY_ASM_OP     , OP_MUL     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"      ,

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD2, 
                    MULTIPLY_ASM_LABEL  , LBL_SKIP2  , 

                    /* return r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"      ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 

    goto done;
fail:
done:
    return ret;
}

/* (downcase <num>) */
/* 
 * arg value
 * push value
 * fastlib downcase
 * return 
 */
static int mls_icg_add_built_in_procs_char_downcase( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
	int ret = 0;

	if ((ret = multiply_asm(err, icode,res_id,
		MULTIPLY_ASM_OP_ID, OP_ARGC, "obj",
		MULTIPLY_ASM_OP_ID, OP_PUSH, "obj",
		MULTIPLY_ASM_OP_RAW, OP_FASTLIB, OP_FASTLIB_DOWNCASE,
		MULTIPLY_ASM_OP, OP_RETURN,
		MULTIPLY_ASM_FINISH)) != 0)
	{
		goto fail;
	}
	goto done;
fail:
done :
	return ret;
}


/* (make-vector <number> [element]) */
/*
 * arg number
 * arg element
 * arrmk 0      ; Create a blank array
 * pop arr
 *
 * label_head:
 * push number
 * push 0
 * eq
 * jmpc label_tail
 *
 * push arr
 * push element ; Append element
 * arradd
 *
 * push number  ; number -= 1
 * push 1
 * sub
 * pop number
 *
 * jmp label_head
 *
 * label_tail:
 *
 * push arr
 * return
 */
static int mls_icg_add_built_in_procs_make_vector( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "number",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "element",
                    MULTIPLY_ASM_OP_RAW , OP_ARRMK   , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "arr",

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "number",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_EQ      ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_ARRADD  ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "number",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_SUB     ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "number",

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   , 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (vector-fill! <vector> <element>) */
/* 
 * arg v
 * arg element
 *
 * push v     ; l = length(v)
 * size
 * pop l
 *
 * lbl_head:
 * push l
 * push 1
 * sub
 * pop l
 *
 * push element
 * push l
 * push v
 * refset
 *
 * push l
 * push 0
 * ne
 * jmpc lbl_head
 * 
 * push vector
 * return
 */
static int mls_icg_add_built_in_procs_vector_fill( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "v",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "element",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_LE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_SUB     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_REFSET  , 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_NE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (vector->list <vector>) */
/*
 * arg v
 *
 * lstmk 0    ; make a blank list
 * pop lst
 *
 * push v     ; l = length(v)
 * size
 * pop l
 *
 * push 0     ; i = 0
 * pop i
 *
 * lbl_head:
 * push i
 * push 0 
 * eq
 * jmpc lbl_tail
 *
 * push i
 * push v
 * refget
 * push lst
 * lstadd
 *
 * push i     ; i += 1
 * push 1
 * add
 * pop i
 *
 * jmp lbl_head
 * lbl_tail:
 *
 * push v
 * return
 *
 */
static int mls_icg_add_built_in_procs_vector_list_contvert( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id, \
        uint32_t instrument_mk, uint32_t instrument_add)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "v",

                    MULTIPLY_ASM_OP_RAW , instrument_mk, 0, 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "lst", 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_REFGET  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , instrument_add,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

static int mls_icg_add_built_in_procs_vector_to_list( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    return mls_icg_add_built_in_procs_vector_list_contvert(err, icode, res_id, \
            OP_LSTMK, OP_LSTADD);
}

static int mls_icg_add_built_in_procs_list_to_vector( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    return mls_icg_add_built_in_procs_vector_list_contvert(err, icode, res_id, \
            OP_ARRMK, OP_ARRADD);
}

/* (map <procedure> list) */
/*
 * arg proc
 * arg lst
 *
 * lstmk 0    ; make a blank list
 * pop lst
 *
 * push v     ; l = length(v)
 * size
 * pop l
 *
 * push 0     ; i = 0
 * pop i
 *
 * lbl_head:
 * push i
 * push 0 
 * eq
 * jmpc lbl_tail
 *
 * push i     ; get member
 * push v
 * refget
 * 
 * push proc  ; push procedure
 * slv
 * funcmk
 * callc
 *
 * push lst
 * lstadd
 *
 * push i     ; i += 1
 * push 1
 * add
 * pop i
 *
 * jmp lbl_head
 * lbl_tail:
 *
 * push v
 * return
 * 
 */
static int mls_icg_add_built_in_procs_map( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "proc",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "v",

                    MULTIPLY_ASM_OP_RAW , OP_LSTMK   , 0, 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "lst", 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_REFGET  ,

                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "proc",
                    MULTIPLY_ASM_OP     , OP_SLV  ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_LSTADD  ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (apply <procedure> <list>) */
/*
 * arg proc
 * arg lst
 *
 * push lst
 * push 1
 * push proc
 * slv
 * funcmk
 * callc
 * 
 * return
 */
static int mls_icg_add_built_in_procs_apply( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "proc",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "lst",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "proc",
                    MULTIPLY_ASM_OP     , OP_SLV  ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,

                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (for-each <procedure> list) */
/*
 * arg proc
 * arg lst
 *
 * push lst     ; l = length(lst)
 * size
 * pop l
 *
 * push 0     ; i = 0
 * pop i
 *
 * lbl_head:
 * push i
 * push 0 
 * eq
 * jmpc lbl_tail
 *
 * push i     ; get member
 * push v
 * refget
 * 
 * push proc  ; push procedure
 * slv
 * funcmk
 * callc
 *
 * pop none
 *
 * push i     ; i += 1
 * push 1
 * add
 * pop i
 *
 * jmp lbl_head
 * lbl_tail:
 *
 * push v
 * return
 * 
 */
static int mls_icg_add_built_in_procs_for_each( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "proc",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "v",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_REFGET  ,

                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "proc",
                    MULTIPLY_ASM_OP     , OP_SLV  ,
                    MULTIPLY_ASM_OP     , OP_FUNCMK  ,
                    MULTIPLY_ASM_OP     , OP_CALLC   ,

                    MULTIPLY_ASM_OP_NONE, OP_POPC  ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "v",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (reverse <list>) */
/*
 * arg lst
 *
 * lstmk 0    ; make a blank list
 * pop new_lst
 *
 * push lst   ; l = length(lst)
 * size
 * pop i
 *
 * lbl_head:
 * push i
 * push 0
 * eq
 * jmpc lbl_tail
 *
 * push i
 * push v
 * refget
 * push new_lst
 * lstadd
 *
 * push i     ; i -= 1
 * push 1
 * sub
 * pop i
 *
 * push new_lst
 * return
 *
 */
static int mls_icg_add_built_in_procs_reverse( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "lst",

                    MULTIPLY_ASM_OP_RAW , OP_LSTMK   , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "new_lst",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_SIZE    ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_EQ      ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   , 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_SUB     ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_REFGET  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "new_lst",
                    MULTIPLY_ASM_OP     , OP_LSTADD  ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_NE      ,
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_HEAD,

                    MULTIPLY_ASM_LABEL  , LBL_TAIL   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "new_lst",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (null? list) */
/*
 * arg lst
 * push lst
 * size 
 * push 0
 * eq
 * return
 */
static int mls_icg_add_built_in_procs_nullp( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "lst",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_SIZE    ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0    ,
                    MULTIPLY_ASM_OP     , OP_EQ      ,
                    MULTIPLY_ASM_OP     , OP_RETURN  ,
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (list-tail lst pos) */
/*
 * arg lst
 * arg pos
 *
 * lstmk 0    ; make a blank list
 * pop new_lst
 *
 * push lst     ; l = length(v)
 * size
 * pop l
 *
 * lbl_head:
 * push pos
 * push l 
 * eq
 * jmpc lbl_tail
 *
 * push pos     ; get member
 * push v
 * refget
 * push new_lst
 * lstadd
 *
 * push pos     ; pos += 1
 * push 1
 * add
 * pop pos
 *
 * jmp lbl_head
 * lbl_tail:
 *
 * push new_lst
 * return
 */
static int mls_icg_add_built_in_procs_list_tail( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_HEAD = 0, LBL_TAIL = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "lst",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "pos",

                    MULTIPLY_ASM_OP_RAW , OP_LSTMK   , 0,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "new_lst",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "l",

                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "pos",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "l",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_TAIL,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "pos",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "lst",
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "new_lst",
                    MULTIPLY_ASM_OP     , OP_LSTADD  , 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "pos",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "pos",

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,
                    MULTIPLY_ASM_LABEL  , LBL_TAIL   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "new_lst",
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (car <list-or-pair>) */
/* 
 * arg obj
 *
 * push obj
 * typep <list>
 * jmpc l_list
 * push obj
 * paircar
 * return
 * l_list:
 * push obj
 * lstcar
 * return
 */
static int mls_icg_add_built_in_procs_car( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_LIST = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_TYPE, OP_TYPEP   , "list",
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_LIST,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_PAIRCAR , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_LABEL  , LBL_LIST   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_LSTCAR  , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (cdr <list-or-pair>) */
/* 
 * arg obj
 *
 * push obj
 * typep <list>
 * jmpc l_list
 * push obj
 * paircdr
 * return
 * l_list:
 * push obj
 * lstcdr
 * return
 */
static int mls_icg_add_built_in_procs_cdr( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_LIST = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_TYPE, OP_TYPEP   , "list",
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_LIST,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_PAIRCDR , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_LABEL  , LBL_LIST   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_LSTCDR  , 
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (set-car! <list-or-pair> element) */
/* 
 * arg obj
 * arg element
 *
 * push obj
 * typep <list>
 * jmpc l_list
 *
 * push element  ; pair
 * push obj      
 * paircarset
 *
 * push obj
 * return
 *
 * l_list:       ; list
 * push element
 * push 0
 * push obj
 * refset
 *
 * push obj
 * return
 */
static int mls_icg_add_built_in_procs_set_car( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_LIST = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "element",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_TYPE, OP_TYPEP   , "list",
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_LIST,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_PAIRCARSET,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_LABEL  , LBL_LIST   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_REFSET  , 

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (set-cdr! <list-or-pair> element) */
/* 
 * arg obj
 * arg element
 *
 * push obj
 * typep <list>
 * jmpc l_list
 *
 * push element  ; pair
 * push obj      
 * paircarset
 *
 * push obj
 * return
 *
 * l_list:       ; list
 * push element
 * push 0
 * push obj
 * refset
 *
 * push obj
 * return
 */
static int mls_icg_add_built_in_procs_set_cdr( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_LIST = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "element",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_TYPE, OP_TYPEP   , "list",
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_LIST,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_PAIRCDRSET,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_RETURN  ,

                    MULTIPLY_ASM_LABEL  , LBL_LIST   , 
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "element",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_LSTCDRSET, 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "obj",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
                    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (char-upcase <char>) */
/* 
 * arg value
 * push value
 * fastlib upcase
 * return 
 */
static int mls_icg_add_built_in_procs_char_upcase( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_UPCASE,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (char-downcase <char>) */
/* 
 * arg value
 * push value
 * fastlib downcase
 * return 
 */
static int mls_icg_add_built_in_procs_sqrt( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_SQRT,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (string <char-list>) */
static int mls_icg_add_built_in_procs_string( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_HEAD = 1;
    uint32_t type_id;

    ret = virtual_machine_object_type_name_to_id(&type_id, "str", 3);
    if (ret != 0) 
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'str\' isn't a valid type name");
        ret = -MULTIPLE_ERR_ICODEGEN; 
        goto fail; 
    }

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr",

                    /* r = "" */
                    MULTIPLY_ASM_OP_STR , OP_PUSH    , "",
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r",

                    /* sz = size(arr) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr"      ,
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz"       ,

                    /* i = 0 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0          ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"        ,

                    /* head: */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   , 

                    /* if (i == sz) goto lbl_skip */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"        ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz"       ,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP   ,  

                    /* r = r + arr[i] */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_RAW , OP_CONVERT , type_id    ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r",

                    /* i += 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"  ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1    ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"  ,

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD   ,  

                    /* skip: */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   , 

                    /* return r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"  ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}


/* (make-string <number> <char>) */
static int mls_icg_add_built_in_procs_make_string( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_HEAD = 1;
    const uint32_t LBL_1 = 2, LBL_2 = 3;
    uint32_t type_id;

    ret = virtual_machine_object_type_name_to_id(&type_id, "str", 3);
    if (ret != 0) 
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'str\' isn't a valid type name");
        ret = -MULTIPLE_ERR_ICODEGEN; 
        goto fail; 
    }

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr",

                    /* r = "" */
                    MULTIPLY_ASM_OP_STR , OP_PUSH    , "",
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r",

                    /* n = arr[0] */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "n",

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_SIZE    ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_EQ      , 

                    MULTIPLY_ASM_LABEL  , LBL_1      ,
                    /* c = ' ' */
                    MULTIPLY_ASM_OP_STR , OP_PUSH    , " ",
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c",

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_HEAD,

                    MULTIPLY_ASM_LABEL  , LBL_2      ,
                    /* c = arr[1] */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_RAW , OP_CONVERT , type_id, 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "c",

                    /* head */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if n == 0 goto skip */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "n",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP,

                    /* n -= 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "n",
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1,
                    MULTIPLY_ASM_OP     , OP_SUB     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "n",

                    /* r += ch */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "c"  ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"  ,

                    /* goto head */
                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,

                    /* skip: */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   ,

                    /* return r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"  ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}


/* (string-length <string>) */
static int mls_icg_add_built_in_procs_string_length( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "s",

                    /* size(s) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "s",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 

                    /* return */
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (string-append <string list>) */
static int mls_icg_add_built_in_procs_string_append( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_HEAD = 1;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr",

                    /* sz = size(arr) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz" ,

                    /* r = "" */
                    MULTIPLY_ASM_OP_STR , OP_PUSH    , ""   ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"  ,

                    /* i = 0 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0    ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"  ,

                    /* head: */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   , 

                    /* if (i == sz) goto skip; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz" ,
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP,

                    /* r = arr[i] + r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"  ,

                    /* i += 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i"  ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 1    ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i"  ,

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD,

                    /* skip: */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   , 

                    /* return r */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r"  ,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 

                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}


/* (max <num>) */
static int mls_icg_add_built_in_procs_max( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_SKIP2 = 1, LBL_HEAD = 2;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr",

                    /* sz = size(obj) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz" ,

                    /* r = none */
                    MULTIPLY_ASM_OP_NONE, OP_PUSH    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"  ,

                    /* if sz == 0 goto tail */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP ,


                    /* r = arr[0] */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  0    ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"   ,

                    /* i = 1 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  1    ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i" ,

                    /* head: */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if (i == sz) goto skip; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP ,

                    /* if r < arr[i] r = arr[i] */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "r"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP     , OP_GE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP2 ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    ,  "r"  ,
                    /* skip2 */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP2  ,

                    /* i += 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  1  ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    ,  "i"  ,

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD, 

                    /* skip: */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r",
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (min <num>) */
static int mls_icg_add_built_in_procs_min( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;
    const uint32_t LBL_SKIP = 0, LBL_SKIP2 = 1, LBL_HEAD = 2;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_LSTARGC , "arr",

                    /* sz = size(obj) */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr",
                    MULTIPLY_ASM_OP     , OP_SIZE    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "sz" ,

                    /* r = none */
                    MULTIPLY_ASM_OP_NONE, OP_PUSH    , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"  ,

                    /* if sz == 0 goto tail */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    , 0,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP ,


                    /* r = arr[0] */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  0    ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "r"   ,

                    /* i = 1 */
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  1    ,
                    MULTIPLY_ASM_OP_ID  , OP_POPC    , "i" ,

                    /* head: */
                    MULTIPLY_ASM_LABEL  , LBL_HEAD   ,

                    /* if (i == sz) goto skip; */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "i",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "sz",
                    MULTIPLY_ASM_OP     , OP_EQ      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP ,

                    /* if r > arr[i] r = arr[i] */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "r"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP     , OP_LE      , 
                    MULTIPLY_ASM_OP_LBL , OP_JMPC    , LBL_SKIP2 ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "arr" ,
                    MULTIPLY_ASM_OP     , OP_REFGET  , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    ,  "r"  ,
                    /* skip2 */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP2  ,

                    /* i += 1 */
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    ,  "i"  ,
                    MULTIPLY_ASM_OP_INT , OP_PUSH    ,  1  ,
                    MULTIPLY_ASM_OP     , OP_ADD     , 
                    MULTIPLY_ASM_OP_ID  , OP_POPC    ,  "i"  ,

                    MULTIPLY_ASM_OP_LBL , OP_JMP     , LBL_HEAD, 

                    /* skip: */
                    MULTIPLY_ASM_LABEL  , LBL_SKIP   ,

                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "r",
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}


/* (sin <num>) */
static int mls_icg_add_built_in_procs_sin( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_SIN,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (cos <num>) */
static int mls_icg_add_built_in_procs_cos( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_COS,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (tan <num>) */
static int mls_icg_add_built_in_procs_tan( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_TAN,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (asin <num>) */
static int mls_icg_add_built_in_procs_asin( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_ASIN,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (acos <num>) */
static int mls_icg_add_built_in_procs_acos( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_ACOS,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (atan <num>) */
static int mls_icg_add_built_in_procs_atan( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_ATAN,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (exp <num>) */
static int mls_icg_add_built_in_procs_exp( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_EXP,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

/* (log <num>) */
static int mls_icg_add_built_in_procs_log( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id)
{
    int ret = 0;

    if ((ret = multiply_asm(err, icode, res_id,
                    MULTIPLY_ASM_OP_ID  , OP_ARGC    , "obj",
                    MULTIPLY_ASM_OP_ID  , OP_PUSH    , "obj",
                    MULTIPLY_ASM_OP_RAW , OP_FASTLIB , OP_FASTLIB_LOG,
                    MULTIPLY_ASM_OP     , OP_RETURN  , 
                    MULTIPLY_ASM_FINISH)) != 0)
    { goto fail; } 
    goto done;
fail:
done:
    return ret;
}

static int mls_icg_add_built_in_procs_dependence_lcm( \
        struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list)
{
    mls_icg_customizable_built_in_procedure_list_called(customizable_built_in_procedure_list, "gcd", 3);
    return 0;
}

struct mls_icg_add_built_in_handler
{
    const char *name;
    const size_t name_len;
    int (*func)(struct multiple_error *err, \
            struct multiple_ir *multiple_ir, \
            struct multiply_resource_id_pool *res_id);
    int (*dependence)(struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list);
};

static struct mls_icg_add_built_in_handler mls_icg_add_built_in_handlers[] = 
{
    {"abs", 3, mls_icg_add_built_in_procs_abs, NULL},
    {"gcd2", 4, mls_icg_add_built_in_procs_gcd2, NULL},
    {"gcd", 3, mls_icg_add_built_in_procs_gcd, NULL},
    {"lcm", 3, mls_icg_add_built_in_procs_lcm, mls_icg_add_built_in_procs_dependence_lcm},
    {"expt", 4, mls_icg_add_built_in_procs_expt, NULL},
    {"sqrt", 4, mls_icg_add_built_in_procs_sqrt, NULL},
    {"y-combinator", 12, mls_icg_add_built_in_procs_y_combinator, NULL},
    {"make-vector", 11, mls_icg_add_built_in_procs_make_vector, NULL},
    {"vector-fill!", 12, mls_icg_add_built_in_procs_vector_fill, NULL},
    {"vector->list", 12, mls_icg_add_built_in_procs_vector_to_list, NULL},
    {"list->vector", 12, mls_icg_add_built_in_procs_list_to_vector, NULL},
    {"map", 3, mls_icg_add_built_in_procs_map, NULL},
    {"apply", 5, mls_icg_add_built_in_procs_apply, NULL},
    {"for-each", 8, mls_icg_add_built_in_procs_for_each, NULL},
    {"reverse", 7, mls_icg_add_built_in_procs_reverse, NULL},
    {"null?", 5, mls_icg_add_built_in_procs_nullp, NULL},
    {"list-tail", 9, mls_icg_add_built_in_procs_list_tail, NULL},
    {"car", 7, mls_icg_add_built_in_procs_car, NULL},
    {"cdr", 8, mls_icg_add_built_in_procs_cdr, NULL},
    {"set-car!", 8, mls_icg_add_built_in_procs_set_car, NULL},
    {"set-cdr!", 8, mls_icg_add_built_in_procs_set_cdr, NULL},
    {"char-upcase", 11, mls_icg_add_built_in_procs_char_upcase, NULL},
    {"char-downcase", 13, mls_icg_add_built_in_procs_char_downcase, NULL},
    {"string", 6, mls_icg_add_built_in_procs_string, NULL},
    {"make-string", 11, mls_icg_add_built_in_procs_make_string, NULL},
    {"string-length", 13, mls_icg_add_built_in_procs_string_length, NULL},
    {"string-append", 13, mls_icg_add_built_in_procs_string_append, NULL},
    {"max", 3, mls_icg_add_built_in_procs_max, NULL},
    {"min", 3, mls_icg_add_built_in_procs_min, NULL},
    {"sin", 3, mls_icg_add_built_in_procs_sin, NULL},
    {"cos", 3, mls_icg_add_built_in_procs_cos, NULL},
    {"tan", 3, mls_icg_add_built_in_procs_tan, NULL},
    {"asin", 4, mls_icg_add_built_in_procs_asin, NULL},
    {"acos", 4, mls_icg_add_built_in_procs_acos, NULL},
    {"atan", 4, mls_icg_add_built_in_procs_atan, NULL},
    {"exp", 3, mls_icg_add_built_in_procs_exp, NULL},
    {"log", 3, mls_icg_add_built_in_procs_log, NULL},
};
#define MLS_ICG_ADD_BUILT_IN_HANDLERS_COUNT (sizeof(mls_icg_add_built_in_handlers)/sizeof(struct mls_icg_add_built_in_handler))

int mls_icg_add_built_in_procs(struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id, \
        struct mls_icg_fcb_block *icg_fcb_block_init, \
        struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list,
        uint32_t insert_point, uint32_t *instrument_count)
{
    int ret = 0;
    struct mls_icg_customizable_built_in_procedure *customizable_built_in_procedure_cur;
    struct mls_icg_customizable_built_in_procedure *customizable_built_in_procedure_target = NULL;
    int i;
    uint32_t id;
    uint32_t instrument_number;
    *instrument_count = 0;

    /* Dependence */
    customizable_built_in_procedure_cur = customizable_built_in_procedure_list->begin;
    while (customizable_built_in_procedure_cur != NULL)
    {
        if (customizable_built_in_procedure_cur->called != 0)
        {
            for (i = 0; i != MLS_ICG_ADD_BUILT_IN_HANDLERS_COUNT; i++)
            {
                if ((mls_icg_add_built_in_handlers[i].name_len == customizable_built_in_procedure_cur->name_len) && \
                        (strncmp(customizable_built_in_procedure_cur->name, \
                                 mls_icg_add_built_in_handlers[i].name, \
                                 customizable_built_in_procedure_cur->name_len) == 0))
                {
                    customizable_built_in_procedure_target = mls_icg_customizable_built_in_procedure_list_lookup( \
                            customizable_built_in_procedure_list, \
                            mls_icg_add_built_in_handlers[i].name, \
                            mls_icg_add_built_in_handlers[i].name_len);
                    if (customizable_built_in_procedure_target == NULL)
                    {
                        if ((ret = mls_icg_add_built_in_handlers[i].dependence(customizable_built_in_procedure_list)) != 0)
                        { goto fail; }
                    }
                }
            }
        }
        customizable_built_in_procedure_cur = customizable_built_in_procedure_cur->next; 
    }

    customizable_built_in_procedure_cur = customizable_built_in_procedure_list->begin;
    while (customizable_built_in_procedure_cur != NULL)
    {
        if (customizable_built_in_procedure_cur->called != 0)
        {
            for (i = 0; i != MLS_ICG_ADD_BUILT_IN_HANDLERS_COUNT; i++)
            {
                if ((mls_icg_add_built_in_handlers[i].name_len == customizable_built_in_procedure_cur->name_len) && \
                        (strncmp(customizable_built_in_procedure_cur->name, \
                                 mls_icg_add_built_in_handlers[i].name, \
                                 customizable_built_in_procedure_cur->name_len) == 0))
                {
                    instrument_number = (uint32_t)(icode->text_section->size);
                    customizable_built_in_procedure_cur->instrument_number_icode = instrument_number;

                    if ((ret = mls_icg_add_built_in_handlers[i].func( \
                                    err, \
                                    icode, \
                                    res_id)) != 0)
                    { goto fail; }

                    /* Make Lambda */
                    if ((ret = mls_icg_fcb_block_insert_with_configure_type(icg_fcb_block_init, \
                                    insert_point++, \
                                    OP_LAMBDAMK, instrument_number, MLS_ICG_FCB_LINE_TYPE_BLTIN_PROC_MK)) != 0) { goto fail; }
                    /* Save to variable */
                    if ((ret = multiply_resource_get_id( \
                                    err, \
                                    icode, \
                                    res_id, \
                                    &id,  \
                                    customizable_built_in_procedure_cur->name, \
                                    customizable_built_in_procedure_cur->name_len)) != 0)
                    { goto fail; }
                    if ((ret = mls_icg_fcb_block_insert_with_configure(icg_fcb_block_init, insert_point++, OP_POPC, id)) != 0) { goto fail; }

                    (*instrument_count) += 2;
                    break;
                }
            }
        }
        customizable_built_in_procedure_cur = customizable_built_in_procedure_cur->next; 
    }

    goto done;
fail:
done:
    return ret;
}

