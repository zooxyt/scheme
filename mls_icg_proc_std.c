/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures
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
#include "mls_icg_proc_lut.h"
#include "mls_icg_proc_std.h"
#include "mls_icg_proc_invoke.h"

#include "mls_icg_proc_std_string.h"
#include "mls_icg_proc_std_char.h"
#include "mls_icg_proc_std_vector.h"
#include "mls_icg_proc_std_symbol.h"
#include "mls_icg_proc_std_pair_list.h"
#include "mls_icg_proc_std_cont.h"
#include "mls_icg_proc_std_functional.h"
#include "mls_icg_proc_std_boolean.h"


struct mls_icodegen_node_proc_std_item
{
    const int type; 

    int (*func)(struct multiple_error *err, \
            struct mls_icg_context *context, \
            struct mls_icg_fcb_block *icg_fcb_block, \
            struct mls_ast_node *ast_node_proc,
            struct mls_ast_node *ast_node_args,
            const int procedure_idx);
};

static struct mls_icodegen_node_proc_std_item mls_icodegen_node_proc_std_items[] = 
{
    { MLS_ICG_PROC_TYPE_STRINGS, mls_icodegen_node_proc_std_string },
    { MLS_ICG_PROC_TYPE_CHARACTERS, mls_icodegen_node_proc_std_char },
    { MLS_ICG_PROC_TYPE_VECTORS, mls_icodegen_node_proc_std_vectors },
    { MLS_ICG_PROC_TYPE_SYMBOLS, mls_icodegen_node_proc_std_symbols },
    { MLS_ICG_PROC_TYPE_PAIRS_LISTS, mls_icodegen_node_proc_std_pairs_lists },
    { MLS_ICG_PROC_TYPE_CONTINUATIONS, mls_icodegen_node_proc_std_continuations },
    { MLS_ICG_PROC_TYPE_FUNCTIONAL, mls_icodegen_node_proc_std_functional },
    { MLS_ICG_PROC_TYPE_BOOLEANS, mls_icodegen_node_proc_std_booleans },
};
#define MLS_ICODEGEN_NODE_PROC_STD_ITEM_COUNT (sizeof(mls_icodegen_node_proc_std_items)/sizeof(struct mls_icodegen_node_proc_std_item))

int mls_icodegen_node_proc_std(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc, \
        struct mls_ast_node *ast_node_args, \
        const int procedure_idx)
{
    int ret = 0;
    struct mls_ast_node_atom *ast_node_proc_atom;
    struct mls_ast_node *ast_node_args_sub;
    int ast_node_args_count;
    const struct mls_icg_proc_lut_item *procedure_item;
    uint32_t id;
    int matched = 0;
    size_t idx;

    if (mls_icg_is_node_atom_id(ast_node_proc) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: procedure name should be identifier", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_proc_atom = ast_node_proc->ptr;
    ast_node_args_sub = ast_node_args;

    /* Checking arguments number */
    ast_node_args_count = mls_icg_count_list_node(ast_node_args);
    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->op)
    {
        case MLS_ICG_PROC_OP_MANUAL:
            for (idx = 0; idx != MLS_ICODEGEN_NODE_PROC_STD_ITEM_COUNT; idx++)
            {
                if (mls_icodegen_node_proc_std_items[idx].type == procedure_item->type)
                {
                    matched = 1;
                    break;
                }
            }
            if (matched != 0)
            {
                if ((mls_icodegen_node_proc_std_items[idx].func(err, \
                                    context, \
                                    icg_fcb_block, \
                                    ast_node_proc, \
                                    ast_node_args, \
                                    procedure_idx)) != 0)
                { goto fail; }
            }
            else
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: undefined operation on procedure \'%s\'", \
                        mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent), \
                        procedure_item->str);
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
                break;
            }
            break;

        case MLS_ICG_PROC_OP_CALL:

            if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                            context->customizable_built_in_procedure_list, \
                            ast_node_proc_atom->atom->str, ast_node_proc_atom->atom->len)) != 0)
            { goto fail; }

            /* Reversely push arguments */
            if (ast_node_args_count > 0)
            {
                if ((ret = mls_icodegen_node_proc_push_arg_reversely(err, \
                                context, \
                                icg_fcb_block, \
                                ast_node_args_sub, ast_node_args_count)) != 0)
                { goto fail; }
            }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, &id, 
                            ast_node_proc_atom->atom->str, \
                            ast_node_proc_atom->atom->len)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }
            break;
        default:
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: undefined operation on procedure \'%s\'", \
                    mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent), \
                    procedure_item->str);
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
            break;
    }

    goto done;
fail:
done:
    return ret;
}

