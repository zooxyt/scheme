/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Floating Code Block
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

#ifndef _MLS_ICG_FCB_H_
#define _MLS_ICG_FCB_H_

#include <stdio.h>
#include <stdint.h>

/* Attributes for each line */

struct mls_icg_fcb_line_attr
{
    uint32_t attr_id;
    uint32_t res_id;
    struct mls_icg_fcb_line_attr *next;
};

struct mls_icg_fcb_line_attr *mls_icg_fcb_line_attr_new(uint32_t attr_id, \
        uint32_t res_id); 
int mls_icg_fcb_line_attr_destroy(struct mls_icg_fcb_line_attr *attr);

struct mls_icg_fcb_line_attr_list
{
    struct mls_icg_fcb_line_attr *begin;
    struct mls_icg_fcb_line_attr *end;
    size_t size;
};

struct mls_icg_fcb_line_attr_list *mls_icg_fcb_line_attr_list_new(void);
int mls_icg_fcb_line_attr_list_destroy(struct mls_icg_fcb_line_attr_list *list);
int mls_icg_fcb_line_attr_list_append(struct mls_icg_fcb_line_attr_list *list, \
        struct mls_icg_fcb_line_attr *new_attr);
int mls_icg_fcb_line_attr_list_append_with_configure(struct mls_icg_fcb_line_attr_list *list, \
        uint32_t attr_id, uint32_t res_id);


enum
{
    /* No needed to do anything to operand */
    MLS_ICG_FCB_LINE_TYPE_NORMAL = 0,        

    /* operand = global_start + operand */
    MLS_ICG_FCB_LINE_TYPE_PC = 1,            

    /* operand = global_offsets_of_lambda_procs[res_id] */
    MLS_ICG_FCB_LINE_TYPE_LAMBDA_MK = 2,     

    /* operand = global_offsets_of_built_in_proces[res_id] 
     * At the beginning of __init__ */
    MLS_ICG_FCB_LINE_TYPE_BLTIN_PROC_MK = 3, 
};

struct mls_icg_fcb_line
{
    uint32_t opcode;
    uint32_t operand;
    int type;
    struct mls_icg_fcb_line_attr_list *attrs;

    struct mls_icg_fcb_line *prev;
    struct mls_icg_fcb_line *next;
};
struct mls_icg_fcb_line *mls_icg_fcb_line_new(void);
int mls_icg_fcb_line_destroy(struct mls_icg_fcb_line *icg_fcb_line);
struct mls_icg_fcb_line *mls_icg_fcb_line_new_with_configure(uint32_t opcode, uint32_t operand);
struct mls_icg_fcb_line *mls_icg_fcb_line_new_with_configure_type(uint32_t opcode, uint32_t operand, int type);

struct mls_icg_fcb_block
{
    struct mls_icg_fcb_line *begin;
    struct mls_icg_fcb_line *end;
    size_t size;

    struct mls_icg_fcb_block *prev;
    struct mls_icg_fcb_block *next;
};
struct mls_icg_fcb_block *mls_icg_fcb_block_new(void);
int mls_icg_fcb_block_destroy(struct mls_icg_fcb_block *icg_fcb_block);
int mls_icg_fcb_block_append(struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_icg_fcb_line *new_icg_fcb_line);
int mls_icg_fcb_block_insert(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t instrument_number_insert, \
        struct mls_icg_fcb_line *new_icg_fcb_line);

int mls_icg_fcb_block_append_with_configure(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t opcode, uint32_t operand);
int mls_icg_fcb_block_append_with_configure_type(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t opcode, uint32_t operand, int type);

uint32_t mls_icg_fcb_block_get_instrument_number(struct mls_icg_fcb_block *icg_fcb_block);

int mls_icg_fcb_block_insert_with_configure(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t instrument, uint32_t opcode, uint32_t operand);
int mls_icg_fcb_block_insert_with_configure_type(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t instrument, uint32_t opcode, uint32_t operand, int type);

int mls_icg_fcb_block_link(struct mls_icg_fcb_block *icg_fcb_block, \
        uint32_t instrument_number_from, uint32_t instrument_number_to);

struct mls_icg_fcb_block_list
{
    struct mls_icg_fcb_block *begin;
    struct mls_icg_fcb_block *end;
    size_t size;
};
struct mls_icg_fcb_block_list *mls_icg_fcb_block_list_new(void);
int mls_icg_fcb_block_list_destroy(struct mls_icg_fcb_block_list *icg_fcb_block_list);
int mls_icg_fcb_block_list_append(struct mls_icg_fcb_block_list *icg_fcb_block_list, \
        struct mls_icg_fcb_block *new_icg_fcb_block);


struct mls_icg_customizable_built_in_procedure_write_back
{
    uint32_t instrument_number;
    struct mls_icg_customizable_built_in_procedure_write_back *next;
};
struct mls_icg_customizable_built_in_procedure_write_back *mls_icg_customizable_built_in_procedure_write_back_new(uint32_t instrument_number);
int mls_icg_customizable_built_in_procedure_write_back_destroy(struct mls_icg_customizable_built_in_procedure_write_back *write_back);

struct mls_icg_customizable_built_in_procedure_write_back_list
{
    struct mls_icg_customizable_built_in_procedure_write_back *begin;
    struct mls_icg_customizable_built_in_procedure_write_back *end;
    size_t size;
};
struct mls_icg_customizable_built_in_procedure_write_back_list *mls_icg_customizable_built_in_procedure_write_back_list_new(void);
int mls_icg_customizable_built_in_procedure_write_back_list_destroy(struct mls_icg_customizable_built_in_procedure_write_back_list *write_back_list);
int mls_icg_customizable_built_in_procedure_write_back_list_append_with_configure(struct mls_icg_customizable_built_in_procedure_write_back_list *write_back_list, uint32_t instrument_number);

struct mls_icg_customizable_built_in_procedure
{
    char *name;
    size_t name_len;
    int called;

    /* instrument number in icode */
    uint32_t instrument_number_icode;
    struct mls_icg_customizable_built_in_procedure_write_back_list *write_backs;

    struct mls_icg_customizable_built_in_procedure *next;
};
struct mls_icg_customizable_built_in_procedure *mls_icg_customizable_built_in_procedure_new(const char *name, const size_t name_len);
int mls_icg_customizable_built_in_procedure_destroy(struct mls_icg_customizable_built_in_procedure *customizable_built_in_procedure);

struct mls_icg_customizable_built_in_procedure_list
{
    struct mls_icg_customizable_built_in_procedure *begin;
    struct mls_icg_customizable_built_in_procedure *end;
};
struct mls_icg_customizable_built_in_procedure_list *mls_icg_customizable_built_in_procedure_list_new(void);
int mls_icg_customizable_built_in_procedure_list_destroy(struct mls_icg_customizable_built_in_procedure_list *list);

struct mls_icg_customizable_built_in_procedure *mls_icg_customizable_built_in_procedure_list_lookup(struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list, \
        const char *name, const size_t name_len);
int mls_icg_customizable_built_in_procedure_list_called( \
        struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list, \
        const char *name, const size_t name_len);
int mls_icg_customizable_built_in_procedure_list_add_writeback( \
        struct mls_icg_customizable_built_in_procedure_list *customizable_built_in_procedure_list, \
        const char *name, const size_t name_len, uint32_t instrument_number);

#endif

