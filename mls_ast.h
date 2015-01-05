/* Multiple Scheme Programming Language : Abstract Syntax Tree
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

#ifndef _MLS_AST_H_
#define _MLS_AST_H_

#include "mls_lexer.h"

/* Generic Abstract Syntax Tree Node */

#define MLS_AST_NODE_ATOM 0
#define MLS_AST_NODE_CONS 1

struct mls_ast_node
{
    int type;
    void *ptr;

    struct mls_ast_node *rcar;
};
struct mls_ast_node *mls_ast_node_new(int type);
int mls_ast_node_destroy(struct mls_ast_node *node); 
int mls_ast_node_ln(struct mls_ast_node *node); 
int mls_ast_node_col(struct mls_ast_node *node); 
char *mls_ast_node_str(struct mls_ast_node *node); 
struct mls_ast_node *mls_ast_node_clone( \
        struct mls_ast_node *ast_node_src);

struct mls_ast_node_atom
{
    struct token *atom;
};
struct mls_ast_node_atom *mls_ast_node_atom_new(void);
int mls_ast_node_atom_destroy(struct mls_ast_node_atom *node_atom);
struct mls_ast_node_atom *mls_ast_node_atom_clone(struct mls_ast_node_atom *node_atom);

struct mls_ast_node_cons
{
    struct token *token;
    struct mls_ast_node *car;
    struct mls_ast_node *cdr;
};
struct mls_ast_node_cons *mls_ast_node_cons_new(void);
int mls_ast_node_cons_destroy(struct mls_ast_node_cons *node); 
struct mls_ast_node_cons *mls_ast_node_cons_clone(struct mls_ast_node_cons *node_cons);


/* Specialized Abstract Syntax Tree Node */


/* Program */

struct mls_ast_program
{
    struct mls_ast_node *root;
};
struct mls_ast_program *mls_ast_program_new(void); 
int mls_ast_program_destroy(struct mls_ast_program *program);

#endif

