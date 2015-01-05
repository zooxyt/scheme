/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures : char
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
#include "mls_icg_proc_std_string.h"

int mls_icodegen_node_proc_std_string(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc,
        struct mls_ast_node *ast_node_args,
        const int procedure_idx)
{
    int ret = 0;
    const struct mls_icg_proc_lut_item *procedure_item;
    /*uint32_t id;*/
    uint32_t type_id;
    /*int list_node_count;*/

    /*list_node_count = mls_icg_count_list_node(ast_node_args);*/

    (void)ast_node_proc;

    procedure_item = mls_icg_proc_lut_items_std + procedure_idx;

    switch (procedure_item->value)
    {
        case MLS_ICG_STRINGP:

            if ((ret = mls_icodegen_node(err, \
                            context, \
                            icg_fcb_block,  \
                            mls_icg_car(ast_node_args))) != 0)
            { goto fail; }

            ret = virtual_machine_object_type_name_to_id(&type_id, "str", 3);
            if (ret != 0) 
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "\'str\' isn't a valid type name");
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_TYPEP, type_id)) != 0) { goto fail; }

            break;

        case MLS_ICG_MAKE_STRING:
        case MLS_ICG_STRING:
        case MLS_ICG_STRING_LENGTH:
            MULTIPLE_ERROR_NOT_IMPLEMENTED();
            ret = -MULTIPLE_ERR_ICODEGEN; 
            break;
        case MLS_ICG_STRING_REF:
        case MLS_ICG_STRING_SET:
        case MLS_ICG_STRING_EQP:
        case MLS_ICG_STRING_CI_EQP:
        case MLS_ICG_STRING_LP:
        case MLS_ICG_STRING_CI_LP:
        case MLS_ICG_STRING_LEP:
        case MLS_ICG_STRING_CI_LEP:
        case MLS_ICG_STRING_GP:
        case MLS_ICG_STRING_CI_GP:
        case MLS_ICG_STRING_GEP:
        case MLS_ICG_STRING_CI_GEP:
        case MLS_ICG_SUBSTRING:

            MULTIPLE_ERROR_NOT_IMPLEMENTED();
            ret = -MULTIPLE_ERR_ICODEGEN; 
            break;

        case MLS_ICG_STRING_APPEND:
            MULTIPLE_ERROR_NOT_IMPLEMENTED();
            ret = -MULTIPLE_ERR_ICODEGEN; 
            break;

        case MLS_ICG_STRING_COPY:
        case MLS_ICG_STRING_FILL:

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

