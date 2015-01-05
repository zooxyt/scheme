/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : do
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
#include "mls_icg_form_let.h"
#include "mls_icg_form_do.h"


/* (do 
 *   ((variable init step) ...) 
 *   (test expression ...) 
 *   command)
 */

int mls_icodegen_node_form_do(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int list_node_bind_count;
    int idx, idx2;
    uint32_t id_var;
    uint32_t id;
    uint32_t id_null;
    uint32_t instrument_number_test, instrument_number_jmp, instrument_number_jmpc, instrument_number_skip_test;

    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

    /* Binds */
    struct mls_ast_node *ast_node_bind;
    struct mls_ast_node *ast_node_bind_branch_cur;
    struct mls_ast_node *ast_node_bind_branch_cur_in;
    struct mls_ast_node *ast_node_bind_var;
    struct mls_ast_node *ast_node_bind_exp;
    struct mls_ast_node_atom *ast_node_var_atom;

    /* Test Condition */
    struct mls_ast_node *ast_node_test;

    /* Loop Body */
    struct mls_ast_node *ast_node_body;

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

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count <= 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, " \
                "must contains at least binding and exit condition", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else 
    {
        if (mls_icg_is_node_cons(ast_node) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, no binding", \
                    mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        if (mls_icg_is_node_cons(mls_icg_cdr(ast_node)) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, exit condition", \
                    mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
    }

    /* Null */
    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
    { goto fail; }

    ast_node_bind = mls_icg_car(ast_node);
    ast_node_test = mls_icg_car(mls_icg_cdr(ast_node));
    ast_node_body = mls_icg_cdr(mls_icg_cdr(ast_node));

    /* Binds */
    list_node_bind_count = mls_icg_count_list_node(ast_node_bind);

    /* Expressions */
    ast_node_bind_branch_cur = ast_node_bind;
    while (ast_node_bind_branch_cur != NULL)
    {
        ast_node_bind_branch_cur_in = mls_icg_car(ast_node_bind_branch_cur);

        switch (mls_icg_count_list_node(ast_node_bind_branch_cur_in))
        {
            case 2:
                /* (variable init) */
            case 3:
                /* (variable init step) */

                ast_node_bind_exp = mls_icg_car(mls_icg_cdr(ast_node_bind_branch_cur_in));
                if ((ret = mls_icodegen_node_form_let_item(err, \
                                context, \
                                new_icg_fcb_block, \
                                ast_node_bind_exp)) != 0)
                { goto fail; }
                break;

                /*
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, " \
                        "(variable init step) style not supported", \
                        mls_ast_node_ln(ast_node_bind_branch_cur), mls_ast_node_col(ast_node_bind_branch_cur));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
                break;
                */

            default:
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, " \
                        "not an identifier and expression for a binding", \
                        mls_ast_node_ln(ast_node_bind_branch_cur), mls_ast_node_col(ast_node_bind_branch_cur));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
        }

        ast_node_bind_branch_cur = mls_icg_cdr(ast_node_bind_branch_cur);
    }

    /* Save to variables (in reverse order) */
    for (idx = list_node_bind_count - 1; idx >= 0; idx--)
    {
        /* Locate to the node */
        idx2 = idx;
        ast_node_bind_branch_cur = ast_node_bind;
        while (idx2-- > 0)
        { ast_node_bind_branch_cur = mls_icg_cdr(ast_node_bind_branch_cur); }
        ast_node_bind_branch_cur_in = mls_icg_car(ast_node_bind_branch_cur);

        ast_node_bind_var = mls_icg_car(ast_node_bind_branch_cur_in);

        /* Save to variable */
        ast_node_var_atom = ast_node_bind_var->ptr;
        if ((ret = multiply_resource_get_id( \
                        err, \
                        context->icode, \
                        context->res_id, \
                        &id_var, \
                        ast_node_var_atom->atom->str, \
                        ast_node_var_atom->atom->len)) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_var)) != 0) { goto fail; }
    }

    /* Exit condition */
    list_node_count = mls_icg_count_list_node(ast_node_test);
    if (list_node_count == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: do: bad syntax, " \
                "exit condition expected", \
                mls_ast_node_ln(ast_node), mls_ast_node_col(ast_node));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    instrument_number_test = (uint32_t)(new_icg_fcb_block->size);
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    new_icg_fcb_block,  \
                    mls_icg_car(ast_node_test))) != 0)
    { goto fail; }

    /* Exit when true */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }
    instrument_number_jmpc = (uint32_t)(new_icg_fcb_block->size);
    if ((ret = mls_icg_fcb_block_append_with_configure_type(new_icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
    if (list_node_count == 1)
    {
        /* Push none */
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
    }
    else
    {
        /* Push rest expression */
        if ((ret = mls_icodegen_node_body(err, \
                        context, \
                        new_icg_fcb_block, \
                        mls_icg_cdr(ast_node_test))) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
    }
    instrument_number_skip_test = (uint32_t)(new_icg_fcb_block->size);
    mls_icg_fcb_block_link(new_icg_fcb_block, instrument_number_jmpc, instrument_number_skip_test);

    /* Body */
    if ((ret = mls_icodegen_node_body(err, \
                    context, \
                    new_icg_fcb_block, \
                    ast_node_body)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_null)) != 0) { goto fail; }

    /* Step */
    ast_node_bind_branch_cur = ast_node_bind;
    while (ast_node_bind_branch_cur != NULL)
    {
        ast_node_bind_branch_cur_in = mls_icg_car(ast_node_bind_branch_cur);

        switch (mls_icg_count_list_node(ast_node_bind_branch_cur_in))
        {
            case 3:
                /* (variable init step) */
                if ((ret = mls_icodegen_node(err, \
                                context, \
                                new_icg_fcb_block, \
                                mls_icg_cddar(ast_node_bind_branch_cur_in))) != 0)
                { goto fail; }

                ast_node_var_atom = mls_icg_car(ast_node_bind_branch_cur_in)->ptr;
                if ((ret = multiply_resource_get_id( \
                                err, \
                                context->icode, \
                                context->res_id, \
                                &id_var, \
                                ast_node_var_atom->atom->str, \
                                ast_node_var_atom->atom->len)) != 0)
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_var)) != 0) { goto fail; }
                break;
        }

        ast_node_bind_branch_cur = mls_icg_cdr(ast_node_bind_branch_cur);
    }

    /* Jump back to test after executing body */
    instrument_number_jmp = (uint32_t)(new_icg_fcb_block->size);
    if ((ret = mls_icg_fcb_block_append_with_configure_type(new_icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
    mls_icg_fcb_block_link(new_icg_fcb_block, instrument_number_jmp, instrument_number_test);

    /* Argument Count */
    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 0)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_LAMBDAMK, (uint32_t)(context->icg_fcb_block_list->size), MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0) 
    { goto fail; }
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

