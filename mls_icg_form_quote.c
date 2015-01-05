/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : quote
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

#include "multiply_str_aux.h"

#include "vm_opcode.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_quote.h"

static int mls_icodegen_node_form_quote_generic(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    struct mls_ast_node *ast_node_cur, *ast_node_cur_in;
    struct mls_ast_node_atom *ast_node_atom;
    int list_node_count, idx, idx2;
    uint32_t id;
    char *buffer_str = NULL;
    size_t buffer_str_len;
    int value_int;
    double value_float;

    if (mls_icg_is_node_cons(ast_node) != 0)
    {
        /* List */

        list_node_count = mls_icg_count_list_node(ast_node);
        for (idx = list_node_count - 1; idx >= 0; idx--)
        {
            ast_node_cur = ast_node;
            idx2 = idx;
            while (idx2-- != 0) { ast_node_cur = mls_icg_cdr(ast_node_cur); }

            ast_node_cur_in = mls_icg_car(ast_node_cur);
            if ((ret = mls_icodegen_node_form_quote_generic(err, context, icg_fcb_block, ast_node_cur_in)) != 0)
            { goto fail; }
        }
        if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_LSTMK, (uint32_t)list_node_count)) != 0) { goto fail; }
    }
    else
    {
        /* Atom */

        if (mls_icg_is_node_atom_id(ast_node) != 0)
        {
            ast_node_atom = ast_node->ptr;
            if ((ret = multiply_resource_get_id( \
                            err, \
                            context->icode, \
                            context->res_id, \
                            &id, \
                            ast_node_atom->atom->str, \
                            ast_node_atom->atom->len)) != 0)
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_SYMMK, id)) != 0) { goto fail; }
        }
        else
        {
            ast_node_atom = ast_node->ptr;

            switch (ast_node_atom->atom->value)
            {
                case TOKEN_CONSTANT_STRING:
                    /* String */
                    buffer_str_len = ast_node_atom->atom->len;
                    buffer_str = (char *)malloc(sizeof(char) * (buffer_str_len + 1));
                    if (buffer_str == NULL)
                    {
                        MULTIPLE_ERROR_MALLOC();
                        ret = -MULTIPLE_ERR_MALLOC;
                        goto fail; 
                    }
                    memcpy(buffer_str, ast_node_atom->atom->str, ast_node_atom->atom->len);
                    buffer_str[buffer_str_len] = '\0';
                    multiply_replace_escape_chars(buffer_str, &buffer_str_len);

                    if ((ret = multiply_resource_get_str( \
                                    err, \
                                    context->icode, \
                                    context->res_id, \
                                    &id, \
                                    buffer_str, \
                                    buffer_str_len)) != 0)
                    { goto fail; }
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

                    free(buffer_str);
                    buffer_str = NULL;

                    break;

                case TOKEN_CONSTANT_INTEGER_BINARY:
                case TOKEN_CONSTANT_INTEGER_OCTAL:
                case TOKEN_CONSTANT_INTEGER_DECIMAL:
                case TOKEN_CONSTANT_INTEGER_HEXADECIMAL:

                    if (multiply_convert_str_to_int(&value_int, \
                                ast_node_atom->atom->str, \
                                ast_node_atom->atom->len) != 0)
                    {
                        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                                "\'%s\' is an invalid integer", \
                                ast_node_atom->atom->str);
                        ret = -MULTIPLE_ERR_ICODEGEN; 
                        goto fail; 
                    }
                    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, \
                                    value_int)) != 0) 
                    { goto fail; }
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

                    break;

                case TOKEN_CONSTANT_FLOAT_BINARY:
                case TOKEN_CONSTANT_FLOAT_OCTAL:
                case TOKEN_CONSTANT_FLOAT_DECIMAL:
                case TOKEN_CONSTANT_FLOAT_HEXADECIMAL:

                    if (multiply_convert_str_to_float(&value_float, \
                                ast_node_atom->atom->str, \
                                ast_node_atom->atom->len) != 0)
                    {
                        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                                "\'%s\' is an invalid float", \
                                ast_node_atom->atom->str);
                        ret = -MULTIPLE_ERR_ICODEGEN; 
                        goto fail; 
                    }
                    if ((ret = multiply_resource_get_float(err, context->icode, context->res_id, &id, \
                                    value_float)) != 0) 
                    { goto fail; }
                    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

                    break;

                default:

                    multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                            "\'%s\' is an invalid atom", \
                            ast_node_atom->atom->str);
                    ret = -MULTIPLE_ERR_ICODEGEN; 
                    goto fail;
            }

            /*if ((ret = icodegen_resource_id_get_raw(err, &id, NULL, context->icode->id, context->icode->data_section, \*/
            /*ast_node_atom->atom->value, ast_node_atom->atom->str, ast_node_atom->atom->len, \*/
            /*1, context->icode)) != 0)*/
            /*{ goto fail; }*/
            /*if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }*/
        }
    }
fail:
    return ret;
}

int mls_icodegen_node_form_quote(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count != 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: quote: bad syntax", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if ((ret = mls_icodegen_node_form_quote_generic(err, context, icg_fcb_block, mls_icg_car(ast_node))) != 0)
    { goto fail; }

    goto done;
fail:
done:
    return ret;
}

