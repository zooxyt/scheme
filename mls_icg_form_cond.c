/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : cond
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

#include "multiply.h"
#include "multiply_offset.h"

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
#include "mls_icg_form_cond.h"

int mls_icodegen_node_form_cond(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int list_node_count_branch;
    int idx;
    uint32_t instrument_number_branch_begin = 0;
    uint32_t instrument_number_branch_end = 0;
    uint32_t instrument_number_to_tail = 0;
    uint32_t instrument_number_tail = 0;
    struct mls_ast_node *ast_node_branch_cur = NULL;
    struct mls_ast_node *ast_node_branch_cur_in = NULL;
    struct mls_ast_node *ast_node_test = NULL;
    struct mls_ast_node *ast_node_body = NULL;
    uint32_t id, id_null;

    struct multiply_offset_item_pack *offset_item_pack_to_tail = NULL;
    struct multiply_offset_item_pack *offset_item_pack_branch_begin = NULL;
    struct multiply_offset_item_pack *offset_item_pack_branch_end = NULL;
    struct multiply_offset_item *cur_item_1 = NULL, *cur_item_2 = NULL;

    if ((offset_item_pack_to_tail = multiply_offset_item_pack_new()) == NULL) 
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail; 
    }

    if ((offset_item_pack_branch_begin = multiply_offset_item_pack_new()) == NULL) 
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail; 
    }

    if ((offset_item_pack_branch_end = multiply_offset_item_pack_new()) == NULL) 
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail; 
    }

    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
    { goto fail; }

    /* Syntax Checking */
    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count < 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count == 0)
    {
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
    }
    ast_node_branch_cur = ast_node;
    for (idx = 0; idx < list_node_count; idx++)
    {
        ast_node_branch_cur_in = mls_icg_car(ast_node_branch_cur);
        list_node_count_branch = mls_icg_count_list_node(ast_node_branch_cur_in);
        if (list_node_count_branch == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: cond: bad syntax, empty branch", \
                    mls_ast_node_ln(ast_node_branch_cur), mls_ast_node_col(ast_node_branch_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        if ((list_node_count_branch >= 1) && \
                (mls_icg_is_node_atom_id_with_name(mls_icg_car(ast_node_branch_cur_in), "else")))
        {
            if (idx != list_node_count - 1)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: cond: bad syntax, 'else' clause must be the final one", \
                        mls_ast_node_ln(ast_node_branch_cur_in), mls_ast_node_col(ast_node_branch_cur_in));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
            if (list_node_count_branch == 1)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: cond: bad syntax, missing expressions in 'else' clause", \
                        mls_ast_node_ln(ast_node_branch_cur_in), mls_ast_node_col(ast_node_branch_cur_in));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
        }
        ast_node_branch_cur = mls_icg_cdr(ast_node_branch_cur);
    }

    /* Generate icode */

    ast_node_branch_cur = ast_node;
    for (idx = 0; idx < list_node_count; idx++)
    {
        ast_node_branch_cur_in = mls_icg_car(ast_node_branch_cur);
        list_node_count_branch = mls_icg_count_list_node(ast_node_branch_cur_in);

        /* Type */
        if (list_node_count_branch == 1)
        {
            /* (value) */ 
            ast_node_body = mls_icg_car(ast_node_branch_cur_in);
            /* Directly return the value */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            ast_node_body)) != 0)
            { goto fail; }

            /* No need to jump to tail */
            break;
        }
        else if (list_node_count_branch > 1)
        {
            ast_node_test = mls_icg_car(ast_node_branch_cur_in);
            ast_node_body = mls_icg_car(mls_icg_cdr(ast_node_branch_cur_in));

            if (mls_icg_is_node_atom_id_with_name(mls_icg_car(ast_node_branch_cur_in), "else"))
            {
                /* 'else' */

                /* Dealing with body */
                if ((ret = mls_icodegen_node(err, \
                                context, \
                                icg_fcb_block,  \
                                ast_node_body)) != 0)
                { goto fail; }

                /* No need to jump to tail */
                break;
            }
            else
            {
                /* (test body) */

                /* Push condition */
                if ((ret = mls_icodegen_node(err, \
                                context, \
                                icg_fcb_block,  \
                                ast_node_test)) != 0)
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }
                /* Jump to next branch when miss-matched this branch */
                instrument_number_branch_begin = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
                if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }

                /* Dealing with body */
                if ((ret = mls_icodegen_node(err, \
                                context, \
                                icg_fcb_block,  \
                                ast_node_body)) != 0)
                { goto fail; }
                /* Jump to tail */
                instrument_number_to_tail = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
                if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
                if (multiply_offset_item_pack_push_back(offset_item_pack_to_tail, instrument_number_to_tail) != 0)
                { goto fail; }

                /* Pop body value */
                if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id)) != 0) 
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id)) != 0) { goto fail; }


                /* From branch begin jumps to here */
                instrument_number_branch_end = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

                if (multiply_offset_item_pack_push_back(offset_item_pack_branch_begin, instrument_number_branch_begin) != 0)
                { goto fail; }
                if (multiply_offset_item_pack_push_back(offset_item_pack_branch_end, instrument_number_branch_end) != 0)
                { goto fail; }
            }
        }
        /* Next branch */
        ast_node_branch_cur = mls_icg_cdr(ast_node_branch_cur);
    }

    instrument_number_tail = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    cur_item_1 = offset_item_pack_to_tail->begin;
    while (cur_item_1 != NULL)
    {
        mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, instrument_number_tail);
        cur_item_1 = cur_item_1->next; 
    }

    cur_item_1 = offset_item_pack_branch_begin->begin;
    cur_item_2 = offset_item_pack_branch_end->begin;
    while (cur_item_1 != NULL)
    {
        mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, cur_item_2->offset);
        cur_item_1 = cur_item_1->next; 
        cur_item_2 = cur_item_2->next; 
    }

    goto done;
fail:
done:
    if (offset_item_pack_to_tail != NULL) multiply_offset_item_pack_destroy(offset_item_pack_to_tail);
    if (offset_item_pack_branch_begin != NULL) multiply_offset_item_pack_destroy(offset_item_pack_branch_begin);
    if (offset_item_pack_branch_end != NULL) multiply_offset_item_pack_destroy(offset_item_pack_branch_end);
    return ret;
}

