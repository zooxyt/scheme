/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form: Object System
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

#include "multiply_str_aux.h"

#include "vm_opcode.h"
#include "vm_predef.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"
#include "mls_icg_aux.h"

#include "mls_icg_generic.h"
#include "mls_icg_form_scos.h"
#include "mls_icg_body.h"
#include "mls_icg_proc_invoke.h"

/* single arg */
static int mls_icodegen_node_form_arg( \
        struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg, 
        uint32_t opcode)
{
    int ret = 0;
    struct mls_ast_node_atom *ast_node_atom;
    uint32_t id;

    if (mls_icg_is_node_atom_id(ast_node_arg) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: not an identifier", \
                mls_ast_node_ln(ast_node_arg), mls_ast_node_col(ast_node_arg));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }
    ast_node_atom = ast_node_arg->ptr;
    if ((ret = multiply_resource_get_id(err, \
                    icode, \
                    res_id, \
                    &id, \
                    ast_node_atom->atom->str, \
                    ast_node_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, opcode, id)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

/* args-list */ 
static int mls_icodegen_node_form_args_list(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args_list,
        uint32_t opcode)
{
    int ret = 0;
    struct mls_ast_node *ast_node_args_list_sub = NULL;

    ast_node_args_list_sub = ast_node_args_list;
    while (ast_node_args_list_sub != NULL)
    {
        /* Receive an argument */
        if ((ret = mls_icodegen_node_form_arg(err, \
                        context->icode, \
                        context->res_id, \
                        icg_fcb_block, \
                        mls_icg_car(ast_node_args_list_sub),
                        opcode)) != 0)
        { goto fail; }

        /* Next argument */
        ast_node_args_list_sub = mls_icg_cdr(ast_node_args_list_sub);
    }

    goto done;
fail:
done:
    return ret;
}

/* args-list reversely */ 
static int mls_icodegen_node_form_args_list_reverse(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args_list,
        uint32_t opcode)
{
    int ret = 0;
    struct mls_ast_node *ast_node_args_list_sub = NULL;
    int list_node_count, idx, idx2;

    list_node_count = mls_icg_count_list_node(ast_node_args_list);

    for (idx = 0; idx != list_node_count; idx++)
    {
        ast_node_args_list_sub = ast_node_args_list;
        for (idx2 = 0; idx2 != list_node_count; idx2++)
        {
            ast_node_args_list_sub = mls_icg_cdr(ast_node_args_list_sub);
        }
        if ((ret = mls_icodegen_node_form_arg(err, \
                        context->icode, \
                        context->res_id, \
                        icg_fcb_block, \
                        mls_icg_car(ast_node_args_list_sub),
                        opcode)) != 0)
        { goto fail; }
    }

    goto done;
fail:
done:
    return ret;
}

/* 
(defclass <class-name>
  <method-definition-1>
  <method-definition-2>
  ...
  <method-definition-n>)
*/


/* Translation Rules
 * ------------------------------
 * def __predefined__
 * push class-name
 * clstypereg class-name
 * push method-name
 * push class-name
 * push class-name::method-name
 * push module-name
 * domain
 * clsmadd
 *
 * def class-name::method-name
 * arg this
 * arg args
 * <body>
 */

enum
{
    MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_NORMAL = 0, 
    MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR = 1, 
    MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_DTOR = 2,
};
int mls_icodegen_node_form_defclass(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int list_node_count_branch;
    int idx;
    size_t idx_arg;
    int method_type;
    struct mls_ast_node *ast_node_class_name, *ast_node_class_methods;
    struct mls_ast_node *ast_node_class_method_cur;
    struct mls_ast_node *ast_node_method_type, *ast_node_method_signature, *ast_node_method_body;
    int list_node_count_method_signature = 0;
    struct mls_ast_node *ast_node_method_signature_name = 0;
    struct mls_ast_node *ast_node_method_signature_args = NULL, *ast_node_method_signature_args_bak = NULL;
    struct mls_ast_node *ast_node_method_signature_arg;
    struct mls_ast_node_atom *ast_node_name_atom, *ast_node_name_atom_prefix, *ast_node_name_atom_name;
    struct mls_ast_node_atom *ast_node_atom_method_signature_arg;
    uint32_t id_class_name, id_fullname, id_method_name, id_module_name, id_ctor = 0;
    char *str_fullname = NULL; size_t str_fullname_len = 0; 
    uint32_t id_null = 0, id_this = 0, id;

    struct mls_icg_fcb_block *new_icg_fcb_block = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;
    int ctor_enabled = 0;
    uint32_t instrument_number_creator = 0;
    char *method_name = NULL; size_t method_name_len = 0; 

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax, class name expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    if (mls_icg_is_node_atom_id(mls_icg_car(ast_node)) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax, class name should be identifier", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* none */
    if ((ret = multiply_resource_get_none(err, context->icode, context->res_id, &id_null)) != 0) 
    { goto fail; }
    /* 'this' */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_this, \
                    VM_PREDEF_CLASS_THIS, \
                    VM_PREDEF_CLASS_THIS_LEN)) != 0)
    { goto fail; }

    /* module-name */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_module_name, \
                    context->module_name, \
                    context->module_name_len)) != 0)
    { goto fail; }

    /*
     * push class-name
     * clstypereg 
     */

    ast_node_class_name = mls_icg_car(ast_node);
    ast_node_name_atom = ast_node_class_name->ptr;
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id_class_name, \
                    ast_node_name_atom->atom->str, \
                    ast_node_name_atom->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, OP_PUSH, id_class_name)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, OP_CLSTYPEREG, 0)) != 0) { goto fail; }

    /* Methods */
    ast_node_class_methods = mls_icg_cdr(ast_node);
    list_node_count = mls_icg_count_list_node(ast_node_class_methods);
    for (idx = 0; idx < list_node_count; idx++)
    {
        ast_node_class_method_cur = mls_icg_car(ast_node_class_methods);
        ast_node_class_methods = mls_icg_cdr(ast_node_class_methods);

        /* .text Section Item */
        new_icg_fcb_block = mls_icg_fcb_block_new();
        if (new_icg_fcb_block == NULL)
        { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

        /* .export Section Item */
        new_export_section_item = multiple_ir_export_section_item_new();
        if (new_export_section_item == NULL)
        { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
        new_export_section_item->instrument_number = (uint32_t)context->icode->export_section->size;
        new_export_section_item->blank = 0; 

        /*
         * push method-name
         * push class-name
         * push class-name::method-name
         * push module-name
         * domain
         * clsmadd
         */

        list_node_count_branch = mls_icg_count_list_node(ast_node_class_method_cur);
        
        /* Type */
        if (list_node_count_branch >= 1)
        {
            ast_node_method_type = mls_icg_car(ast_node_class_method_cur);
            if (mls_icg_is_node_atom_id_with_name(ast_node_method_type, "defmethod") != 0)
            {
                method_type = MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_NORMAL;
            }
            else if (mls_icg_is_node_atom_id_with_name(ast_node_method_type, "defctor") != 0)
            {
                method_type = MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR;
            }
            else if (mls_icg_is_node_atom_id_with_name(ast_node_method_type, "defdtor") != 0)
            {
                method_type = MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_DTOR;
            }
            else
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax, " \
                        "only method definition forms is allowed", \
                        mls_ast_node_ln(ast_node_method_type), mls_ast_node_col(ast_node_method_type));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }
        }
        else
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax, no method type in branch", \
                    mls_ast_node_ln(ast_node_class_method_cur), mls_ast_node_col(ast_node_class_method_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }

        /* Signature */
        if (list_node_count_branch >= 2)
        {
            ast_node_method_signature = mls_icg_car(mls_icg_cdr(ast_node_class_method_cur));
            if (mls_icg_is_node_atom_id(ast_node_method_signature) == 1)
            {
                multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax on method signature", \
                        mls_ast_node_ln(mls_icg_cdr(ast_node_class_method_cur)), mls_ast_node_col(mls_icg_cdr(ast_node_class_method_cur)));
                ret = -MULTIPLE_ERR_ICODEGEN;
                goto fail;
            }

            list_node_count_method_signature = mls_icg_count_list_node(ast_node_method_signature);
            switch (method_type)
            {
                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_NORMAL:
                    if (list_node_count_method_signature == 0)
                    {
                        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax on method signature, " \
                                "no method name", \
                                mls_ast_node_ln(ast_node_method_signature), mls_ast_node_col(ast_node_method_signature));
                        ret = -MULTIPLE_ERR_ICODEGEN;
                        goto fail;
                    }
                    list_node_count_method_signature -= 1;
                    /* Method Signature process */
                    ast_node_method_signature_name = mls_icg_car(ast_node_method_signature);
                    if (mls_icg_is_node_atom_id(ast_node_method_signature_name) == 0)
                    {
                        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                                "%d:%d: error: defclass: bad syntax on method signature, method name should be identifier", \
                                mls_ast_node_ln(ast_node_method_signature), mls_ast_node_col(ast_node_method_signature));
                        ret = -MULTIPLE_ERR_ICODEGEN;
                        goto fail;
                    }
                    ast_node_name_atom_name = ast_node_method_signature_name->ptr;
                    method_name = ast_node_name_atom_name->atom->str; 
                    method_name_len = ast_node_name_atom_name->atom->len; 
                    break;

                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR:
                    method_name = (char *)VM_PREDEF_CLASS_CONSTRUCTOR;
                    method_name_len = (size_t)VM_PREDEF_CLASS_CONSTRUCTOR_LEN;

                    break;

                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_DTOR:
                    method_name = (char *)VM_PREDEF_CLASS_DESTRUCTOR;
                    method_name_len = (size_t)VM_PREDEF_CLASS_DESTRUCTOR_LEN;

                    break;
            }

            /* Domain */
            ast_node_name_atom_prefix = ast_node_class_name->ptr;

            /* Method name */
            if ((ret = multiply_resource_get_id( \
                            err, \
                            context->icode, \
                            context->res_id, \
                            &id_method_name, \
                            method_name, \
                            method_name_len)) != 0)
            { goto fail; }

            /* Fullname */
            if ((ret = multiply_concat_prefix_and_name( \
                            &str_fullname, &str_fullname_len, \
                            ast_node_name_atom_prefix->atom->str, ast_node_name_atom_prefix->atom->len, 
                            method_name, method_name_len)) != 0)
            { goto fail; }
            if ((ret = multiply_resource_get_id( \
                            err, \
                            context->icode, \
                            context->res_id, \
                            &id_fullname, \
                            str_fullname, \
                            str_fullname_len)) != 0)
            { goto fail; }
            free(str_fullname); str_fullname = NULL;
            
            if (method_type == MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR)
            { id_ctor = id_fullname; }

            if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                            OP_PUSH, id_method_name)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                            OP_PUSH, id_class_name)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                            OP_PUSH, id_fullname)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                            OP_PUSH, id_module_name)) != 0) { goto fail; }
            if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                            OP_DOMAIN, 0)) != 0) { goto fail; }
            switch (method_type)
            {
                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_NORMAL:
                    if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                                    OP_CLSMADD, 0)) != 0) { goto fail; }
                    ast_node_method_signature_args = mls_icg_cdr(ast_node_method_signature);
                    break;
                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR:
                    if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                                    OP_CLSCTORADD, 0)) != 0) { goto fail; }
                    ast_node_method_signature_args = ast_node_method_signature;
                    ctor_enabled = 1;
                    break;
                case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_DTOR:
                    if ((ret = mls_icg_fcb_block_append_with_configure(context->icg_fcb_block_predefined, \
                                    OP_CLSDTORADD, 0)) != 0) { goto fail; }
                    ast_node_method_signature_args = ast_node_method_signature;
                    break;
            }
            ast_node_method_signature_args_bak = ast_node_method_signature_args;
        }
        else
        {
            multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: defclass: bad syntax, no method signature in branch", \
                    mls_ast_node_ln(ast_node_class_method_cur), mls_ast_node_col(ast_node_class_method_cur));
            ret = -MULTIPLE_ERR_ICODEGEN;
            goto fail;
        }

        /* Method Subroutine */
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_DEF, id_fullname)) != 0) { goto fail; }

        switch (method_type)
        {
            case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_NORMAL:
            case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_CTOR:
                /* 'this' */
                if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_ARGC, id_this)) != 0) { goto fail; }
                break;
        }

        /* Arguments */
        if ((ret = mls_icodegen_node_form_args_list(err, \
                context, \
                new_icg_fcb_block, \
                ast_node_method_signature_args, \
                OP_ARG)) != 0)
        { goto fail; }

        /* Body */
        ast_node_method_body = mls_icg_cdr(mls_icg_cdr(ast_node_class_method_cur));
        if ((ret = mls_icodegen_node_body(err, \
                        context, \
                        new_icg_fcb_block, \
                        ast_node_method_body)) != 0)
        { goto fail; }
        /* Pop original key */
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_null)) != 0) { goto fail; }

        switch (method_type)
        {
            case MLS_ICODEGEN_NODE_FORM_DEFCLASS_METHOD_TYPE_DTOR:
                if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_this)) != 0) { goto fail; }
                if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_CLSDTOR, 0)) != 0) { goto fail; }
                break;
        }

        /* Push none */
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }

        /* Arguments + 'this' */
        new_export_section_item->args_count = (size_t)list_node_count_method_signature + 1;
        if (new_export_section_item->args_count > 0)
        {
            new_export_section_item->args = (uint32_t *)malloc(sizeof(uint32_t) * new_export_section_item->args_count);
            if (new_export_section_item->args == NULL) { goto fail; }
            new_export_section_item->args_types = (uint32_t *)malloc(sizeof(uint32_t) * new_export_section_item->args_count);
            if (new_export_section_item->args_types == NULL) { goto fail; }
            new_export_section_item->args[0] = id_this;
            new_export_section_item->args_types[0] = MULTIPLE_IR_EXPORT_SECTION_ITEM_ARGS_NORMAL;
            for (idx_arg = 1; idx_arg != new_export_section_item->args_count; idx_arg++)
            {
                ast_node_method_signature_arg = mls_icg_car(ast_node_method_signature_args);
                ast_node_method_signature_args = mls_icg_cdr(ast_node_method_signature_args);

                if (mls_icg_is_node_atom_id(ast_node_method_signature_arg) == 0)
                {
                    multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, \
                            "%d:%d: error: defclass: bad syntax on method signature, argument should be identifier", \
                            mls_ast_node_ln(ast_node_method_signature_arg), mls_ast_node_col(ast_node_method_signature_arg));
                    ret = -MULTIPLE_ERR_ICODEGEN;
                    goto fail;
                }
                ast_node_atom_method_signature_arg = ast_node_method_signature_arg->ptr; 
                if ((ret = multiply_resource_get_id( \
                                err, \
                                context->icode, \
                                context->res_id, \
                                &id, \
                                ast_node_atom_method_signature_arg->atom->str, \
                                ast_node_atom_method_signature_arg->atom->len)) != 0)
                { goto fail; }

                new_export_section_item->args[idx_arg] = id;
                new_export_section_item->args_types[idx_arg] = MULTIPLE_IR_EXPORT_SECTION_ITEM_ARGS_NORMAL;
            }
        }
        new_export_section_item->name = id_fullname;

        /* Append block */
        if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
        { MULTIPLE_ERROR_INTERNAL(); goto fail; }
        new_icg_fcb_block = NULL;
        /* Append blank export section */
        if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
        { MULTIPLE_ERROR_INTERNAL(); goto fail; }
        new_export_section_item = NULL;

    }


    /* Class Instance Creator */
    instrument_number_creator = (uint32_t)(context->icode->export_section->size);

    /* .text Section Item */
    new_icg_fcb_block = mls_icg_fcb_block_new();
    if (new_icg_fcb_block == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
    /* .export Section Item */
    new_export_section_item = multiple_ir_export_section_item_new();
    if (new_export_section_item == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }
    new_export_section_item->instrument_number = (uint32_t)context->icode->export_section->size;
    new_export_section_item->blank = 0; 
    new_export_section_item->name= id_class_name; 

    if (ctor_enabled == 0)
    {
        /* No Constructor */

        new_export_section_item->args_count = 0; 

        /* def       <class-name> 
         * push      <class-name>
         * clsinstmk
         * return */
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_DEF, id_class_name)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_class_name)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_CLSINSTMK, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
    }
    else
    {
        /* Customized Constructor */

        /* def        <class-name> 
         * arg        <arguments in original order>
         * push       <class-name> 
         * clsinstmk 
         * dup
         * pop        'this'
         * push       <arguments in reverse order>
         * push       'this'
         * push       argument_count
         * push       <full-name>
         * push       <module-name>
         * domain 
         * funcmk 
         * call 
         * pop        none
         * return 
         */
        new_export_section_item->args_count = (size_t)list_node_count_method_signature; 

        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_DEF, id_class_name)) != 0) { goto fail; }

        ast_node_method_signature_args = ast_node_method_signature_args_bak;

        if (mls_icodegen_node_form_args_list(err, \
                    context, \
                    new_icg_fcb_block, \
                    ast_node_method_signature_args, \
                    OP_ARG) != 0)
        { goto fail; }

        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_class_name)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_CLSINSTMK, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_DUP, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_this)) != 0) { goto fail; }

        if (mls_icodegen_node_form_args_list_reverse(err, \
                    context, \
                    new_icg_fcb_block, \
                    ast_node_method_signature_args, \
                    OP_ARG) != 0)
        { goto fail; }

        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_this)) != 0) { goto fail; }
        if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, list_node_count_method_signature + 1)) != 0)
        { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_ctor)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_PUSH, id_module_name)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_DOMAIN, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_CALL, 0)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_POPC, id_null)) != 0) { goto fail; }
        if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }
    }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(context->icg_fcb_block_list, new_icg_fcb_block)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }
    new_icg_fcb_block = NULL;
    /* Append blank export section */
    if ((ret = multiple_ir_export_section_append(context->icode->export_section, new_export_section_item)) != 0)
    { MULTIPLE_ERROR_INTERNAL(); goto fail; }
    new_export_section_item = NULL;

    /* Put Class Instance Creator on the module variable */
    if ((ret = mls_icg_fcb_block_append_with_configure_type( \
                    icg_fcb_block, \
                    OP_LAMBDAMK, instrument_number_creator, \
                    MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_POPM, id_class_name)) != 0) { goto fail; }

    /* Class definition returns a none */
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id_null)) != 0) { goto fail; }


    goto done;
fail:
    if (str_fullname != NULL) free(str_fullname);
    if (new_icg_fcb_block != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}


/*
 * (invoke-method object (<method-name> <arguments-list>) 
 */
int mls_icodegen_node_form_invoke_method(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    int ast_node_args_count;
    struct mls_ast_node *ast_node_object;
    struct mls_ast_node *ast_node_method;
    struct mls_ast_node *ast_node_method_name, *ast_node_arguments;
    struct mls_ast_node_atom *ast_node_atom_method_name;
    uint32_t id;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count != 2)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: method-invoke: bad syntax, "
                "object, method and arguments expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    ast_node_object = mls_icg_car(ast_node);
    ast_node_method = mls_icg_cdar(ast_node);
    ast_node_method_name = mls_icg_car(ast_node_method);
    ast_node_arguments = mls_icg_cdr(ast_node_method);

    if (mls_icg_is_node_atom_id(ast_node_method_name) == 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: method-invoke: bad syntax, "
                "method name should be identifier", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(ast_node_method_name));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* Reversely push arguments */
    ast_node_args_count = mls_icg_count_list_node(ast_node_arguments);
    if (ast_node_args_count > 0)
    {
        if ((ret = mls_icodegen_node_proc_push_arg_reversely(err, \
                        context, \
                        icg_fcb_block, \
                        ast_node_arguments, ast_node_args_count)) != 0)
        { goto fail; }
    }

    if ((ret = multiply_resource_get_int(err, context->icode, context->res_id, &id, ast_node_args_count)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    ast_node_atom_method_name = ast_node_method_name->ptr;
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_atom_method_name->atom->str, \
                    ast_node_atom_method_name->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    if ((ret = mls_icodegen_node(err, \
        context, \
        icg_fcb_block, \
        ast_node_object)) != 0)
    { goto fail; }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CLSMINVOKE, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_FUNCMK, 0)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CALL, 0)) != 0) { goto fail; }


    goto done;
fail:
done:
    return ret;
}

/*
 * (set-property object <property-name> <value>)
 */
int mls_icodegen_node_form_set_property(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    struct mls_ast_node *ast_node_object, *ast_node_member_name, *ast_node_value;
    struct mls_ast_node_atom *ast_node_atom_method_name = NULL; 
    uint32_t id;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count != 3)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: set-property: bad syntax, "
                "object, property and value expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* push value
     * push <member-name>
     * push object
     * clspset */
    ast_node_object = mls_icg_car(ast_node);
    ast_node_member_name = mls_icg_cdar(ast_node);
    ast_node_value = mls_icg_cddar(ast_node);

    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_value)) != 0)
    { goto fail; }

    ast_node_atom_method_name = ast_node_member_name->ptr;
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_atom_method_name->atom->str, \
                    ast_node_atom_method_name->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_object)) != 0)
    { goto fail; }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CLSPSET, 0)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

/*
 * (get-property object <property-name>)
 */
int mls_icodegen_node_form_get_property(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node)
{
    int ret = 0;
    int list_node_count;
    struct mls_ast_node *ast_node_object, *ast_node_member_name;
    struct mls_ast_node_atom *ast_node_atom_method_name = NULL; 
    uint32_t id;

    list_node_count = mls_icg_count_list_node(ast_node);
    if (list_node_count != 2)
    {
        multiple_error_update(err, -MULTIPLE_ERR_ICODEGEN, "%d:%d: error: get-property: bad syntax, " \
                "object, property and value expected", \
                mls_ast_node_ln(context->ast_node_parent), mls_ast_node_col(context->ast_node_parent));
        ret = -MULTIPLE_ERR_ICODEGEN;
        goto fail;
    }

    /* push <member-name>
     * push object
     * clspget */

    ast_node_object = mls_icg_car(ast_node);
    ast_node_member_name = mls_icg_cdar(ast_node);

    ast_node_atom_method_name = ast_node_member_name->ptr;
    if ((ret = multiply_resource_get_id( \
                    err, \
                    context->icode, \
                    context->res_id, \
                    &id, \
                    ast_node_atom_method_name->atom->str, \
                    ast_node_atom_method_name->atom->len)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }

    if ((ret = mls_icodegen_node(err, \
                    context, \
                    icg_fcb_block, \
                    ast_node_object)) != 0)
    { goto fail; }

    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_CLSPGET, 0)) != 0) { goto fail; }

    goto done;
fail:
done:
    return ret;
}

