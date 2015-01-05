/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures : Continuations
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "vm_opcode.h"
#include "vm_types.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_proc_lut.h"
#include "mls_icg_proc_std.h"
#include "mls_icg_proc_std_cont.h"

/* 
 * Continuation
 * http://en.wikipedia.org/wiki/Continuation
 *
 * In implementer's perspective, a continuation itself is 
 * just a normal procedure (or say function), but
 * with the following features (in call/cc case):
 * 1: Entrance Point is the next instrument after finish
 *    executing the call/cc procedure
 * 2: The entrance point has been decided before executing
 *    call/cc and been made into a procedure (or function)
 *    and passed in the call/cc part as an argument
 * 3: The function itself is a closure means it stores 
 *    the 'running environment' at the time it been produced,
 *    the consume of storing the 'running environment' isn't
 *    something the implementer should take care of.
 */

/* Example:
 *
 * (define call/cc call-with-current-continuation)
 * (define cont #f)
 * (+ 2 (call/cc 
 *   (lambda (c) 
 *     (set! cont c) 
 *     (* 3 4))))
 * (cont 2)
 *
 * 'c' has been decided in compile time with the entrance point 
 * at the end of (call/cc ...)
 *
 * While executing the line:
 * (set! cont c)
 * The variable cont has been assigned with the procedure 'c' 
 * (which entrance point is at the end of 'call/cc' block), AND with
 * The running environment at the position of (set! cont c)
 *
 * So, in the next procedure calling to cont, the number 2 has been
 * passed as an argument into a procedure which has the entrance point
 * at the end of 'call/cc' block as the following status:
 * (+ 2 2)
 * The result of the evaluation of '+' is obviously 4.
 */

/*
 * Process of compiling call/cc
 *
 * Take the previous program as an example for demonstrate the process
 * of compiling call/cc 
 *
 * 1: (define cont #f)
 *
 * push #f 
 * pop cont
 *
 *
 * 2: (lambda (c)
 *      (set! cont c) 
 *      (* 3 4))
 *
 * label_lambda:
 * arg c 
 *
 * push c   ; (set! cont c) 
 * pop cont
 *
 * push 3   ; (* 3 4)
 * push 4
 * mul
 * return
 *
 * 3: (+ 2 ...
 * 
 * push 2
 * 
 * lambdamk <current-continuation> 
 * funcmk
 *
 * push 1   ; Argument Count
 * lambdamk label_lambda
 * funcmk
 * callc
 *
 * label_current_continuation:
 *
 * add
 *
 *
 * 4: (cont 2)
 *
 * push 2
 * push 1   ; Argument count
 * push cont
 * slv
 * callc
 *
 *
 * 5: Continuation Transporter 
 * Pick the return value from the previous frame. 
 * We can't directly call the target (continuation) itself, 
 * because the target (continuation) has no 'arg' instrument 
 * that can't fetch the return value (argument of calling continuation) 
 * from the previous stack frame.
 *
 * arg v    ; return value 
 * push v   
 * return   
 *
 */

int mls_icodegen_node_proc_std_continuations(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    const struct mls_icg_proc_lut_item *procedure_item;
    uint32_t id;
    uint32_t instrument_number_push_cont = 0;
    uint32_t instrument_number_return_to = 0;
    uint32_t instrument_number_cont_transporter_skip = 0;
    uint32_t instrument_number_cont_transporter = 0;
    uint32_t instrument_number_cont_transporter_end = 0;
    int cont = 0;

    (void)ast_node_proc;

    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->value)
    {
        case MLS_ICG_CALL_WITH_CURRENT_CONTINUATION:
        case MLS_ICG_CALL_CC:

            /* Current Continuation */
            instrument_number_push_cont = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                            OP_CONTMK, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, \
                            OP_FUNCMK, 0)) != 0) { goto fail; }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 1)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, \
                            OP_PUSH, id)) != 0) { goto fail; }

            /* In Continuation? */
            if (mls_icg_context_test(context, MLS_ICG_CONTEXT_CONT)) { cont = 1; }

            /* Mark In Continuation */
            if (cont == 0) mls_icg_context_set(context, MLS_ICG_CONTEXT_CONT);

            /* Body of call/cc */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block, \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            /* Unmark In Continuation */
            if (cont == 0) mls_icg_context_clear(context, MLS_ICG_CONTEXT_CONT);

            /* Solve and Call */
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            /* Transporter */
            instrument_number_cont_transporter_skip = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                            OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0)
            { goto fail; }
            instrument_number_cont_transporter = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_ARGCS, id)) != 0) { goto fail; }
            instrument_number_return_to = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                            OP_RETURNTO, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) 
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
            instrument_number_cont_transporter_end = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

            mls_icg_fcb_block_link(icg_fcb_block, instrument_number_cont_transporter_skip, instrument_number_cont_transporter_end);
            mls_icg_fcb_block_link(icg_fcb_block, instrument_number_return_to, instrument_number_cont_transporter_end);
            mls_icg_fcb_block_link(icg_fcb_block, instrument_number_push_cont, instrument_number_cont_transporter);


            break;

        case MLS_ICG_VALUES:
        case MLS_ICG_CALL_WITH_VALUES:
        case MLS_ICG_DYNAMIC_WIND:
            MULTIPLE_ERROR_NOT_IMPLEMENTED();
            ret = -MULTIPLE_ERR_ICODEGEN; 
            break;

        default:
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: undefined operation on procedure \'%s\'", \
                    mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent), \
                    procedure_item->str);
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
            break;
    }
    goto done;
fail:
done:
    return ret;
}

