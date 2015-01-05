/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form: module
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
#include "mls_icg_form_module.h"

int mls_icodegen_node_form_module(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    struct mls_ast_node_atom *ast_node_name_atom;
    uint32_t id_null;

    (void)icg_fcb_block;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count < 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: module: bad syntax, module name expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    else if (list_node_count > 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: module: bad syntax, only one module name expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node)) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: module: module name should be identifier", \
                mls_ast_node_ln(ast_node), mls_ast_node_col(ast_node));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_name_atom = mls_icg_car(ast_node)->ptr;
    if (mls_icg_context_update_module_name(context, \
            ast_node_name_atom->atom->str, ast_node_name_atom->atom->len) != 0)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_INTERNAL;
        goto fail;
    }

    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}


