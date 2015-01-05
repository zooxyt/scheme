/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : lambda
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
#include "mls_icg_body.h"

#include "mls_icg_form_lambda.h"

/* single arg */
static int mls_icodegen_node_form_lambda_arg( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg)
{
    int ret = 0;
    struct mls_ast_node_atom *ast_node_atom;
    uint32_t id;

    if (mls_icg_is_node_atom_id(ast_node_arg) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: lambda: parameter should be identifier", \
                mls_ast_node_ln(ast_node_arg), mls_ast_node_col(ast_node_arg));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_atom = ast_node_arg->ptr;
    if ((ret = multiply_resource_get_id( \
                    err, \
                    icode, \
                    res_id, \
                    &id, \
                    ast_node_atom->atom->str, \
                    ast_node_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_ARGC, id)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

/* args-list */ 
static int mls_icodegen_node_form_lambda_args_list(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args_list)
{
    int ret = 0;
    struct mls_ast_node *ast_node_args_list_sub = NULL;

    ast_node_args_list_sub = ast_node_args_list;
    while (ast_node_args_list_sub != NULL)
    {
        /* Receive an argument */
        if ((ret = mls_icodegen_node_form_lambda_arg( \
                        err, \
                        context->icode, \
                        context->res_id, \
                        icg_fcb_block, \
                        mls_icg_car(ast_node_args_list_sub))) != 0)
        { goto fail; }

        /* Next argument */
        ast_node_args_list_sub = mls_icg_cdr(ast_node_args_list_sub);
    }

    goto done;
fail:
done:
    return ret;
}

/* (lambda (args-list) body) */ 
int mls_icodegen_node_form_lambda_style1(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args_list, \
        struct mls_ast_node *ast_node_body)
{
    int ret = 0;
    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

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

    /* Arguments */
    if ((ret = mls_icodegen_node_form_lambda_args_list(err, context, new_icg_fcb_block, \
                    ast_node_args_list)) != 0)
    { goto fail; }

    /* Body */
    if ((ret = mls_icodegen_node_body(err, context, new_icg_fcb_block, ast_node_body)) != 0)
    { goto fail; }

    /* Return */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }

    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_LAMBDAMK, (uint32_t)(context->icg_fcb_block_list->size), MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0)
    { goto fail; }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }
    new_icg_fcb_block = NULL;
    /* Append blank export section */
    if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }
    new_export_section_item = NULL;

    goto done;
fail:
    if (new_icg_fcb_block != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}

/* (lambda (arg) body) */ 
int mls_icodegen_node_form_lambda_style2(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg, \
        struct mls_ast_node *ast_node_body)
{
    int ret = 0;
    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

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

    if (ast_node_arg != NULL)
    {
        /* Arguments */
        if ((ret = mls_icodegen_node_form_lambda_arg( \
                        err, \
                        context->icode, \
                        context->res_id, \
                        new_icg_fcb_block, \
                        ast_node_arg)) != 0)
        { goto fail; }
    }

    /* Body */
    if ((ret = mls_icodegen_node_body(err, \
                    context, \
                    new_icg_fcb_block, \
                    ast_node_body)) != 0)
    { goto fail; }

    /* Return */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }

    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_LAMBDAMK, (uint32_t)(context->icg_fcb_block_list->size), MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0)
    { goto fail; }

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


/* (lambda ? ) */
int mls_icodegen_node_form_lambda(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;

    /* (lambda <name:id> body)*/ 
    struct mls_ast_node *ast_node_list_name = NULL;
    /* (lambda (args-list) body)*/ 
    struct mls_ast_node *ast_node_args_list = NULL;
    /* Lambda body */
    struct mls_ast_node *ast_node_body = NULL;

    /* elements count */
    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count < 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: lambda: bad syntax, " \
                "parameter list and expression body expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count == 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: lambda: expression body expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Arguments Style */
    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node)) == 0)
    {
        /* (lambda (args-list) body)*/ 

        ast_node_args_list = mls_icg_car(ast_node);
        ast_node_body = mls_icg_cdr(ast_node);

        if ((ret = mls_icodegen_node_form_lambda_style1(err, context, icg_fcb_block, \
                        ast_node_args_list, \
                        ast_node_body)) != 0)
        { goto fail; }
    }
    else
    {
        /* (lambda <name:id> expression-list)*/ 

        ast_node_list_name = mls_icg_car(ast_node);

        (void)ast_node_list_name;
        MULTIPLE_ERROR_NOT_IMPLEMENTED();
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }


    goto done;
fail:
done:
    return ret;
}

