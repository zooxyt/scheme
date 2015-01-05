/* Multiple Scheme Programming Language : Stub
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

#ifndef _SCHEME_STUB_H_
#define _SCHEME_STUB_H_

#include <stdio.h>

#include "multiple_ir.h"
#include "multiple_err.h"

#define MLS_FRONTNAME "scheme"
#define MLS_FULLNAME "Scheme"
#define MLS_EXT "ss;scm"

struct mls_stub
{
    /* plain text of source code */
    char *code;
    size_t len;

    /* intermediate data */
    struct token_list *tokens;
    struct mls_ast_program *program;

    /* options */
    int opt_internal_reconstruct;

	/* debug info */
	int debug_info;
};

int mls_stub_create(struct multiple_error *err, void **stub_out, \
        char *pathname_dst, int type_dst, \
        char *pathname_src, int type_src);
int mls_stub_destroy(void *stub);
int mls_stub_debug_info_set(void *stub, int debug_info);
int mls_stub_tokens_print(struct multiple_error *err, void *stub);
int mls_stub_reconstruct(struct multiple_error *err, struct multiple_ir **ir, void *stub);
int mls_stub_irgen(struct multiple_error *err, struct multiple_ir **ir, void *stub);

#endif

