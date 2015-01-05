/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : if
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
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_define.h"
#include "mls_icg_body.h"
#include "mls_icg_form_if.h"

int mls_icodegen_node_form_if(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_ast_node *ast_node_test = NULL;
    struct mls_ast_node *ast_node_proc_true = NULL;
    struct mls_ast_node *ast_node_proc_false = NULL;
    int list_node_count;
    uint32_t instrument_number_jmpc = 0;
    uint32_t instrument_number_jmp = 0;
    uint32_t instrument_number_begin_of_false = 0;
    uint32_t instrument_number_end = 0;
    uint32_t id_null;

    /* elements count */
    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count < 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    if (list_node_count == 2)
    {
        /* if then style */
        ast_node_test = mls_icg_car(ast_node);
        ast_node_proc_true = mls_icg_cdar(ast_node);
    }
    else if (list_node_count == 3)
    {
        /* if then else style */
        ast_node_test = mls_icg_car(ast_node);
        ast_node_proc_true = mls_icg_cdar(ast_node);
        ast_node_proc_false = mls_icg_cddar(ast_node);
    }
    else
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: if: bad syntax, 2 or 3 branches required for \'if\' form", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Test Condition */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block,  \
                    ast_node_test)) != 0)
    { goto fail; }

    /* Reverse the test result */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_NOTL, 0)) != 0) { goto fail; }

    /* If false, jump to else block or exit point*/
    instrument_number_jmpc = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMPC, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }

    /* then block */
    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_proc_true)) != 0)
    { goto fail; }


    /* Record the end of then */
    instrument_number_jmp = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);
    if ((ret = mls_icg_fcb_block_append_with_configure_type(icg_fcb_block, OP_JMP, 0, MLS_ICG_FCB_LINE_TYPE_PC)) != 0) { goto fail; }

    instrument_number_begin_of_false = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

    /* else block */
    if (ast_node_proc_false)
    {
        if ((ret = mls_icodegen_node(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_proc_false)) != 0)
        { goto fail; }
    }
    else
    {

        if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
        { goto fail; }

        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }
    }

    /* end */
    instrument_number_end = mls_icg_fcb_block_get_instrument_number(icg_fcb_block);

    mls_icg_fcb_block_link(icg_fcb_block, instrument_number_jmpc, instrument_number_begin_of_false);
    mls_icg_fcb_block_link(icg_fcb_block, instrument_number_jmp, instrument_number_end);

    goto done;
fail:
done:
    return ret;
}

