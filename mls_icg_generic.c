/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Generic
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
#include "multiple_misc.h"

#include "multiply_str_aux.h"

#include "vm_opcode.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"
#include "mls_icg_generic.h"

/* Forms */
#include "mls_icg_form_define.h"

#include "mls_icg_form_lambda.h"
#include "mls_icg_form_do.h"
#include "mls_icg_form_let.h"
#include "mls_icg_form_leta.h"
#include "mls_icg_form_letrec.h"
#include "mls_icg_form_if.h"
#include "mls_icg_form_cond.h"
#include "mls_icg_form_case.h"
#include "mls_icg_form_and.h"
#include "mls_icg_form_or.h"
#include "mls_icg_form_begin.h"
#include "mls_icg_form_set.h"
#include "mls_icg_form_quote.h"
#include "mls_icg_form_module.h"
#include "mls_icg_form_import.h"
#include "mls_icg_form_delay.h"

#include "mls_icg_form_macro.h"
#include "mls_icg_form_scos.h"

/* Procedures */
#include "mls_icg_proc_lut.h"
#include "mls_icg_proc_display.h"
#include "mls_icg_proc_newline.h"
#include "mls_icg_proc_std.h"
#include "mls_icg_proc_stdnum.h"
#include "mls_icg_proc_special.h"
#include "mls_icg_proc_invoke.h"


static int mls_icodegen_node_atom(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node_atom *ast_node_atom)
{
    int ret = 0;
    uint32_t id;
    char *buffer_str = NULL;
    size_t buffer_str_len;
    int value_int;
    double value_float;

    (void)err;

    switch (ast_node_atom->atom->value)
    {
        case TOKEN_IDENTIFIER:
            if ((ret = multiply_resource_get_id(err, context->icode, context->res_id, \
                            &id, \
                            ast_node_atom->atom->str, 
                            ast_node_atom->atom->len)) != 0) 
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

            break;

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
                        "%d:%d: \'%s\' is an invalid float", \
                        ast_node_atom->atom->pos_ln, \
                        ast_node_atom->atom->pos_col, \
                        ast_node_atom->atom->str);
                ret = -MULTIPLE_ERR_ICODEGEN; 
                goto fail; 
            }
            if ((ret = multiply_resource_get_float(err, context->icode, context->res_id, &id, \
                            value_float)) != 0) 
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            break;

        case TOKEN_CONSTANT_FALSE:
            if ((ret = multiply_resource_get_false(err, context->icode, context->res_id, &id)) != 0) 
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            break;

        case TOKEN_CONSTANT_TRUE:
            if ((ret = multiply_resource_get_true(err, context->icode, context->res_id, &id)) != 0) 
            { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
            break;

        case TOKEN_CONSTANT_CHARACTER:
            if ((ret = multiply_resource_get_str(err, context->icode, context->res_id, \
                            &id, \
                            ast_node_atom->atom->str, 
                            ast_node_atom->atom->len)) != 0) 
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

    goto done;
fail:
done:
    if (buffer_str != NULL) free(buffer_str);
    return ret;
}

struct mls_icodegen_node_cons_branch_item
{
    const char *lead;
    size_t lead_len;
    int (*func)(struct multiple_error *err, \
            struct mls_icg_context *context, \
            struct mls_icg_fcb_block *icg_fcb_block, \
            struct mls_ast_node *ast_node);
};
static struct mls_icodegen_node_cons_branch_item mls_icodegen_node_cons_branch_items[] = 
{
    {"define", 6, mls_icodegen_node_form_define}, 
    {"let", 3, mls_icodegen_node_form_let}, 
    {"let*", 4, mls_icodegen_node_form_leta}, 
    {"letrec", 6, mls_icodegen_node_form_letrec}, 
    {"do", 2, mls_icodegen_node_form_do}, 
    {"begin", 5, mls_icodegen_node_form_begin}, 
    {"if", 2, mls_icodegen_node_form_if}, 
    {"cond", 4, mls_icodegen_node_form_cond}, 
    {"case", 4, mls_icodegen_node_form_case}, 
    {"and", 3, mls_icodegen_node_form_and}, 
    {"or", 2, mls_icodegen_node_form_or}, 
    {"set!", 4, mls_icodegen_node_form_set}, 
    {"quote", 5, mls_icodegen_node_form_quote}, 
    {"lambda", 6, mls_icodegen_node_form_lambda}, 
    {"delay", 5, mls_icodegen_node_form_delay}, 
    {"force", 5, mls_icodegen_node_form_force}, 
    {"define-syntax", 13, mls_icodegen_node_form_define_syntax}, 
    {"defclass", 8, mls_icodegen_node_form_defclass}, 
    {"invoke-method", 13, mls_icodegen_node_form_invoke_method}, 
    {"get-property", 12, mls_icodegen_node_form_get_property}, 
    {"set-property", 12, mls_icodegen_node_form_set_property}, 
    {"module", 6, mls_icodegen_node_form_module}, 
    {"import", 6, mls_icodegen_node_form_import}, 
    {"display", 7, mls_icodegen_node_proc_display}, 
    {"newline", 7, mls_icodegen_node_proc_newline}, 
};
#define MLS_ICODEGEN_NODE_CONS_BRANCH_ITEMS_COUNT (sizeof(mls_icodegen_node_cons_branch_items)/sizeof(struct mls_icodegen_node_cons_branch_item))

static int mls_icodegen_node_cons(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node_cons *ast_node_cons)
{
    int ret = 0;
    int procedure_idx;
    struct mls_ast_node *ast_node_first = ast_node_cons->car;
    struct mls_ast_node *ast_node_rest = ast_node_cons->cdr;
    struct mls_ast_node_atom *ast_node_atom;

    int matched = 0;
    int idx = 0;

    if (mls_icg_is_node_atom_id(ast_node_first) != 0)
    {
        ast_node_atom = ast_node_first->ptr;
        context->ast_node_parent = ast_node_first;
        for (idx = 0; idx != MLS_ICODEGEN_NODE_CONS_BRANCH_ITEMS_COUNT; idx++)
        {
            if ((mls_icodegen_node_cons_branch_items[idx].lead_len == ast_node_atom->atom->len) && \
                    (strncmp(mls_icodegen_node_cons_branch_items[idx].lead, \
                             ast_node_atom->atom->str, \
                             ast_node_atom->atom->len) == 0))
            {
                matched = 1;

                /* Clear Tail Attribute */
                mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);

                if ((ret = mls_icodegen_node_cons_branch_items[idx].func(err, context, icg_fcb_block, \
                                ast_node_rest)) != 0) { goto fail; }
                break;
            }
        }
    }

    if (matched == 0)
    {

        context->ast_node_parent = ast_node_cons->car;

        /* (standard procedures) */
        if ((ret = mls_icodegen_node_proc_detect(err, \
                        &procedure_idx, \
                        ast_node_first,\
                        mls_icg_proc_lut_items_std, MLS_PROC_LUT_ITEMS_STD_COUNT, \
                        ast_node_rest)) != 0)
        { goto fail; }

        if (procedure_idx != MLS_ICG_UNKNOWN)
        {
            /* Clear Tail Attribute */
            mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);
            if ((ret = mls_icodegen_node_proc_std(err, context, icg_fcb_block, \
                            ast_node_first, ast_node_rest, \
                            procedure_idx)) != 0) { goto fail; }
            goto finish;
        }

        /* (standard numeric procedures) */
        if ((ret = mls_icodegen_node_proc_detect(err, \
                        &procedure_idx, \
                        ast_node_first,\
                        mls_icg_proc_lut_items_stdnum, MLS_PROC_LUT_ITEMS_STDNUM_COUNT, \
                        ast_node_rest)) != 0)
        { goto fail; }

        if (procedure_idx != MLS_ICG_UNKNOWN)
        {
            /* Clear Tail Attribute */
            mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);

            if ((ret = mls_icodegen_node_proc_stdnum(err, context, icg_fcb_block, \
                            ast_node_first, ast_node_rest, \
                            procedure_idx)) != 0) { goto fail; }
            goto finish;
        }

        /* (special procedures) */
        if ((ret = mls_icodegen_node_proc_detect(err, \
                        &procedure_idx, \
                        ast_node_first,\
                        mls_icg_proc_lut_items_special, MLS_PROC_LUT_ITEMS_SPECIAL_COUNT, \
                        ast_node_rest)) != 0)
        { goto fail; }

        if (procedure_idx != MLS_ICG_UNKNOWN)
        {
            /* Clear Tail Attribute */
            mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);

            if ((ret = mls_icodegen_node_proc_special(err, context, icg_fcb_block, \
                            ast_node_first, ast_node_rest, \
                            procedure_idx)) != 0) { goto fail; }
            goto finish;
        }

        /* procedure invoke */
        if (mls_icg_context_test(context, MLS_ICG_CONTEXT_TAIL))
        {
            /* Tail call */
            mls_icg_context_set(context, MLS_ICG_CONTEXT_TAILCALL);
        }
        /* Clear Tail Attribute */
        mls_icg_context_clear(context, MLS_ICG_CONTEXT_TAIL);

        if ((ret = mls_icodegen_node_proc_invoke(err, context, icg_fcb_block, \
                        ast_node_first, ast_node_rest)) != 0) { goto fail; }

    }

finish:;
       goto done;
fail:
done:
       return ret;
}

int mls_icodegen_node(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *node)
{
    int ret = 0;
    struct mls_ast_node_atom *ast_node_atom;
    struct mls_ast_node_cons *ast_node_cons;

    switch (node->type)
    {
        case MLS_AST_NODE_ATOM:
            ast_node_atom = (struct mls_ast_node_atom *)(node->ptr);

            if ((ret = mls_icodegen_node_atom(err, context, icg_fcb_block, \
                            ast_node_atom)) != 0)
            { goto fail; }

            break;
        case MLS_AST_NODE_CONS:
            ast_node_cons = (struct mls_ast_node_cons *)(node->ptr);
            if ((ret = mls_icodegen_node_cons(err, context, icg_fcb_block, \
                            ast_node_cons)) != 0)
            { goto fail; }

            if (ret != 0) goto fail;
            break;
        default:
            MULTIPLE_ERROR_INTERNAL();
            ret = -MULTIPLE_ERR_INTERNAL;
            goto fail;
            break;
    }
    ret = 0;
    goto done;
fail:
done:
    return ret;
}

