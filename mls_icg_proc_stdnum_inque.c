/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Numeric Procedures : Inequalities
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

#include "multiply_offset.h"

#include "vm_opcode.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_proc_stdnum.h"
#include "mls_icg_proc_stdnum_inque.h"

static int procedure_inque_reverse(int reverse, int procedure_idx)
{
    if (reverse != 0)
    {
        switch (procedure_idx)
        {
            case MLS_ICG_L: return MLS_ICG_GE;
            case MLS_ICG_LE: return MLS_ICG_G;
            case MLS_ICG_G: return MLS_ICG_LE;
            case MLS_ICG_GE: return MLS_ICG_L;
            case MLS_ICG_EQ: return MLS_ICG_NE;
            case MLS_ICG_NE: return MLS_ICG_EQ;
            default: return MLS_ICG_EQ;
        }
    }
    else
    {
        return procedure_idx;
    }
}

int mls_icodegen_node_proc_stdnum_inque(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    struct mls_ast_node *ast_node_args_sub = ast_node_args, *ast_node_args_sub_2 = NULL;
    int ast_node_args_count;
    uint32_t id_none, id_false, id_true;
    struct multiply_offset_item_pack *offset_item_pack_jump_start_points = NULL;
    struct multiply_offset_item *offset_item_cur;
    uint32_t instrument_number_jmpc;
    uint32_t instrument_number_false, instrument_number_jmp, instrument_number_end;
    int reverse = 0;

    (void)ast_node_proc;

    offset_item_pack_jump_start_points = multiply_offset_item_pack_new();

    /* Checking arguments number */
    ast_node_args_count = mls_icg_count_list_node(ast_node_args);

    /* Constants */
    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_none)) != 0)
    { goto fail; }
    if ((ret = multiply_resource_get_false(err, context->icode, context->res_id, &id_false)) != 0)
    { goto fail; }
    if ((ret = multiply_resource_get_true(err, context->icode, context->res_id, &id_true)) != 0)
    { goto fail; }

    /* Push the first argument */
    ast_node_args_count -= 1;
    ast_node_args_sub_2 = mls_icg_car(ast_node_args_sub);
    if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                    ast_node_args_sub_2)) != 0)
    { goto fail; }
    ast_node_args_sub = mls_icg_cdr(ast_node_args_sub);

    for (;;)
    {
        /* Pairly comparing */
        /*ast_node_args_sub_1 = ast_node_args_sub_2;*/
        ast_node_args_sub_2 = mls_icg_car(ast_node_args_sub);

        /* Push the next argument */
        if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                        ast_node_args_sub_2)) != 0)
        { goto fail; }

        /* Take operation */
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEUP, 0)) != 0) { goto fail; }
        switch (procedure_inque_reverse(reverse, procedure_idx))
        {
            case MLS_ICG_L:
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_L, 0)) != 0) { goto fail; }
                break;
            case MLS_ICG_LE:
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LE, 0)) != 0) { goto fail; }
                break;
            case MLS_ICG_G:
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_G, 0)) != 0) { goto fail; }
                break;
            case MLS_ICG_GE:
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_GE, 0)) != 0) { goto fail; }
                break;
            case MLS_ICG_EQ:
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_EQ, 0)) != 0) { goto fail; }
                break;
        }
        if (reverse == 0) reverse = 1; else reverse = 0;

        /* Not match */ 
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }
        instrument_number_jmpc = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
        if ((ret = multiply_offset_item_pack_push_back(offset_item_pack_jump_start_points, instrument_number_jmpc)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
        /* Matched, go to next test */

        /* Next node */
        ast_node_args_sub = mls_icg_cdr(ast_node_args_sub);
        ast_node_args_count -= 1;
        if (ast_node_args_count == 0) break;
    }
    /* No miss match, push true and jump to end */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_true)) != 0) { goto fail; }
    instrument_number_jmp = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }
    /* False block */
    instrument_number_false = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_false)) != 0) { goto fail; }
    instrument_number_end = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

    offset_item_cur = offset_item_pack_jump_start_points->begin;
    while (offset_item_cur != NULL)
    {
        mls_icg_fcb_block_link(icg_fcb_block, \
                offset_item_cur->offset, \
                instrument_number_false);
        offset_item_cur = offset_item_cur->next; 
    }
    mls_icg_fcb_block_link(icg_fcb_block, instrument_number_jmp, instrument_number_end);

    goto done;
fail:
done:
    if (offset_item_pack_jump_start_points != NULL)
    { multiply_offset_item_pack_destroy(offset_item_pack_jump_start_points); }
    return ret;
}

