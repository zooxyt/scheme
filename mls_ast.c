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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "mls_lexer.h"
#include "mls_ast.h"


struct mls_ast_node *mls_ast_node_new(int type)
{
    struct mls_ast_node *new_node;

    new_node = (struct mls_ast_node *)malloc(sizeof(struct mls_ast_node));
    if (new_node == NULL) return NULL;

    new_node->type = type;
    new_node->ptr = NULL;
    new_node->rcar = NULL;

    return new_node;
}

int mls_ast_node_destroy(struct mls_ast_node *node)
{
    if (node == NULL) { return -MULTIPLE_ERR_NULL_PTR; }

    switch (node->type)
    {
        case MLS_AST_NODE_ATOM:
            mls_ast_node_atom_destroy(node->ptr);
            break;
        case MLS_AST_NODE_CONS:
            mls_ast_node_cons_destroy(node->ptr); 
            break;
        default:
            return -MULTIPLE_ERR_INTERNAL;
            break;
    }
    free(node);

    return 0;
}

int mls_ast_node_ln(struct mls_ast_node *node)
{
    struct mls_ast_node_atom *node_atom;
    struct mls_ast_node_cons *node_cons;

    if (node == NULL) { return 0; }

    switch (node->type)
    {
        case MLS_AST_NODE_ATOM:
            node_atom = node->ptr;
            if ((node_atom == NULL) || (node_atom->atom == NULL)) return 0;
            return (int)(node_atom->atom->pos_ln);
        case MLS_AST_NODE_CONS:
            node_cons = node->ptr;
            if ((node_cons == NULL) || (node_cons->token == NULL)) return 0;
            return (int)(node_cons->token->pos_ln);
        default:
            return 0;
    }
}

int mls_ast_node_col(struct mls_ast_node *node)
{
    struct mls_ast_node_atom *node_atom;
    struct mls_ast_node_cons *node_cons;

    if (node == NULL) { return 0; }

    switch (node->type)
    {
        case MLS_AST_NODE_ATOM:
            node_atom = node->ptr;
            if ((node_atom == NULL) || (node_atom->atom == NULL)) return 0;
            return (int)(node_atom->atom->pos_col);
        case MLS_AST_NODE_CONS:
            node_cons = node->ptr;
            if ((node_cons == NULL) || (node_cons->token == NULL)) return 0;
            return (int)(node_cons->token->pos_col);
        default:
            return 0;
    }
}

char *mls_ast_node_str(struct mls_ast_node *node)
{
    struct mls_ast_node_atom *node_atom;
    struct mls_ast_node_cons *node_cons;

    if (node == NULL) { return 0; }

    switch (node->type)
    {
        case MLS_AST_NODE_ATOM:
            node_atom = node->ptr;
            if ((node_atom == NULL) || (node_atom->atom == NULL)) return 0;
            return node_atom->atom->str;
        case MLS_AST_NODE_CONS:
            node_cons = node->ptr;
            if ((node_cons == NULL) || (node_cons->token == NULL)) return 0;
            return node_cons->token->str;
        default:
            return NULL;
    }
}

struct mls_ast_node *mls_ast_node_clone( \
        struct mls_ast_node *ast_node_src)
{
    struct mls_ast_node *new_ast_node = NULL;

    switch (ast_node_src->type)
    {
        case MLS_AST_NODE_ATOM:
            if ((new_ast_node = mls_ast_node_new(MLS_AST_NODE_ATOM))== NULL) 
            { goto fail; }
            if ((new_ast_node->ptr = mls_ast_node_atom_clone(ast_node_src->ptr)) == NULL)
            { goto fail; }
            break;

        case MLS_AST_NODE_CONS:
            if ((new_ast_node = mls_ast_node_new(MLS_AST_NODE_CONS))== NULL) 
            {  goto fail; }
            if ((new_ast_node->ptr = mls_ast_node_cons_clone(ast_node_src->ptr))== NULL) 
            {  goto fail; }
            break;

        default:
            goto fail;
    }

    /*if (ast_node_src->rcar != NULL)*/
    /*{*/
    /*if ((new_ast_node->rcar = mls_ast_node_clone( \*/
    /*ast_node_src->rcar)) == NULL)*/
    /*{ goto fail; }*/
    /*}*/

    goto done;
fail:
    if (new_ast_node != NULL)
    {
        mls_ast_node_destroy(new_ast_node);
        new_ast_node = NULL;
    }
done:
    return new_ast_node;
}


struct mls_ast_node_atom *mls_ast_node_atom_new(void)
{
    struct mls_ast_node_atom *new_node_atom;

    new_node_atom = (struct mls_ast_node_atom *)malloc(sizeof(struct mls_ast_node_atom));
    if (new_node_atom == NULL) return NULL;

    new_node_atom->atom = NULL;

    return new_node_atom;
}

int mls_ast_node_atom_destroy(struct mls_ast_node_atom *node_atom)
{
    if (node_atom == NULL) return -MULTIPLE_ERR_NULL_PTR;

    if (node_atom->atom != NULL) token_destroy(node_atom->atom);
    free(node_atom);

    return 0;
}

struct mls_ast_node_atom *mls_ast_node_atom_clone(struct mls_ast_node_atom *node_atom)
{
    struct mls_ast_node_atom *new_node_atom = NULL;

    if ((new_node_atom = mls_ast_node_atom_new()) == NULL)
    { goto fail; }
    if (node_atom->atom != NULL) 
    { 
        if ((new_node_atom->atom = token_clone(node_atom->atom)) == NULL)
        { goto fail; }; 
    }
    return new_node_atom;

fail:
    mls_ast_node_atom_destroy(new_node_atom);
    return NULL;
}

struct mls_ast_node_cons *mls_ast_node_cons_new(void)
{
    struct mls_ast_node_cons *new_node_cons;

    new_node_cons = (struct mls_ast_node_cons *)malloc(sizeof(struct mls_ast_node_cons));
    if (new_node_cons == NULL) return NULL;

    new_node_cons->token = NULL;
    new_node_cons->car = NULL;
    new_node_cons->cdr = NULL;

    return new_node_cons;
}

int mls_ast_node_cons_destroy(struct mls_ast_node_cons *node_cons)
{
    if (node_cons == NULL) return -MULTIPLE_ERR_NULL_PTR;

    if (node_cons->token != NULL) token_destroy(node_cons->token);
    if (node_cons->car != NULL) mls_ast_node_destroy(node_cons->car);
    if (node_cons->cdr != NULL) mls_ast_node_destroy(node_cons->cdr);
    free(node_cons);

    return 0;
}

struct mls_ast_node_cons *mls_ast_node_cons_clone(struct mls_ast_node_cons *node_cons)
{
    struct mls_ast_node_cons *new_node_cons = NULL;

    if ((new_node_cons = mls_ast_node_cons_new()) == NULL)
    { goto fail; }
    if (node_cons->car != NULL) 
    { 
        if ((new_node_cons->car = mls_ast_node_clone(node_cons->car)) == NULL)
        { goto fail; }
    }
    if (node_cons->cdr != NULL) 
    { 
        if ((new_node_cons->cdr = mls_ast_node_clone(node_cons->cdr)) == NULL)
        { goto fail; }
    }
    if (node_cons->token != NULL)
    {
        if ((new_node_cons->token = token_clone(node_cons->token)) == NULL)
        { goto fail; }
    }

    goto done;
fail:
    if (new_node_cons == NULL)
    {
        mls_ast_node_cons_destroy(new_node_cons);
        new_node_cons = NULL;
    }
done:
    return new_node_cons;
}


struct mls_ast_program *mls_ast_program_new(void)
{
    struct mls_ast_program *new_program;

    new_program = (struct mls_ast_program *)malloc(sizeof(struct mls_ast_program));
    if (new_program == NULL) return NULL;

    new_program->root = NULL;

    return new_program;
}

int mls_ast_program_destroy(struct mls_ast_program *program)
{
    if (program == NULL) return -MULTIPLE_ERR_NULL_PTR;

    if (program->root != NULL) mls_ast_node_destroy(program->root);

    free(program);

    return 0;
}


