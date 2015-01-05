/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Auxiliary
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

#ifndef _MLS_ICG_AUX_H_
#define _MLS_ICG_AUX_H_

#include "mls_ast.h"

struct mls_ast_node *mls_icg_car(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_caar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_caaar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_caadr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cadr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cadar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_caddr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cdr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cdar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cdaar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cdadr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cddr(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cddar(struct mls_ast_node *ast_node);
struct mls_ast_node *mls_icg_cdddr(struct mls_ast_node *ast_node);

int mls_icg_is_node_cons(const struct mls_ast_node *ast_node);
int mls_icg_is_node_atom_id(const struct mls_ast_node *ast_node);
int mls_icg_is_node_atom_id_with_name(const struct mls_ast_node *ast_node, const char *name);

int mls_icg_count_list_node(struct mls_ast_node *ast_node);
int mls_icg_print_indent(const int indent);


#endif

