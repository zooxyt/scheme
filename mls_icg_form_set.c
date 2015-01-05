/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : set!
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

#include "vm_opcode.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_aux.h"
#include "mls_icg_context.h"
#include "mls_icg_generic.h"

#include "mls_icg_form_set.h"

int mls_icodegen_node_form_set(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_ast_node *ast_node_left_value = NULL;
    struct mls_ast_node *ast_node_right_value = NULL;
    struct mls_ast_node_atom *ast_node_name_atom;
    uint32_t id;
    int list_node_count;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count < 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    if (list_node_count != 2)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: set!: bad syntax, 2 branches required for \'set!\' form", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    ast_node_left_value = mls_icg_car(ast_node);
    ast_node_right_value = mls_icg_cdar(ast_node);

    if (mls_icg_is_node_atom_id(ast_node_left_value) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: set!: identifier expected", \
                mls_ast_node_ln(ast_node_left_value), mls_ast_node_col(ast_node_left_value));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_name_atom = ast_node_left_value->ptr;

    /* Expression */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block,  \
                    ast_node_right_value)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }

    /* Save in variable */
    if ((ret = multiply_resource_get_id(err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_name_atom->atom->str, \
                    ast_node_name_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPCX, id)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

