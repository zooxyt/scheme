/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures : pairs and lists
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
#include "mls_icg_proc_std_pair_list.h"


static int mls_icodegen_node_proc_std_pairs_lists_list(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;
    int list_node_count;
    int idx, idx2;

    struct mls_ast_node *ast_node_cur, *ast_node_cur_in;

    list_node_count = mls_icg_count_list_node(ast_node_args);
    for (idx = list_node_count - 1; idx >= 0; idx--)
    {
        ast_node_cur = ast_node_args;
        idx2 = idx;
        while (idx2-- != 0) { ast_node_cur = mls_icg_cdr(ast_node_cur); }

        ast_node_cur_in = mls_icg_car(ast_node_cur);
        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block,  \
                        ast_node_cur_in)) != 0)
        { goto fail; }
    }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LSTMK, (uint32_t)list_node_count)) != 0) { goto fail; }
    goto done;
fail:
done:
    return ret;
}

int mls_icodegen_node_proc_std_pairs_lists(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    int ast_node_args_count;
    const struct mls_icg_proc_lut_item *procedure_item;
    uint32_t id;
    uint32_t type_id;
    struct mls_ast_node_atom *ast_node_atom;

    (void)ast_node_proc;

    ast_node_args_count = mls_icg_count_list_node(ast_node_args);
    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->value)
    {
        case MLS_ICG_PAIRP:
            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            ret = virtual_machine_object_type_name_to_id(&type_id, "pair", 4);
            if (ret != 0) 
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'pair\' isn't a valid type name");
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEP, type_id)) != 0) { goto fail; }

            break;

        case MLS_ICG_CONS:

            /* Push cdr */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr((ast_node_args))))) != 0)
            { goto fail; }

            /* Push car */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PAIRMK, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_CAR:
        case MLS_ICG_CDR: 

            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            switch (procedure_item->value)
            {
                case MLS_ICG_CAR:
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LSTCAR, 0)) != 0) { goto fail; }
                    break;
                case MLS_ICG_CDR:
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LSTCDR, 0)) != 0) { goto fail; }
                    break;
            }

            break;

        case MLS_ICG_LISTP:

            /* Push object */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            ret = virtual_machine_object_type_name_to_id(&type_id, "list", 4);
            if (ret != 0) 
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'list\' isn't a valid type name");
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEP, type_id)) != 0) { goto fail; }
            break;

        case MLS_ICG_LIST:

            if ((ret = mls_icodegen_node_proc_std_pairs_lists_list(err, \
                            context, \
                            icg_fcb_block, \
                            ast_node_args)) != 0)
            { goto fail; }

            break;

        case MLS_ICG_LENGTH:

            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SIZE, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_APPEND:

            /* Push element */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }

            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LSTADD, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_SET_CAR:
        case MLS_ICG_SET_CDR:

            /* Push member */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }

            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            switch (procedure_item->value)
            {
                case MLS_ICG_SET_CAR:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "set-car!", 8)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_SET_CDR:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "set-cdr!", 8)) != 0)
                    { goto fail; }
                    break;
            }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            switch (procedure_item->value)
            {
                case MLS_ICG_NULLP:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "null?", 5)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_REVERSE:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "reverse", 7)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_SET_CAR:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "set-car!", 8)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_SET_CDR:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "set-cdr!", 8)) != 0)
                    { goto fail; }
                    break;
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            if ((procedure_item->value) && (mls_icg_is_node_atom_id(mls_icg_car(ast_node_args))))
            {
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }
                ast_node_atom = mls_icg_car(ast_node_args)->ptr;
                if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                &id, \
                                ast_node_atom->atom->str, ast_node_atom->atom->len)) != 0)
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id)) != 0) { goto fail; }
                break;
            }

            break;

        case MLS_ICG_NULLP:
        case MLS_ICG_REVERSE:

            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            switch (procedure_item->value)
            {
                case MLS_ICG_NULLP:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "null?", 5)) != 0)
                    { goto fail; }
                    break;

                case MLS_ICG_REVERSE:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "reverse", 7)) != 0)
                    { goto fail; }
                    break;
            }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            switch (procedure_item->value)
            {
                case MLS_ICG_NULLP:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "null?", 5)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_REVERSE:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                                    &id, \
                                    "reverse", 7)) != 0)
                    { goto fail; }
                    break;
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_LIST_TAIL:

            /* Push start */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }
            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                            context->customizable_built_in_procedure_list, \
                            "list-tail", 9)) != 0)
            { goto fail; }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 2)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                            &id, \
                            "list-tail", 9)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_LIST_REF:

            /* Push index */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }
            /* Push list */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_REFGET, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_MEMQ:
        case MLS_ICG_MEMV:
        case MLS_ICG_MEMBER:

        case MLS_ICG_ASSQ:
        case MLS_ICG_ASSV:
        case MLS_ICG_ASSOC:

        case MLS_ICG_LIST_TO_STRING:
        case MLS_ICG_STRING_TO_LIST:

            MULTIPLE_ERROR_NOT_IMPLEMENTED();
            ret = -MULTIPLE_ERR_ICODEGEN; 
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

