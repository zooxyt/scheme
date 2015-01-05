/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : import
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
#include "mls_icg_form_define.h"
#include "mls_icg_body.h"
#include "mls_icg_form_import.h"

static int mls_icodegen_node_form_import_add_only_lib(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_ast_node *ast_node_library_name)
{
    int ret = 0;
    struct multiple_ir_import_section_item *new_import_section_item = NULL;
    uint32_t id_library;
    struct mls_ast_node_atom *ast_node_library_name_atom;

    ast_node_library_name_atom = ast_node_library_name->ptr; 

    /* Domain */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_library, \
                    ast_node_library_name_atom->atom->str, \
                    ast_node_library_name_atom->atom->len)) != 0)
    { goto fail; }


    /* Create new import section item */
    if ((new_import_section_item = multiple_ir_import_section_item_new()) == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }
    new_import_section_item->name = id_library;
    /* Append import section */
    if ((ret = multiple_ir_import_section_append_deduplicate(context->icode->import_section, &new_import_section_item)) != 0)
    { goto fail; }
    new_import_section_item = NULL;

fail:
    return ret;
}

static int mls_icodegen_node_form_import_add(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_library_name, \
        struct mls_ast_node *ast_node_procedure_name, \
        char *new_name, size_t new_name_len)
{
    int ret = 0;
    struct multiple_ir_import_section_item *new_import_section_item = NULL;
    uint32_t id_procedure, id_library, id_new_name;
    struct mls_ast_node_atom *ast_node_procedure_name_atom;
    struct mls_ast_node_atom *ast_node_library_name_atom;

    ast_node_library_name_atom = ast_node_library_name->ptr; 
    ast_node_procedure_name_atom = ast_node_procedure_name->ptr; 

    /* New Name */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_new_name, \
                    new_name, \
                    new_name_len)) != 0)
    { goto fail; }
    /* Procedure */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_procedure, \
                    ast_node_procedure_name_atom->atom->str, \
                    ast_node_procedure_name_atom->atom->len)) != 0)
    { goto fail; }
    /* Domain */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_library, \
                    ast_node_library_name_atom->atom->str, \
                    ast_node_library_name_atom->atom->len)) != 0)
    { goto fail; }

    /* Create new import section item */
    if ((new_import_section_item = multiple_ir_import_section_item_new()) == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
    new_import_section_item->name = id_library;
    /* Append import section */
    if ((ret = multiple_ir_import_section_append_deduplicate(context->icode->import_section, &new_import_section_item)) != 0)
    { goto fail; }
    new_import_section_item = NULL;

    /* Combine the function */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_procedure)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_library)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_DOMAIN, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPC, id_new_name)) != 0) { goto fail; }

fail:
    return ret;
}

static int mls_icodegen_node_form_import_add_with_node(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_library_name, \
        struct mls_ast_node *ast_node_procedure_name)
{
    struct mls_ast_node_atom *ast_node_procedure_name_atom = ast_node_procedure_name->ptr;
    char *name = ast_node_procedure_name_atom->atom->str;
    size_t name_len = ast_node_procedure_name_atom->atom->len;

    return mls_icodegen_node_form_import_add(err, context, icg_fcb_block, \
            ast_node_library_name, \
            ast_node_procedure_name, \
            name, name_len);
}

static int mls_icodegen_node_form_import_opt(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_library_name, \
        struct mls_ast_node *ast_node_opt)
{
    int ret = 0;
    struct mls_ast_node *ast_node_opt_name;
    struct mls_ast_node *ast_node_opt_arg1;
    struct mls_ast_node *ast_node_opt_arg2;
    int list_node_count;
    char *new_name = NULL;
    size_t new_name_len;

    list_node_count = mls_icg_count_list_node(ast_node_opt);

    if (list_node_count <= 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: import: bad syntax, options expected", \
                mls_ast_node_ln(ast_node_opt), \
                mls_ast_node_col(ast_node_opt));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node_opt)) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: import: bad syntax, library name must be identifier", \
                mls_ast_node_ln(mls_icg_car(ast_node_opt)), \
                mls_ast_node_col(mls_icg_car(ast_node_opt)));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_opt_name = mls_icg_car(ast_node_opt);

    if (mls_icg_is_node_atom_id_with_name(ast_node_opt_name, "rename") != 0)
    {
        if (list_node_count < 3)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, " \
                    "original procedure name and alias name expected", \
                    mls_ast_node_ln(ast_node_opt_name), \
                    mls_ast_node_col(ast_node_opt_name));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        ast_node_opt_arg1 = mls_icg_car(mls_icg_cdr(ast_node_opt)); /* old name */
        ast_node_opt_arg2 = mls_icg_car(mls_icg_cdr(mls_icg_cdr(ast_node_opt))); /* new name */

        if (mls_icg_is_node_atom_id(ast_node_opt_arg1) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, " \
                    "original procedure name should be identifier", \
                    mls_ast_node_ln(mls_icg_cdr(ast_node_opt)), \
                    mls_ast_node_col(mls_icg_cdr(ast_node_opt)));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        if (mls_icg_is_node_atom_id(ast_node_opt_arg2) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, alias name should be identifier", \
                    mls_ast_node_ln(mls_icg_cddr(ast_node_opt)), \
                    mls_ast_node_col(mls_icg_cddr(ast_node_opt)));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        new_name_len = ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->len;
        new_name = (char *)malloc(sizeof(char) * (new_name_len + 1));
        if (new_name == NULL)
        {
            MULTIPLE_ERROR_MALLOC();
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        memcpy(new_name, ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->str, new_name_len);
        new_name[new_name_len] = '\0';

        if ((ret = mls_icodegen_node_form_import_add(err, context, icg_fcb_block, \
                        ast_node_library_name, \
                        ast_node_opt_arg1, \
                        new_name, new_name_len)) != 0)
        { goto fail; }
    }
    else if (mls_icg_is_node_atom_id_with_name(ast_node_opt_name, "prefix") != 0)
    {
        if (list_node_count < 3)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, " \
                    "original procedure name and prefix expected", \
                    mls_ast_node_ln(ast_node_opt_name), \
                    mls_ast_node_col(ast_node_opt_name));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        ast_node_opt_arg1 = mls_icg_cdar(ast_node_opt);
        ast_node_opt_arg2 = mls_icg_cddar(ast_node_opt);

        if (mls_icg_is_node_atom_id(ast_node_opt_arg1) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, " \
                    "original procedure name should be identifier", \
                    mls_ast_node_ln(mls_icg_cdr(ast_node_opt)), \
                    mls_ast_node_col(mls_icg_cdr(ast_node_opt)));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        if (mls_icg_is_node_atom_id(ast_node_opt_arg2) == 0)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, prefix should be identifier", \
                    mls_ast_node_ln(mls_icg_cddr(ast_node_opt)), \
                    mls_ast_node_col(mls_icg_cddr(ast_node_opt)));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        new_name_len = ((struct mls_ast_node_atom *)(ast_node_opt_arg1->ptr))->atom->len + \
                       ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->len;
        new_name = (char *)malloc(sizeof(char) * (new_name_len + 1));
        if (new_name == NULL)
        {
            MULTIPLE_ERROR_MALLOC();
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        memcpy(new_name, \
                ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->str, \
                ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->len);
        memcpy(new_name + ((struct mls_ast_node_atom *)(ast_node_opt_arg2->ptr))->atom->len, \
                ((struct mls_ast_node_atom *)(ast_node_opt_arg1->ptr))->atom->str, \
                ((struct mls_ast_node_atom *)(ast_node_opt_arg1->ptr))->atom->len);
        new_name[new_name_len] = '\0';

        if ((ret = mls_icodegen_node_form_import_add(err, context, icg_fcb_block, \
                        ast_node_library_name, \
                        ast_node_opt_arg1, \
                        new_name, new_name_len)) != 0)
        { goto fail; }
    }
    else
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: import: bad syntax, " \
                "invalid library option, only \'rename\' and \'prefix\' allowed", \
                mls_ast_node_ln(ast_node_opt_name), mls_ast_node_col(ast_node_opt_name));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    goto done;
fail:
done:
    if (new_name != NULL) free(new_name);
    return ret;
}

int mls_icodegen_node_form_import(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count, list_node_count_2;

    struct mls_ast_node *ast_node_branch_cur;
    struct mls_ast_node *ast_node_branch_cur_in;

    struct mls_ast_node *ast_node_library_name;
    struct mls_ast_node *ast_node_procedure_name_list;
    struct mls_ast_node *ast_node_procedure_name;

    uint32_t id_null;

    list_node_count = mls_icg_count_list_node(ast_node);

    if (list_node_count != 1)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: import: bad syntax, must contains import spec", \
                    mls_ast_node_ln(context->ast_node_parent), \
                    mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_cons(ast_node) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                "%d:%d: error: import: bad syntax, must be a pair", \
                mls_ast_node_ln(ast_node), \
                mls_ast_node_col(ast_node));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    ast_node_branch_cur = ast_node;
    while (ast_node_branch_cur != NULL)
    {
        ast_node_branch_cur_in = mls_icg_car(ast_node_branch_cur);
        list_node_count = mls_icg_count_list_node(ast_node_branch_cur_in);
        if (list_node_count < 1)
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                    "%d:%d: error: import: bad syntax, must contains the library name and/or procedures", \
                    mls_ast_node_ln(ast_node_branch_cur), \
                    mls_ast_node_col(ast_node_branch_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }
        else if (list_node_count == 1)
        {
            if (mls_icg_is_node_atom_id(mls_icg_car(ast_node_branch_cur_in)) == 1)
            {
                if ((ret = mls_icodegen_node_form_import_add_only_lib(err, \
                                context, \
                                ast_node_branch_cur_in)) != 0)
                { goto fail; }
            }
            else
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                        "%d:%d: error: import: bad syntax, library name must be identifier", \
                        mls_ast_node_ln(ast_node_branch_cur_in), \
                        mls_ast_node_col(ast_node_branch_cur_in));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
        }
        else
        {
            if (mls_icg_is_node_atom_id(mls_icg_car(ast_node_branch_cur_in)) == 0)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                        "%d:%d: error: import: bad syntax, must contains the library name", \
                        mls_ast_node_ln(mls_icg_car(ast_node_branch_cur)), \
                        mls_ast_node_col(mls_icg_car(ast_node_branch_cur)));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }

            ast_node_library_name = mls_icg_car(ast_node_branch_cur_in);

            if (mls_icg_is_node_cons(ast_node_library_name) != 0)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                        "%d:%d: error: import: bad syntax, library name must be identifier", \
                        mls_ast_node_ln(ast_node_library_name), \
                        mls_ast_node_col(ast_node_library_name));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }

            list_node_count -= 1;
            ast_node_procedure_name_list = mls_icg_cdr(ast_node_branch_cur_in);
            while (list_node_count != 0)
            {
                if (mls_icg_is_node_cons(ast_node_procedure_name_list) != 0)
                {
                    list_node_count_2 = mls_icg_count_list_node(mls_icg_car(ast_node_procedure_name_list));
                    if (list_node_count_2 <= 0)
                    {
                        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                                "%d:%d: error: import: bad syntax, options expected", \
                                mls_ast_node_ln(ast_node_procedure_name_list), \
                                mls_ast_node_col(ast_node_procedure_name_list));
                        ret = -MULTIPLE_ERR_ICODEGEN;
                        goto fail;
                    }

                    if ((ret = mls_icodegen_node_form_import_opt(err, \
                                    context, \
                                    icg_fcb_block, \
                                    ast_node_library_name, \
                                    mls_icg_car(ast_node_procedure_name_list))) != 0)
                    { goto fail; }
                }
                else
                {
                    ast_node_procedure_name = mls_icg_car(ast_node_procedure_name_list);

                    if ((ret = mls_icodegen_node_form_import_add_with_node(err, \
                                    context, \
                                    icg_fcb_block, \
                                    ast_node_library_name, \
                                    ast_node_procedure_name)) != 0)
                    { goto fail; }
                }

                ast_node_procedure_name_list = mls_icg_cdr(ast_node_procedure_name_list);
                list_node_count -= 1;
            }
        }

        ast_node_branch_cur = mls_icg_cdr(ast_node_branch_cur);
    }

    if ((ret = multiply_resource_get_none( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_null)) != 0) 
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

