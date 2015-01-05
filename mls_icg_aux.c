/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Auxiliary
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

#include <stdio.h>
#include <string.h>

#include "mls_lexer.h"
#include "mls_ast.h"
#include "mls_icg.h"
#include "mls_icg_aux.h"

struct mls_ast_node *mls_icg_car(struct mls_ast_node *ast_node)
{
    if (ast_node == NULL) return NULL;

    /* Atom or Pair */
    if (ast_node->type != MLS_AST_NODE_CONS) return NULL;

    return ((struct mls_ast_node_cons *)(ast_node->ptr))->car;
}

struct mls_ast_node *mls_icg_caar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_car(ast_node));
}

struct mls_ast_node *mls_icg_caaar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_car(mls_icg_car(ast_node)));
}

struct mls_ast_node *mls_icg_caadr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_car(mls_icg_car(ast_node)));
}

struct mls_ast_node *mls_icg_cadr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_car(ast_node));
}

struct mls_ast_node *mls_icg_cadar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_cdr(mls_icg_car(ast_node)));
}

struct mls_ast_node *mls_icg_caddr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_cdr(mls_icg_car(ast_node)));
}

struct mls_ast_node *mls_icg_cdr(struct mls_ast_node *ast_node)
{
    if (ast_node == NULL) return NULL;

    /* Atom or Pair */
    if (ast_node->type != MLS_AST_NODE_CONS) return NULL;

    return ((struct mls_ast_node_cons *)(ast_node->ptr))->cdr;
}

struct mls_ast_node *mls_icg_cdar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_cdr(ast_node));
}

struct mls_ast_node *mls_icg_cdaar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_car(mls_icg_cdr(ast_node)));
}

struct mls_ast_node *mls_icg_cdadr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_car(mls_icg_cdr(ast_node)));
}

struct mls_ast_node *mls_icg_cddr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_cdr(ast_node));
}

struct mls_ast_node *mls_icg_cddar(struct mls_ast_node *ast_node)
{
    return mls_icg_car(mls_icg_cdr(mls_icg_cdr(ast_node)));
}

struct mls_ast_node *mls_icg_cdddr(struct mls_ast_node *ast_node)
{
    return mls_icg_cdr(mls_icg_cdr(mls_icg_cdr(ast_node)));
}

int mls_icg_is_node_cons(const struct mls_ast_node *ast_node)
{
    /* Null */
    if (ast_node == NULL) return 0;

    /* Atom or Pair */
    if (ast_node->type != MLS_AST_NODE_CONS) return 0;

    return 1;
}

int mls_icg_is_node_atom_id(const struct mls_ast_node *ast_node)
{
    struct mls_ast_node_atom *ast_node_atom;

    /* Null */
    if (ast_node == NULL) return 0;

    /* Atom or Pair */
    if (ast_node->type != MLS_AST_NODE_ATOM) return 0;

    /* Type */
    ast_node_atom = ast_node->ptr;
    if (ast_node_atom->atom->value != TOKEN_IDENTIFIER) return 0;

    return 1;
}

int mls_icg_is_node_atom_id_with_name(const struct mls_ast_node *ast_node, const char *id)
{
    struct mls_ast_node_atom *ast_node_atom;
    size_t id_len;

    /* Null */
    if (ast_node == NULL) return 0;

    /* Atom or Pair */
    if (ast_node->type != MLS_AST_NODE_ATOM) return 0;

    /* Type */
    ast_node_atom = ast_node->ptr;
    if (ast_node_atom->atom->value != TOKEN_IDENTIFIER) return 0;

    /* Text Literal */
    id_len = strlen(id);
    if ((ast_node_atom->atom->len != id_len) || (strncmp(ast_node_atom->atom->str, id, id_len) != 0)) return 0;

    return 1;
}

int mls_icg_count_list_node(struct mls_ast_node *ast_node)
{
    struct mls_ast_node *ast_node_cur;
    struct mls_ast_node_cons *ast_node_cons;
    int count = 0;

    ast_node_cur = ast_node;
    for (;;)
    {
        /* Null */
        if (ast_node_cur == NULL) break;
        /* Is Pair */
        if (ast_node_cur->type != MLS_AST_NODE_CONS) break;
        /* Increase Counter */
        count += 1;
        /* Rest */
        ast_node_cons = ast_node_cur->ptr;
        if (ast_node_cons->cdr == NULL) break;
        /* Next */
        ast_node_cur = ast_node_cons->cdr;
    }

    return count;
}

int mls_icg_print_indent(const int indent)
{
    int count = indent;
    while (count-- != 0) { fputc(' ', stdout); }
    return 0;
}

