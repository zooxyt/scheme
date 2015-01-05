/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures : vector
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
#include "mls_icg_proc_std_vector.h"


static int mls_icodegen_node_proc_std_vectors_vector(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;
    int idx, idx2;
    int ast_node_args_count;
    struct mls_ast_node *ast_node_args_sub;
    struct mls_ast_node *ast_node_args_sub_in;

    ast_node_args_count = mls_icg_count_list_node(ast_node_args);

    for (idx = ast_node_args_count - 1; idx >= 0; idx--)
    {
        idx2 = idx;
        ast_node_args_sub = ast_node_args;
        while (idx2-- != 0) { ast_node_args_sub = mls_icg_cdr(ast_node_args_sub); }
        ast_node_args_sub_in = mls_icg_car(ast_node_args_sub);

        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block,  \
                        ast_node_args_sub_in)) != 0)
        { goto fail; }
    }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_ARRMK, (uint32_t)ast_node_args_count)) != 0) { goto fail; }

fail:
    return ret;
}

int mls_icodegen_node_proc_std_vectors(struct multiple_error *err, \
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

    (void)ast_node_proc;

    ast_node_args_count = mls_icg_count_list_node(ast_node_args);
    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->value)
    {
        case MLS_ICG_MAKE_VECTOR:

            /* (make-vector number [element]) */

            /* push element ; Push arguments reversely
             * push number
             * lambdamk pc1
             * funcmk
             * call
             *
             */

            /* Element */
            if (ast_node_args_count == 2)
            {
                /* element specified */
                if ((ret = mls_icodegen_node(err, \
                                context, \
                                icg_fcb_block,  \
                                mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
                { goto fail; }
            }
            else if (ast_node_args_count == 1)
            {
                if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 0)) != 0)
                { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            }
            else
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error:make-vector: expect 1 or 2 arguments", \
                        mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }

            /* Number */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                            context->customizable_built_in_procedure_list, \
                            "make-vector", 11)) != 0)
            { goto fail; }

            /* Call make-vector */
            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, &id, \
                            "make-vector", 11)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_VECTOR:

            if ((ret = mls_icodegen_node_proc_std_vectors_vector(err, \
                            context, \
                            icg_fcb_block, \
                            ast_node_args)) != 0)
            { goto fail; }

            break;

        case MLS_ICG_VECTORP:

            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            ret = virtual_machine_object_type_name_to_id(&type_id, "array", 5);
            if (ret != 0) 
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'array\' isn't a valid type name");
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEP, type_id)) != 0) { goto fail; }

            break;

        case MLS_ICG_VECTOR_LENGTH:

            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SIZE, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_VECTOR_REF: 

            /* Push index */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }
            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_REFGET, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_VECTOR_SET:

            /* Push value */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(mls_icg_cdr(ast_node_args))))) != 0)
            { goto fail; }
            /* Push index */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }
            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_REFSET, 0)) != 0) { goto fail; }

            break;

        case MLS_ICG_VECTOR_VECTOR_TO_LIST:
        case MLS_ICG_VECTOR_LIST_TO_VECTOR:

            /* Push vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            switch (procedure_item->value)
            {
                case MLS_ICG_VECTOR_VECTOR_TO_LIST:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "vector->list!", 12)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_VECTOR_LIST_TO_VECTOR:
                    if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                                    context->customizable_built_in_procedure_list, \
                                    "list->vector!", 12)) != 0)
                    { goto fail; }
                    break;
            }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 1)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            switch (procedure_item->value)
            {
                case MLS_ICG_VECTOR_VECTOR_TO_LIST:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, &id, \
                                    "vector->list", 12)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_VECTOR_LIST_TO_VECTOR:
                    if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, &id, \
                                    "list->vector", 12)) != 0)
                    { goto fail; }
                    break;
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SLV, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

            break;


        case MLS_ICG_VECTOR_VECTOR_FILL:

            /* Element */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(mls_icg_cdr(ast_node_args)))) != 0)
            { goto fail; }

            /* Vector */
            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            if ((ret = mls_icg_customizable_built_in_procedure_list_called( \
                            context->customizable_built_in_procedure_list, \
                            "vector-fill!", 12)) != 0)
            { goto fail; }

            /* Arguments Count */
            if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, 2)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            /* Proc Name */
            if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, &id, \
                            "vector-fill!", 12)) != 0)
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

