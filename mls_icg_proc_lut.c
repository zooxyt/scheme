/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Procedures Lookup Table
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
#include "mls_icg_proc_stdnum.h"

#include "mls_icg_proc_stdnum_arith.h"
#include "mls_icg_proc_stdnum_inque.h"

int mls_icodegen_node_proc_detect(struct multiple_error *err, \
        int *idx_out, \
        const struct mls_ast_node *ast_node_proc, \
        const struct mls_icg_proc_lut_item *items, const size_t count, \
        struct mls_ast_node *ast_node_args)
{
    int ret = 0;
    size_t idx;
    struct mls_ast_node_atom *ast_node_proc_atom;
    const struct mls_icg_proc_lut_item *procedure_item;
    int ast_node_args_count;

    if (!(mls_icg_is_node_atom_id(ast_node_proc)))
    {
        *idx_out = MLS_ICG_UNKNOWN;
        return 0;
    }
    ast_node_proc_atom = ast_node_proc->ptr;

    for (idx = 0; idx != count; idx++)
    {
        if ((items[idx].len == ast_node_proc_atom->atom->len) && \
                (strncmp(items[idx].str, \
                         ast_node_proc_atom->atom->str, \
                         items[idx].len) == 0))
        {
            /* Checking arguments number */
            ast_node_args_count = mls_icg_count_list_node(ast_node_args);
            procedure_item = items + idx;
            if (ast_node_args_count < procedure_item->args_min)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: procedure \'%s\' required at least %d arguments", \
                        mls_ast_node_ln(ast_node_args), mls_ast_node_col(ast_node_args), \
                        procedure_item->str, \
                        procedure_item->args_min);
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
            if ((procedure_item->args_max != -1) && (procedure_item->args_max < ast_node_args_count))
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: procedure \'%s\' required at most %d arguments", \
                        mls_ast_node_ln(ast_node_args), mls_ast_node_col(ast_node_args), \
                        procedure_item->str, \
                        procedure_item->args_max);
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
            if (procedure_item->op == MLS_ICG_PROC_OP_UNHANDLED)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: unhandled procedure \'%s\'", \
                        mls_ast_node_ln(ast_node_args), mls_ast_node_col(ast_node_args), \
                        procedure_item->str);
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
            *idx_out = items[idx].value; 
            return 0;
        }
    }

fail:
    *idx_out = MLS_ICG_UNKNOWN;
    return ret;
}

