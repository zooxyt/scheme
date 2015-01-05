/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form: Macro
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
#include "mls_icg_form_macro.h"

/* (syntax-rules */
/*
static int mls_icodegen_node_form_syntax_rules(struct multiple_error *err, \
        struct mls_icg_fcb_block_list *icg_fcb_block_list, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct icode *icode, \
        struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list, \
        struct mls_ast_node *ast_node,
        const int indent, const int verbose)
{
    int ret = 0;
    int list_node_count;
    struct mls_ast_node *ast_node_keyword_list, *ast_node_rules;

    (void)icg_fcb_block_list;
    (void)icg_fcb_block;
    (void)icode;
    (void)customizable_built_in_procedure_list;

    if (verbose != 0)
    {
        mls_icg_print_indent(indent);
        printf("syntax-rules ");
    }

    list_node_count = mls_icg_count_list_node(ast_node);

    if (list_node_count < 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "error: syntax-rules: bad syntax");
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node)) != 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "error: syntax-rules: bad syntax, keyword list should not be atom");
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    ast_node_keyword_list = mls_icg_car(ast_node);
    ast_node_rules = mls_icg_car(mls_icg_cdr(ast_node));

    (void)ast_node_keyword_list;
    (void)ast_node_rules;

    MULTIPLE_ERROR_NOT_IMPLEMENTED();
    ret = -MULTIPLE_ERR_ICODEGEN;
    goto fail;

    goto done;
fail:
done:
    return ret;
}
*/


/* (define-syntax <keyword> <transformer spec>) */
int mls_icodegen_node_form_define_syntax(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    struct mls_ast_node *ast_node_keyword, *ast_node_macro_transformer;

    (void)context;
    (void)icg_fcb_block;

    list_node_count = mls_icg_count_list_node(ast_node);

    if (list_node_count != 2)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "error: define-syntax: bad syntax");
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node)) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "error: define-syntax: bad syntax");
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    ast_node_keyword = mls_icg_car(ast_node);
    ast_node_macro_transformer = mls_icg_car(mls_icg_cdr(ast_node));
    (void)ast_node_keyword;

    if (mls_icg_is_node_atom_id_with_name(mls_icg_car(ast_node_macro_transformer), "syntax-rules") != 0)
    {
    }
    else
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "error: macro-transformer: only 'syntax-rules' form is allowed");
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    goto done;
fail:
done:
    return ret;
}

