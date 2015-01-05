/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Special Procedures
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
#include "mls_icg_proc_special.h"

int mls_icodegen_node_proc_special(struct multiple_error *err, \
        struct mls_icg_context *context,
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
    procedure_item = mls_icg_proc_lut_items_special + procedure_idx;

    switch (procedure_item->op)
    {
        case MLS_ICG_PROC_OP_CALL:

            if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                            context->customizable_built_in_procedure_list, \
                            ast_node_proc_atom->atom->str, ast_node_proc_atom->atom->len)) != 0)
            { goto fail; }

            /* Push arguments */
            while (ast_node_args_sub != NULL)
            {
                if ((ret = mls_icodegen_node(err, context, icg_fcb_block, \
                                mls_icg_car(ast_node_args_sub))) != 0)
                { goto fail; }
                ast_node_args_sub = mls_icg_cdr(ast_node_args_sub);
            }
            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            if ((ret = multiply_resource_get_id( \
                            err, 
                            context->icode, \
                            context->res_id, \
                            &id, \
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

