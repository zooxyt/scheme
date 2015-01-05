/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures : functional
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
#include "vm_types.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_proc_lut.h"
#include "mls_icg_proc_std.h"
#include "mls_icg_proc_std_functional.h"


int mls_icodegen_node_proc_std_functional(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    const struct mls_icg_proc_lut_item *procedure_item;
    uint32_t id;
    uint32_t type_id;

    (void)ast_node_proc;

    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->value)
    {

        case MLS_ICG_PROCEDUREP:

            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            ret = virtual_machine_object_type_name_to_id(&type_id, "function", 8);
            if (ret != 0) 
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'function\' isn't a valid type name");
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEP, type_id)) != 0) { goto fail; }

            break;

        case MLS_ICG_APPLY:
        case MLS_ICG_MAP:
        case MLS_ICG_FOR_EACH:

            /* List */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }

            /* Procedure */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            switch (procedure_item->value)
            {
                case MLS_ICG_APPLY:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "apply", 5)) != 0)
                    { goto fail; }
                    break;

                case MLS_ICG_MAP:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "map", 3)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_FOR_EACH:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "for-each", 8)) != 0)
                    { goto fail; }
                    break;
            }

            /* Call map */
            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 2)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            switch (procedure_item->value)
            {
                case MLS_ICG_APPLY:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, 
                                    "apply", 5)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_MAP:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, 
                                    "map", 3)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_FOR_EACH:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, 
                                    "for-each", 8)) != 0)
                    { goto fail; }
                    break;
            }
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

