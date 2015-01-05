/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Global Context
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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "multiple_ir.h"
#include "multiply.h"

#include "mls_ast.h"
#include "mls_icg_fcb.h"
#include "mls_icg_context.h"

int mls_icg_context_init(struct mls_icg_context *context)
{
    context->icg_fcb_block_list = NULL;
    context->icg_fcb_block_predefined = NULL;
    context->icode = NULL;
    context->res_id = NULL;

    context->module_name = NULL;
    context->module_name_len = 0;
    context->attr = 0;
    context->ast_node_parent = NULL;
    return 0;
}

int mls_icg_context_uninit(struct mls_icg_context *context)
{
    if (context->module_name != NULL)
    { free(context->module_name); context->module_name = NULL; }

    return 0;
}

int mls_icg_context_update_module_name(struct mls_icg_context *context, \
        const char *module_name, const size_t module_name_len)
{
    if (context->module_name != NULL)
    { free(context->module_name); }

    context->module_name = (char *)malloc(sizeof(char) * (module_name_len + 1));
    if (context->module_name == NULL) { return -1; }
    memcpy(context->module_name, module_name, module_name_len);
    context->module_name[module_name_len] = '\0';
    context->module_name_len = module_name_len;

    return 0;
}

int mls_icg_context_clear_all(struct mls_icg_context *context)
{
    context->attr = 0;
    return 0;
}

int mls_icg_context_set(struct mls_icg_context *context, uint32_t idx)
{
    context->attr |= (uint32_t)(1 << idx);
    return 0;
}

int mls_icg_context_clear(struct mls_icg_context *context, uint32_t idx)
{
    context->attr &= ~((uint32_t)(1 << idx));
    return 0;
}

int mls_icg_context_test(struct mls_icg_context *context, uint32_t idx)
{
    return (context->attr & (uint32_t)(1 << idx)) != 0 ? 1 : 0;
}

