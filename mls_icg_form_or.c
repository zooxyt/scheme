/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : or
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
#include "vm_types.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_define.h"
#include "mls_icg_body.h"
#include "mls_icg_form_or.h"

int mls_icodegen_node_form_or(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    uint32_t true_id, false_id;
    uint32_t instrument_number = 0;
    uint32_t instrument_number_to_tail = 0;
    uint32_t instrument_number_tail = 0;
    int idx;
    uint32_t type_id;
    struct mls_ast_node *ast_node_cur;
    struct mls_ast_node *ast_node_test;
    struct multiply_offset_item_pack *offset_item_pack_to_tail = NULL;
    struct multiply_offset_item_pack *offset_item_pack_to_next_branch = NULL;
    struct multiply_offset_item_pack *offset_item_pack_begin_of_branch = NULL;
    struct multiply_offset_item *cur_item_1 = NULL, *cur_item_2 = NULL;

    if ((offset_item_pack_to_tail = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    if ((offset_item_pack_to_next_branch = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    if ((offset_item_pack_begin_of_branch = multiply_offset_item_pack_new()) == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    if ((ret = multiply_resource_get_true(err, context->icode, context->res_id, &true_id)) != 0)
    { goto fail; }
    if ((ret = multiply_resource_get_false(err, context->icode, context->res_id, &false_id)) != 0)
    { goto fail; }

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count == 0)
    {
        /* (or) => #f */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, false_id)) != 0)
        { goto fail; }
    }
    else
    {
        idx = 0;
        ast_node_cur = ast_node;
        while (idx != list_node_count)
        {
            ast_node_test = mls_icg_car(ast_node_cur);

            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            ast_node_test)) != 0)
            { goto fail; }

			/* Convert non-boolean to true */
			ret = virtual_machine_object_type_name_to_id(&type_id, "bool", 4);
			if (ret != 0)
			{
				multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'bool\' isn't a valid type name");
				ret = -MULTIPLE_ERR_ICODEGEN;
				goto fail;
			}
			if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CONVERT, type_id)) != 0) { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }

            /* To next test */
            instrument_number = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if (multiply_offset_item_pack_push_back(offset_item_pack_to_next_branch, instrument_number) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }

            /* Or we push #t */
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, true_id)) != 0)
            { goto fail; }

            /* Jump to tail */
            instrument_number_to_tail = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
            if (multiply_offset_item_pack_push_back(offset_item_pack_to_tail, instrument_number_to_tail) != 0)
            { goto fail; }

            instrument_number = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
            if (multiply_offset_item_pack_push_back(offset_item_pack_begin_of_branch, instrument_number) != 0)
            { goto fail; }

            ast_node_cur = mls_icg_cdr(ast_node_cur);
            idx++;
        }
        /* #f when passed all tests */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, false_id)) != 0)
        { goto fail; }

        /* Tail */
        instrument_number_tail = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

        cur_item_1 = offset_item_pack_to_tail->begin;
        while (cur_item_1 != NULL)
        {
            mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, instrument_number_tail);
            cur_item_1 = cur_item_1->next; 
        }

        cur_item_1 = offset_item_pack_to_next_branch->begin;
        cur_item_2 = offset_item_pack_begin_of_branch->begin;
        while (cur_item_1 != NULL)
        {
            mls_icg_fcb_block_link(icg_fcb_block, cur_item_1->offset, cur_item_2->offset);
            cur_item_1 = cur_item_1->next; 
            cur_item_2 = cur_item_2->next; 
        }
    }

    goto done;
fail:
done:
    if (offset_item_pack_to_tail != NULL) multiply_offset_item_pack_destroy(offset_item_pack_to_tail);
    if (offset_item_pack_to_next_branch != NULL) multiply_offset_item_pack_destroy(offset_item_pack_to_next_branch);
    if (offset_item_pack_begin_of_branch != NULL) multiply_offset_item_pack_destroy(offset_item_pack_begin_of_branch);
    return ret;
}

