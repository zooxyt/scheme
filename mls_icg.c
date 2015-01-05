/* Multiple Scheme Programming Language : Intermediate Code Generator
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

#include "vm_opcode.h"
#include "vm_predef.h"

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_fcb.h"
#include "mls_icg_aux.h"

#include "mls_icg_context.h"

#include "mls_icg_generic.h"
#include "mls_icg_body.h"
#include "mls_icg_built_in_proc.h"


static int mls_icodegen_merge_from_fcb_block_list(struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct mls_icg_fcb_block_list *icg_fcb_block_list)
{
    int ret = 0;
    struct mls_icg_fcb_block *icg_fcb_block_cur;
    struct mls_icg_fcb_line *icg_fcb_line_cur;

    uint32_t instrument_number;
    struct multiple_ir_export_section_item *export_section_item_cur;
    struct multiple_ir_text_section_item *text_section_item_cur;

    uint32_t offset_start;
    uint32_t fcb_size = 0;

    uint32_t count;

    if (icode == NULL) return -MULTIPLE_ERR_INTERNAL;
    if (icg_fcb_block_list == NULL) return -MULTIPLE_ERR_INTERNAL;

	/* Do not disturb the instrument produced by other way */
	offset_start = (uint32_t)(icode->text_section->size);
	
	export_section_item_cur = icode->export_section->begin;
    icg_fcb_block_cur = icg_fcb_block_list->begin;
    while (icg_fcb_block_cur != NULL)
    {
        icg_fcb_line_cur = icg_fcb_block_cur->begin;

        /* Record the absolute instrument number */
        instrument_number = (uint32_t)icode->text_section->size;
        if (export_section_item_cur == NULL)
        {
            MULTIPLE_ERROR_INTERNAL();
            ret = -MULTIPLE_ERR_INTERNAL;
            goto fail;
        }
        export_section_item_cur->instrument_number = instrument_number;

        while (icg_fcb_line_cur != NULL)
        {
            switch (icg_fcb_line_cur->type)
            {
                case MLS_ICG_FCB_LINE_TYPE_NORMAL:
                    if ((ret = multiply_icodegen_text_section_append(err, \
                                    icode, \
                                    icg_fcb_line_cur->opcode, icg_fcb_line_cur->operand)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_FCB_LINE_TYPE_PC:
                    if ((ret = multiply_icodegen_text_section_append(err, \
                                    icode, \
                                    icg_fcb_line_cur->opcode, instrument_number + icg_fcb_line_cur->operand)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK:
                    /* Operand of this instrument here is the index number of lambda */
                    if ((ret = multiply_icodegen_text_section_append(err, \
                                    icode, \
                                    icg_fcb_line_cur->opcode, icg_fcb_line_cur->operand)) != 0)
                    { goto fail; }
                    break;
                case MLS_ICG_FCB_LINE_TYPE_BLTIN_PROC_MK:
                    if ((ret = multiply_icodegen_text_section_append(err, \
                                    icode, \
                                    icg_fcb_line_cur->opcode, icg_fcb_line_cur->operand)) != 0)
                    { goto fail; }
                    break;
            }

            fcb_size += 1;
            icg_fcb_line_cur = icg_fcb_line_cur->next;
        }

        icg_fcb_block_cur = icg_fcb_block_cur->next;
        export_section_item_cur = export_section_item_cur->next;
    }

    /* 2nd pass, dealing with lambdas */
    icg_fcb_block_cur = icg_fcb_block_list->begin;
    /* Skip text body of built-in procedures at the beginning part */
    text_section_item_cur = icode->text_section->begin;
    while (offset_start-- > 0)
    {
        text_section_item_cur = text_section_item_cur->next; 
    }
    /* Process lambda mks */
    while (icg_fcb_block_cur != NULL)
    {
        icg_fcb_line_cur = icg_fcb_block_cur->begin;
        while (icg_fcb_line_cur != NULL)
        {
            if (icg_fcb_line_cur->type == MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK)
            {
                /* Locate to the export section item */
                count = icg_fcb_line_cur->operand;
                export_section_item_cur = icode->export_section->begin;
                while ((export_section_item_cur != NULL) && (count != 0))
                {
                    count--;
                    export_section_item_cur = export_section_item_cur->next;
                }
                if (export_section_item_cur == NULL)
                {
                    MULTIPLE_ERROR_INTERNAL();
                    ret = -MULTIPLE_ERR_INTERNAL;
                    goto fail;
                }
                text_section_item_cur->operand = export_section_item_cur->instrument_number; 
            }
            text_section_item_cur = text_section_item_cur->next; 
            icg_fcb_line_cur = icg_fcb_line_cur->next;
        }

        icg_fcb_block_cur = icg_fcb_block_cur->next;
    }

    goto done;
fail:
done:
    return ret;
}

static int mls_icodegen_special(struct multiple_error *err, \
        struct multiple_ir *icode, \
        struct multiply_resource_id_pool *res_id, \
        struct mls_icg_fcb_block_list *icg_fcb_block_list, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        const char *name, const size_t name_len)
{
    int ret = 0;
    uint32_t id;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

    new_export_section_item = multiple_ir_export_section_item_new();
    if (new_export_section_item == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    new_export_section_item->args_count = 0;
    new_export_section_item->args = NULL;
    new_export_section_item->args_types = NULL;

    /* Return */
    if ((ret = multiply_resource_get_none(err, icode, res_id, &id)) != 0) 
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_PUSH, id)) != 0) { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(icg_fcb_block, OP_RETURN, 0)) != 0) { goto fail; }

    /* Append block */
    if ((ret = mls_icg_fcb_block_list_append(icg_fcb_block_list, icg_fcb_block)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }

    /* Append export section item */
    if ((ret = multiply_resource_get_id( \
                    err, icode, res_id, &id, \
                    name, name_len)) != 0)
    { goto fail; }

    new_export_section_item->name = id;
    new_export_section_item->instrument_number = (uint32_t)icode->export_section->size;
    if ((ret = multiple_ir_export_section_append(icode->export_section, new_export_section_item)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        goto fail;
    }

    goto done;
fail:
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
done:
    return ret;
}

static int mls_icodegen_program(struct multiple_error *err, \
        struct multiple_ir **icode_out, \
        struct mls_ast_program *program)
{
    int ret = 0;
    uint32_t id;
    struct mls_icg_fcb_block_list *new_icg_fcb_block_list = NULL;
    struct mls_icg_fcb_block *new_icg_fcb_block_autorun = NULL;
    struct mls_icg_fcb_block *new_icg_fcb_block_predefined = NULL;
    struct mls_icg_customizable_built_in_procedure_list *new_customizable_built_in_procedure_list = NULL;
    struct multiple_ir *new_icode = NULL;
    struct multiply_resource_id_pool *new_res_id = NULL;
    struct multiple_ir_export_section_item *new_export_section_item = NULL;

    struct mls_icg_context context;

    uint32_t instrument_number_insert_point_built_in_proc;
    uint32_t instrument_count_built_in_proc;

    context.module_name = NULL;

    new_icg_fcb_block_list = mls_icg_fcb_block_list_new();
    if (new_icg_fcb_block_list == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_export_section_item = multiple_ir_export_section_item_new();
    if (new_export_section_item == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_icg_fcb_block_autorun = mls_icg_fcb_block_new();
    if (new_icg_fcb_block_autorun == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_icg_fcb_block_predefined = mls_icg_fcb_block_new();
    if (new_icg_fcb_block_predefined == NULL) 
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_icode = multiple_ir_new();
    if (new_icode == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_res_id = multiply_resource_id_pool_new();
    if (new_res_id == NULL)
    { MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }

    new_customizable_built_in_procedure_list = mls_icg_customizable_built_in_procedure_list_new();
    if (new_customizable_built_in_procedure_list == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    /* __predefined__ */
    if ((ret = multiply_resource_get_id( \
                    err, 
                    new_icode, \
                    new_res_id, \
                    &id, \
                    VM_PREDEF_MODULE_CLASS_STRUCT_DEFINITION, \
                    VM_PREDEF_MODULE_CLASS_STRUCT_DEFINITION_LEN)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block_predefined, OP_DEF, id)) != 0) { goto fail; }

    /* def in .text section */
    if ((ret = multiply_resource_get_id( \
                    err, \
                    new_icode, \
                    new_res_id, \
                    &id, \
                    VM_PREDEF_MODULE_AUTORUN, \
                    VM_PREDEF_MODULE_AUTORUN_LEN)) != 0)
    { goto fail; }
    if ((ret = mls_icg_fcb_block_append_with_configure(new_icg_fcb_block_autorun, OP_DEF, id)) != 0) { goto fail; }
    new_export_section_item->name = id;

    instrument_number_insert_point_built_in_proc = mls_icg_fcb_block_get_instrument_number(new_icg_fcb_block_autorun);

    /* Setup Context */
    mls_icg_context_init(&context);
    mls_icg_context_clear_all(&context);
    context.icg_fcb_block_list = new_icg_fcb_block_list;
    context.icg_fcb_block_predefined = new_icg_fcb_block_predefined;
    context.icode = new_icode;
    context.res_id = new_res_id;
    context.customizable_built_in_procedure_list = new_customizable_built_in_procedure_list;
    if (mls_icg_context_update_module_name(&context, \
                VM_PREDEF_MODULE_NO_NAME, VM_PREDEF_MODULE_NO_NAME_LEN) != 0)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_INTERNAL;
        goto fail;
    }

    /* Root */
    mls_icg_context_set(&context, MLS_ICG_CONTEXT_ROOT);

    /* First turn, generate floating code block */
    if (program->root != NULL)
    {
        if ((ret = mls_icodegen_node_body(err, \
                        &context, \
                        new_icg_fcb_block_autorun, \
                        program->root)) != 0)
        { goto fail; }
    }


    /* <Procedure Arrangement in final icode> 
     * 1. Built-in procedures 
     * 2. Lambda functions 
     * 3. class methods + lambda functions 
     * 4. '__autorun__' (entrance of program)
     * 5. '__predefined__' (class data structure) 
     */

    /* Put built-in procedures directly into icode,
     * and put initialize code into '__autorun__' */
    /* 1. Built-in procedures */
    if ((ret = mls_icg_add_built_in_procs(err, \
                    new_icode, \
                    new_res_id, \
                    new_icg_fcb_block_autorun, \
                    new_customizable_built_in_procedure_list, \
                    instrument_number_insert_point_built_in_proc,
                    &instrument_count_built_in_proc)) != 0)
    { goto fail; }

    /* '__autorun__' subroutine */
    if ((ret = mls_icodegen_special( \
                    err, \
                    new_icode, \
                    new_res_id, \
                    new_icg_fcb_block_list, new_icg_fcb_block_autorun, \
                    VM_PREDEF_MODULE_AUTORUN, VM_PREDEF_MODULE_AUTORUN_LEN)) != 0)
    { goto fail; }
    new_icg_fcb_block_autorun = NULL;

    /* '__predefined__' subroutine */
    if ((ret = mls_icodegen_special( \
                    err, \
                    new_icode, \
                    new_res_id, \
                    new_icg_fcb_block_list, new_icg_fcb_block_predefined, \
                    VM_PREDEF_MODULE_CLASS_STRUCT_DEFINITION, VM_PREDEF_MODULE_CLASS_STRUCT_DEFINITION_LEN)) != 0)
    { goto fail; }
    new_icg_fcb_block_predefined = NULL;

    /* Generate icode */
    if ((ret = mls_icodegen_merge_from_fcb_block_list(err, new_icode, \
                    new_icg_fcb_block_list)) != 0)
    {
        MULTIPLE_ERROR_INTERNAL();
        ret = -MULTIPLE_ERR_INTERNAL;
        goto fail;
    }

    *icode_out = new_icode;

    ret = 0;
    goto done;
fail:
    if (new_icode != NULL) multiple_ir_destroy(new_icode);
done:
    if (new_res_id != NULL) multiply_resource_id_pool_destroy(new_res_id);
    if (new_icg_fcb_block_list != NULL) mls_icg_fcb_block_list_destroy(new_icg_fcb_block_list);
    if (new_icg_fcb_block_autorun != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block_autorun);
    if (new_icg_fcb_block_predefined != NULL) mls_icg_fcb_block_destroy(new_icg_fcb_block_predefined);
    if (new_customizable_built_in_procedure_list != NULL) mls_icg_customizable_built_in_procedure_list_destroy(new_customizable_built_in_procedure_list);
    if (new_export_section_item != NULL) multiple_ir_export_section_item_destroy(new_export_section_item);
    mls_icg_context_uninit(&context);
    return ret;
}

int mls_irgen(struct multiple_error *err, struct multiple_ir **icode_out, struct mls_ast_program *program, int verbose)
{
    int ret = 0;

    struct multiple_ir *new_icode = NULL;

    (void)verbose;

    ret = mls_icodegen_program(err, &new_icode, program);

    *icode_out = new_icode;

    return ret;
}

