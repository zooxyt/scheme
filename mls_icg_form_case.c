/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : case
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
#include "mls_icg_form_case.h"

static int mls_icodegen_node_form_case_item(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_key, \
        struct mls_ast_node *ast_node_body, \
        int final_one, \
        uint32_t id_null, \
        struct multiply_offset_item_pack *offset_item_pack_to_tail, \
        struct multiply_offset_item_pack *offset_item_pack_to_next_branch, \
        struct multiply_offset_item_pack *offset_item_pack_begin_of_branch)
{
    int ret = 0;
    uint32_t instrument_number;
    int list_node_count_branch;

    /* Beginning of a branch */
    instrument_number = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    if (multiply_offset_item_pack_push_back(offset_item_pack_begin_of_branch, instrument_number) != 0)
    { goto fail; }

    /* Duplicate the key */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }
    /* Push test key */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block,  \
                    ast_node_key)) != 0)
    { goto fail; }
    /* Compare */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_EQ, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }
    /* Failed, to next branch */

    if (final_one == 0)
    {
        instrument_number = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
        if (multiply_offset_item_pack_push_back(offset_item_pack_to_next_branch, instrument_number) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0)
        { goto fail; }

        /* Pop original key */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id_null)) != 0) { goto fail; }
        /* Matched */
        list_node_count_branch = mls_icg_count_list_node(ast_node_body);
        if (list_node_count_branch == 0)
        {
            /* Push none */
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }
        }
        else
        {
            /* Push body */
            if ((ret = mls_icodegen_node_body(err, \
                            context, \
                            icg_fcb_block,  \
                            ast_node_body)) != 0)
            { goto fail; }
        }
        /* Jump to the end */
        instrument_number = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
        if (multiply_offset_item_pack_push_back(offset_item_pack_to_tail, instrument_number) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0)
        { goto fail; }
    }
    goto done;
fail:
done:
    return ret;
}

int mls_icodegen_node_form_case(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int list_node_count_branch;
    int idx;

    struct mls_ast_node *ast_node_key;
    struct mls_ast_node *ast_node_branches;

    struct mls_ast_node *ast_node_branch_cur;
    struct mls_ast_node *ast_node_branch_cur_in;

    struct mls_ast_node *ast_node_branch_cur_in_element_cur;
    struct mls_ast_node *ast_node_branch_cur_in_element_cur_in;

    uint32_t id_null;
    struct multiply_offset_item_pack *offset_item_pack_to_tail = NULL;
    struct multiply_offset_item_pack *offset_item_pack_to_next_branch = NULL;
    struct multiply_offset_item_pack *offset_item_pack_begin_of_branch = NULL;
    uint32_t instrument_number_tail;
    struct multiply_offset_item *cur_item_1 = NULL, *cur_item_2 = NULL;

    if ((offset_item_pack_to_tail = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
    if ((offset_item_pack_to_next_branch = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
    if ((offset_item_pack_begin_of_branch = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

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
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d error: case: bad syntax, must contains at least key", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Generate icode */

    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
    { goto fail; }

    /* Push the key */
    ast_node_key = mls_icg_car(ast_node);
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block,  \
                    ast_node_key)) != 0)
    { goto fail; }

    /* Branches */
    ast_node_branches = mls_icg_cdr(ast_node);
    ast_node_branch_cur = ast_node_branches;
    for (idx = 1; idx < list_node_count; idx++)
    {
        ast_node_branch_cur_in = mls_icg_car(ast_node_branch_cur);
        list_node_count_branch = mls_icg_count_list_node(ast_node_branch_cur_in);

        if (list_node_count_branch == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: case: bad syntax, no content in branch", \
                    mls_ast_node_ln(ast_node_branch_cur), mls_ast_node_col(ast_node_branch_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }

        if (mls_icg_is_node_cons(mls_icg_car(ast_node_branch_cur_in)) != 0)
        {
            /* Cons in this branch */

            ast_node_branch_cur_in_element_cur = mls_icg_car(ast_node_branch_cur_in);
            while (ast_node_branch_cur_in_element_cur != NULL)
            {
                ast_node_branch_cur_in_element_cur_in = mls_icg_car(ast_node_branch_cur_in_element_cur);

                if ((ret = mls_icodegen_node_form_case_item(err, \
                                context, \
                                icg_fcb_block, \
                                ast_node_branch_cur_in_element_cur_in, \
                                mls_icg_cdr(ast_node_branch_cur_in), \
                                (idx == list_node_count - 1) && (mls_icg_cdr(ast_node_branch_cur_in_element_cur) == NULL) ? 1 : 0, \
                                id_null, \
                                offset_item_pack_to_tail, \
                                offset_item_pack_to_next_branch, \
                                offset_item_pack_begin_of_branch)) != 0)
                { goto fail; }

                ast_node_branch_cur_in_element_cur = mls_icg_cdr(ast_node_branch_cur_in_element_cur);
            }
        }
        else
        {
            /* Solo test in this branch */

            if ((ret = mls_icodegen_node_form_case_item(err, \
                            context, \
                            icg_fcb_block, \
                            mls_icg_car(ast_node_branch_cur_in), \
                            mls_icg_cdr(ast_node_branch_cur_in), \
                            idx == list_node_count - 1 ? 1 : 0, \
                            id_null, \
                            offset_item_pack_to_tail, \
                            offset_item_pack_to_next_branch, \
                            offset_item_pack_begin_of_branch)) != 0)
            { goto fail; }
        }

        /* Next branch */
        ast_node_branch_cur = mls_icg_cdr(ast_node_branch_cur);
    }

    /* Missed matched all */
    /* Pop original key */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id_null)) != 0) { goto fail; }
    /* Push none */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }

    instrument_number_tail = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    cur_item_1 = offset_item_pack_to_tail->begin;
    while (cur_item_1 != NULL)
    {
        mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, instrument_number_tail);
        cur_item_1 = cur_item_1->next; 
    }

    cur_item_1 = offset_item_pack_to_next_branch->begin;
    cur_item_2 = offset_item_pack_begin_of_branch->begin;
    if (cur_item_2 != NULL)
    {
        if (cur_item_2->next != NULL)
        {
            cur_item_2 = cur_item_2->next;
            while (cur_item_2 != NULL)
            {
                mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, cur_item_2->offset);
                cur_item_1 = cur_item_1->next; 
                cur_item_2 = cur_item_2->next; 
            }
        }
    }
    if (cur_item_1 != NULL)
    {
        mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, instrument_number_tail);
    }

    goto done;
fail:
done:
    if (offset_item_pack_to_tail != NULL) multiply_offset_item_pack_destroy(offset_item_pack_to_tail);
    if (offset_item_pack_begin_of_branch != NULL) multiply_offset_item_pack_destroy(offset_item_pack_begin_of_branch);
    if (offset_item_pack_to_next_branch != NULL) multiply_offset_item_pack_destroy(offset_item_pack_to_next_branch);
    return ret;
}

