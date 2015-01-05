/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : define
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
#include "mls_icg_form_lambda.h"
#include "mls_icg_body.h"

/* Define Function */
/* (define (name args) body) */
static int mls_icodegen_node_define_procedure(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_name,
        struct mls_ast_node *ast_node_args,
        struct mls_ast_node *ast_node_body)
{
    int ret = 0;
    uint32_t id;
    struct mls_ast_node_atom *ast_node_name_atom;

    ast_node_name_atom = ast_node_name->ptr;

    if (ast_node_name_atom->atom->value != TOKEN_IDENTIFIER)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: define: bad syntax, \'%s\' should be identifier", \
                mls_ast_node_ln(ast_node_name), \
                mls_ast_node_col(ast_node_name), \
                mls_ast_node_str(ast_node_name)); \
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Lambda */
    if ((ret = mls_icodegen_node_form_lambda_style1(err, context, icg_fcb_block, \
                    ast_node_args, \
                    ast_node_body)) != 0)
    { goto fail; }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }

    /* Save in variable */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_name_atom->atom->str, \
                    ast_node_name_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPCL, id)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;

}

/* Define variable */
/* (define var-name expression) */
static int mls_icodegen_node_define_variable(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_name,
        struct mls_ast_node *ast_node_body)
{
    int ret = 0;
    uint32_t id;
    struct mls_ast_node_atom *ast_node_name_atom;

    ast_node_name_atom = ast_node_name->ptr;

    if (ast_node_name_atom->atom->value != TOKEN_IDENTIFIER)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: define: bad syntax, \'%s\' should be identifier", \
                mls_ast_node_ln(ast_node_name), \
                mls_ast_node_col(ast_node_name), \
                mls_ast_node_str(ast_node_name)); \
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Expression */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_body)) != 0)
    { goto fail; }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }

    /* Save in variable */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_name_atom->atom->str, \
                    ast_node_name_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPCL, id)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

int mls_icodegen_node_form_define(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_ast_node *ast_node_cur = ast_node;
    int list_node_count;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: define: bad syntax, variable and expression expected", \
                mls_ast_node_ln(context->ast_node_parent), \
                mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count == 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: define: bad syntax, (missing expression after identifier)", \
                mls_ast_node_ln(context->ast_node_parent), \
                mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Binding type */
    switch (mls_icg_car(ast_node_cur)->type)
    {
        case MLS_AST_NODE_ATOM:
            /* Define variable */
            /* (define var-name expression) */
            if (list_node_count > 2)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                        "%d:%d: error: define: bad syntax, " \
                        "(multiple expressions after identifier)", \
                        mls_ast_node_ln(mls_icg_car(ast_node_cur)), \
                        mls_ast_node_col(mls_icg_car(ast_node_cur)));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
            if ((ret = mls_icodegen_node_define_variable(err, context, icg_fcb_block, \
                            mls_icg_car(ast_node_cur), /* name */
                            mls_icg_cdar(ast_node_cur) /* body */
                            )) != 0)
            { goto fail; }
            break;
        case MLS_AST_NODE_CONS:
            /* Define procedure */
            /* (define (procedure-name [args]) <body>)*/
            if ((ret = mls_icodegen_node_define_procedure(err, context, icg_fcb_block, \
                            mls_icg_caar(ast_node_cur), /* name */
                            mls_icg_cadr(ast_node_cur), /* arguments */
                            mls_icg_cdr(ast_node_cur) /* body */
                            )) != 0)
            { goto fail; }
            break;
        default:
            MULTIPLE_ERROR_INTERNAL();
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
            break;
    }

    goto done;
fail:
done:
    return ret;
}

