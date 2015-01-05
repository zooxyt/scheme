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

#ifndef _MLS_ICG_PROC_LUT_H_
#define _MLS_ICG_PROC_LUT_H_

struct mls_icg_proc_lut_item
{
    const int value;
    const char *str;
    const size_t len;
    const int args_min;
    const int args_max;
    const int op;
    const int type;
};

#define MLS_ICG_PROC_OP_MANUAL 0
#define MLS_ICG_PROC_OP_CALL 1
#define MLS_ICG_PROC_OP_UNHANDLED 2

#define MLS_ICG_UNKNOWN (-1)

int mls_icodegen_node_proc_detect(struct multiple_error *err, \
        int *idx_out, \
        const struct mls_ast_node *ast_node_proc, \
        const struct mls_icg_proc_lut_item *items, const size_t count, \
        struct mls_ast_node *ast_node_args);

int mls_icodegen_node_proc(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc, \
        struct mls_ast_node *ast_node_args, \
        const int procedure_idx);

#endif

