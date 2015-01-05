/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Customize Procedures
   Copyright(C) 2013 Cheryl Natsu

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
#include "mls_icg_proc_invoke.h"

int mls_icodegen_node_proc_push_arg(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg, int remain)
{
    int ret = 0;
    while (remain-- != 0)
    {
        /* Push argument */
        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block, \
                        mls_icg_car(ast_node_arg))) != 0)
        { goto fail; }

        ast_node_arg = mls_icg_cdr(ast_node_arg);
    }

fail:
    return ret;
}

int mls_icodegen_node_proc_push_arg_reversely(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg, int remain)
{
    int ret = 0;

    if (remain == 0) return 0;

    /* Next Layer (Previous argument) */
    if ((ret = mls_icodegen_node_proc_push_arg_reversely(err, \
                    context, \
                    icg_fcb_block, \
                    mls_icg_cdr(ast_node_arg), remain - 1)) != 0)
    { goto fail; }

    /* Push argument */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    mls_icg_car(ast_node_arg))) != 0)
    { goto fail; }

    goto done;
fail:
done:
    return ret;
}

static int mls_icodegen_node_proc_invoke_normal(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;
    int ast_node_args_count;
    uint32_t id;

    /* 'promise' requires duplicate the function */
    if (mls_icg_context_test(context, MLS_ICG_CONTEXT_DUPFUNC))
    {
        /* Clear the duplicate attribute */
        mls_icg_context_clear(context, MLS_ICG_CONTEXT_DUPFUNC);

        /* Solve the proc */
        if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                        ast_node_proc)) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    }

    /* Push arguments */
    ast_node_args_count = mls_icg_count_list_node(ast_node_args);
    if (ast_node_args_count > 0)
    {
        if ((ret = mls_icodegen_node_proc_push_arg(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_args, ast_node_args_count)) != 0)
        { goto fail; }
    }
    /* Arguments count */
    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
    { goto fail; }
    /* Reverse pushed arguments */
    if (ast_node_args_count > 1)
    {
        /* Arguments count for reverse */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
        /* Reverse */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_REVERSE, 0)) != 0) { goto fail; }
    }

    /* Arguments count for invoking */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    /* Solve the proc */
    if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                    ast_node_proc)) != 0)
    { goto fail; }

    /* Call the lambda function */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

static int mls_icodegen_node_proc_invoke_tail(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;
    int ast_node_args_count;
    uint32_t id;
    struct mls_ast_node *ast_node_cur, *ast_node_cur_in;

    ast_node_args_count = mls_icg_count_list_node(ast_node_args);

    /* Disable GC */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_IDGC, 0)) != 0) { goto fail; }

    /* Push arguments */
    ast_node_cur = ast_node_args;
    while (ast_node_cur != NULL)
    {
        ast_node_cur_in = mls_icg_car(ast_node_cur);

        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_cur_in)) != 0)
        { goto fail; }

        ast_node_cur = mls_icg_cdr(ast_node_cur);
    }

    /* Lift */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LIFT, (uint32_t)ast_node_args_count)) != 0) { goto fail; }

    /* Enable GC */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_IEGC, 0)) != 0) { goto fail; }

    /* Arguments Count */
    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    /* Solve the proc */
    if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                    ast_node_proc)) != 0)
    { goto fail; }

    /* Call the lambda function */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TAILCALLC, 0)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

int mls_icodegen_node_proc_invoke(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;

    if (mls_icg_context_test(context, MLS_ICG_CONTEXT_TAILCALL))
    {
        /* Clear Tail Attribute */
        mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAILCALL);

        /* Tail Call */
        if ((ret = mls_icodegen_node_proc_invoke_tail(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_proc, \
                        ast_node_args)) != 0)
        { goto fail; }
    }
    else
    {
        /* Normal Call */
        if ((ret = mls_icodegen_node_proc_invoke_normal(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_proc, \
                        ast_node_args)) != 0)
        { goto fail; }
    }

    goto done;
fail:
done:
    return ret;
}

