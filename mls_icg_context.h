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

#ifndef _MLS_ICG_CONTEXT_H_
#define _MLS_ICG_CONTEXT_H_

#include <stdint.h>

#include "multiple_ir.h"

#include "multiply.h"

#include "mls_ast.h"
#include "mls_icg_fcb.h"

struct mls_icg_context
{
    struct mls_icg_fcb_block_list *icg_fcb_block_list;
    struct mls_icg_fcb_block *icg_fcb_block_predefined;
    struct multiple_ir *icode;
    struct multiply_resource_id_pool *res_id;
    char *module_name;
    size_t module_name_len;
    struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list;

    struct mls_ast_node *ast_node_parent;
    uint32_t attr; /* Short message passing in short distance */
};

int mls_icg_context_init(struct mls_icg_context *context);
int mls_icg_context_uninit(struct mls_icg_context *context);
int mls_icg_context_update_module_name(struct mls_icg_context *context, \
        const char *module_name, const size_t module_name_len);

enum
{
    MLS_ICG_CONTEXT_TAIL = 0,
    MLS_ICG_CONTEXT_TAILCALL = 1,
    MLS_ICG_CONTEXT_ROOT = 2,
    MLS_ICG_CONTEXT_TOPLEVEL = 3,
    MLS_ICG_CONTEXT_CONT = 4,
    MLS_ICG_CONTEXT_DUPFUNC = 5,
};

int mls_icg_context_clear_all(struct mls_icg_context *context);
int mls_icg_context_set(struct mls_icg_context *context, uint32_t idx);
int mls_icg_context_clear(struct mls_icg_context *context, uint32_t idx);
int mls_icg_context_test(struct mls_icg_context *context, uint32_t idx);

#endif

