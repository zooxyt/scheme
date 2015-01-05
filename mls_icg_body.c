/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Body
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

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"
#include "mls_icg_generic.h"
#include "mls_icg_body.h"

int mls_icodegen_node_body(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_ast_node *ast_node_cur;
    struct mls_ast_node *ast_node_first;
    uint32_t id;
    int no_produce_value_on_stack;
    int root = 0;
    int root_begin = 0;
    int cont = 0;
    uint32_t instrument_number_trap_set = 0;
    uint32_t instrument_number_trap;

    /* Is Root? */
    if (mls_icg_context_test(context, MLS_ICG_CONTEXT_ROOT)) 
    {
        root = 1;
        /*mls_icg_context_clear(context, MLS_ICG_CONTEXT_ROOT);*/
    }
    /*root = 1;*/
    /* In Continuation? */
    if (mls_icg_context_test(context, MLS_ICG_CONTEXT_CONT)) { cont = 1; }
    (void)cont;

    ast_node_cur = ast_node;
    while (ast_node_cur != NULL)
    {
        ast_node_first = mls_icg_car(ast_node_cur);

        if (root)
        {
            /* Clear Root (Depends on the situations) */
            mls_icg_context_clear(context, MLS_ICG_CONTEXT_ROOT);
            /* begin block doesn't counts in sub of root */
            if ((mls_icg_is_node_cons(ast_node_first) != 0) &&
                    (mls_icg_is_node_atom_id_with_name(mls_icg_car(ast_node_first), "begin") != 0))
            {
                root_begin = 1;
                mls_icg_context_set(context, MLS_ICG_CONTEXT_ROOT);
            }
            else
            {
                root_begin = 0;
            }
        }

        /* Set trap */
        if (root && (root_begin == 0))
        {
            instrument_number_trap_set = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_TRAPSET, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
        }

        /* Set Tail Attribute (Currently DISABLED) */
        if ((mls_icg_cdr(ast_node_cur) == NULL) &&
                (0 != 0))
        {
            mls_icg_context_set(context, MLS_ICG_CONTEXT_TAIL);
        }

        if (ast_node_first == NULL)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: bad syntax: expression expected", \
                    mls_ast_node_ln(ast_node_cur), mls_ast_node_col(ast_node_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }

        /* Dig into a line of begin block */
        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block,  \
                        ast_node_first)) != 0)
        { goto fail; }

        /* Clear Tail Attribute */
        mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);

        /* Trap */
        if (root && (root_begin == 0))
        {
            instrument_number_trap = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TRAP, 0)) != 0) { goto fail; }

            mls_icg_fcb_block_link(icg_fcb_block, instrument_number_trap_set, instrument_number_trap);
        }

        /* Some forms don't produce value */
        no_produce_value_on_stack = 0;

        /* Next node */
        ast_node_cur = mls_icg_cdr(ast_node_cur);

        if (ast_node_cur == NULL)
        {
            /* The final node */
            if (no_produce_value_on_stack != 0) 
            {
                /* Since no value produced on the stack, we give a none */
                if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id)) != 0) 
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            }
            else
            {
                /* Nothing needed to do */
            }
        }
        else
        {
            /* Not the final node */
            if (no_produce_value_on_stack != 0) 
            {
                /* Nothing needed to do */
            }
            else
            {
                if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id)) != 0) 
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id)) != 0) { goto fail; }
            }
        }
    }
    goto done;
fail:
done:
    return ret;
}

