/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : let* (Serial)
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

#include "vm_opcode.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_define.h"
#include "mls_icg_body.h"
#include "mls_icg_form_leta.h"


/*
 * (let* ((var1 exp1)
 *       (var2 exp2)
 *       ...
 *       (varn expn))
 *      <body>)
 *
 * =>
 *
 * (let ((var1 exp1))
 *   (let ((var2 exp2))
 *     ...
 *       (let ((varn expn))
 *         <body>)))
 *
 * =>
 *
 * ((lambda (var1)
 *   ((lambda (var2)
 *     body)
 *     exp2)
 *   exp1)
 */

static int mls_icodegen_node_form_leta_single(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_binds_rest, \
        struct mls_ast_node *ast_node_body, \
        int remain)
{
    int ret = 0;
    struct mls_ast_node *new_ast_node_binds_rest;
    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;
    struct mls_ast_node *ast_node_bind_branch_cur_in;
    struct mls_ast_node *ast_node_bind_var;
    struct mls_ast_node_atom *ast_node_var_atom;
    struct mls_ast_node *ast_node_bind_exp;
    uint32_t id;

    new_icg_fcb_block = mls_icg_fcb_block_new();
    if (new_icg_fcb_block == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    new_export_section_item = multiple_ir_export_section_item_new();
    if (new_export_section_item == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }
    new_export_section_item->blank = 1;

    /* Extracting neccessary data */
    new_ast_node_binds_rest = mls_icg_cdr(ast_node_binds_rest);
    ast_node_bind_branch_cur_in = mls_icg_car(ast_node_binds_rest);
    /* Neccessary checking */
    if (mls_icg_count_list_node(ast_node_bind_branch_cur_in) != 2)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: let: bad syntax, " \
                "not an identifier and expression for a binding", \
                mls_ast_node_ln(ast_node_bind_branch_cur_in), \
                mls_ast_node_col(ast_node_bind_branch_cur_in));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_bind_var = mls_icg_car(ast_node_bind_branch_cur_in);
    ast_node_var_atom = ast_node_bind_var->ptr;
    ast_node_bind_exp = mls_icg_car(mls_icg_cdr(ast_node_bind_branch_cur_in));

    /* Construct lambda inside */ 

    /* 1 Argument */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_var_atom->atom->str, \
                    ast_node_var_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_ARGC, id)) != 0)
    { goto fail; }
    if (remain == 1)
    {
        /* Body */
        if ((ret = mls_icodegen_node_body(err, \
                        context, \
                        new_icg_fcb_block, \
                        ast_node_body)) != 0)
        { goto fail; }
    }
    else
    {
        /* Deeper */
        if ((ret = mls_icodegen_node_form_leta_single(err, \
                        context, \
                        new_icg_fcb_block, \
                        new_ast_node_binds_rest, \
                        ast_node_body, \
                        remain - 1)) != 0)
        { goto fail; }
    }
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }


    /* Outside */

    /* Expressions */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_bind_exp)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) 
    { goto fail; }
    /* Arguments count */
    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 1)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0)
    { goto fail; }
    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_LAMBDAMK, \
                    (uint32_t)(context->icg_fcb_block_list->size), \
                    MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0) 
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }
    /* Append blank export section */
    if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }

    goto done;
fail:
    if (new_icg_fcb_block != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}

int mls_icodegen_node_form_leta(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int list_node_bind_count;
    struct mls_ast_node *ast_node_bind;
    struct mls_ast_node *ast_node_body;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: let: bad syntax, " \
                "must contains at least a binding and body", \
                mls_ast_node_ln(context->ast_node_parent), \
                mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count == 1)
    {
        if (mls_icg_is_node_cons(ast_node) != 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: let: bad syntax, " \
                    "no expression in body", \
                    mls_ast_node_ln(context->ast_node_parent), \
                    mls_ast_node_col(context->ast_node_parent));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        else
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: let: bad syntax, no binding", \
                    mls_ast_node_ln(context->ast_node_parent), \
                    mls_ast_node_col(context->ast_node_parent));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
    }
    else
    {
        if (mls_icg_is_node_cons(ast_node) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: let: bad syntax, no binding", \
                    mls_ast_node_ln(context->ast_node_parent), \
                    mls_ast_node_col(context->ast_node_parent));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
    }

    ast_node_bind = mls_icg_car(ast_node);
    ast_node_body = mls_icg_cdr(ast_node);
    list_node_bind_count = mls_icg_count_list_node(ast_node_bind);

    /* Dig into the binds */
    if ((ret = mls_icodegen_node_form_leta_single(err, \
                context, \
                icg_fcb_block, \
                ast_node_bind, \
                ast_node_body, \
                list_node_bind_count)) != 0)
    { goto fail; }

    goto done;
fail:
done:
    return ret;
}

