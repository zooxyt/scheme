/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : delay & force
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
#include "vm_types.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_delay.h"

/* (delay ?) */
int mls_icodegen_node_form_delay(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

    /* (delay <exp)
     * =>
     * (lambda () <exp>) */

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

    /* Expression */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    new_icg_fcb_block, \
                    mls_icg_car(ast_node))) != 0)
    { goto fail; }

    /* Return */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }

    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_PROMMK, (uint32_t)(context->icg_fcb_block_list->size), MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0) 
    { goto fail; }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }

    /* Append blank export section */
    if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }

    goto done;
fail:
    if (new_icg_fcb_block != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}

/* (force ?) */
int mls_icodegen_node_form_force(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    uint32_t id_zero;
    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

    /* Zero */
    if ((ret = multiply_resource_get_int( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_zero, 0)) != 0)
    { goto fail; }

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

    /* Translate to 
     * (define (force delayed-object)
     *   (delayed-object))
     * =
     * (define force
     *   (lambda ()
     *     (delayed-object))) */

    /* Step 1: (delayed-object) */

    /* Save a copy of promise */
    mls_icg_context_set(context, MLS_ICG_CONTEXT_DUPFUNC);
    /* Expression */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    new_icg_fcb_block, \
                    ast_node)) != 0)
    { goto fail; }
    /* Cache the result to promise */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PROMC, 0)) != 0) { goto fail; }
    /* Return */
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }


    /* Step 2: (lambda ()
     *           (delayed-object)) */
    /* Arguments Count */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_zero)) != 0) { goto fail; }
    /* Make Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, \
                    OP_LAMBDAMK, (uint32_t)(context->icg_fcb_block_list->size), MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0) 
    { goto fail; }

    /* Step 3: ((lambda ()
     *            (delayed-object))) */
    /* Call Lambda */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALLC, 0)) != 0) { goto fail; }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }

    /* Append blank export section */
    if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }

    goto done;
fail:
    if (new_icg_fcb_block != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}

