/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Numeric Procedures : Arithmetic
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
#include "mls_icg_proc_stdnum.h"
#include "mls_icg_proc_stdnum_arith.h"

int mls_icodegen_node_proc_stdnum_arith(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    struct mls_ast_node *ast_node_args_sub = ast_node_args;
    int ast_node_args_count;
    const struct mls_icg_proc_lut_item *procedure_item;
    uint32_t id;

    (void)ast_node_proc;

    ast_node_args_count = mls_icg_count_list_node(ast_node_args);
    procedure_item = mls_icg_proc_lut_items_stdnum + procedure_idx;

    if ((procedure_item->value == MLS_ICG_ADD) && (ast_node_args_count == 0))
    {
        /* (+) */
        if ((ret = multiply_resource_get_int( \
                        err, \
                        context->icode, \
                        context->res_id, \
                        &id, \
                        0)) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
    }
    else if ((procedure_item->value == MLS_ICG_SUB) && (ast_node_args_count == 1))
    {
        /* (- x) */
        if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                        mls_icg_car(ast_node_args))) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NEG, 0)) != 0) { goto fail; }
    }
    else
    {
        switch (procedure_item->value)
        {
            case MLS_ICG_ADD: case MLS_ICG_SUB: case MLS_ICG_MUL:
            case MLS_ICG_DIV: case MLS_ICG_QUOTIENT:
            case MLS_ICG_REMAINDER: case MLS_ICG_MODULO:
                /* reduce style */
                if (ast_node_args_count > 0)
                {
                    /* Push the first argument */
                    if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                                    mls_icg_car(ast_node_args_sub))) != 0)
                    { goto fail; }
                    ast_node_args_sub = mls_icg_cdr(ast_node_args_sub);
                    ast_node_args_count -= 1;
                }
                while (ast_node_args_count != 0)
                {
                    /* Push the arguments */
                    if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                                    mls_icg_car(ast_node_args_sub))) != 0)
                    { goto fail; }
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEUP, 0)) != 0) { goto fail; }
                    switch (procedure_item->value)
                    {
                        case MLS_ICG_ADD:
                            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_ADD, 0)) != 0) { goto fail; }
                            break;
                        case MLS_ICG_SUB:
                            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SUB, 0)) != 0) { goto fail; }
                            break;
                        case MLS_ICG_MUL:
                            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_MUL, 0)) != 0) { goto fail; }
                            break;
                        case MLS_ICG_DIV:
                        case MLS_ICG_QUOTIENT:
                            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DIV, 0)) != 0) { goto fail; }
                            break;
                        case MLS_ICG_REMAINDER:
                        case MLS_ICG_MODULO:
                            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_MOD, 0)) != 0) { goto fail; }
                            break;
                    }
                    ast_node_args_sub = mls_icg_cdr(ast_node_args_sub);
                    ast_node_args_count -= 1;
                }
                break;

            default:
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: undefined operation on procedure \'%s\'", \
                        mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent), \
                        procedure_item->str);
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
                break;
        }
    }

    goto done;
fail:
done:
    return ret;
}

