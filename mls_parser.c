/* Multiple Scheme Programming Language : Parser
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
#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "mls_ast.h"
#include "mls_parser.h"

#include "mls_me.h"

static int mls_parse_list(struct multiple_error *err, \
        struct mls_ast_node **ast_node_out, \
        struct token **token_cur_in_out, \
        int level)
{
    int ret = 0;
    struct token *token_cur = NULL;

    struct mls_ast_node *new_ast_node = NULL, *new_ast_node_2 = NULL;
    struct mls_ast_node_atom *new_ast_node_atom = NULL;
    struct mls_ast_node_cons *new_ast_node_cons = NULL;

    struct mls_ast_node *ast_node_head = NULL;
    struct mls_ast_node *ast_node_cur = NULL;
    struct token *token_bak = NULL;

    if (*token_cur_in_out == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }

    token_cur = *token_cur_in_out;
    *ast_node_out = NULL;

    while (token_cur->value != TOKEN_FINISH)
    {
        switch (token_cur->value)
        {
            case ')':
                token_cur = token_cur->next;
                goto finish;
                break;
            case '(':
                token_bak = token_cur;
                token_cur = token_cur->next;
                ret = mls_parse_list(err, &new_ast_node, &token_cur, level + 1);
                if (ret != 0) goto fail;

                if ((new_ast_node_2 = mls_ast_node_new(MLS_AST_NODE_CONS)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }

                if ((new_ast_node_cons = mls_ast_node_cons_new()) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }

                if ((new_ast_node_cons->token = token_clone(token_bak)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }
                if (new_ast_node != NULL) new_ast_node->rcar = new_ast_node_2;
                new_ast_node_cons->car = new_ast_node; new_ast_node = NULL;
                new_ast_node_cons->cdr = NULL; 

                new_ast_node_2->ptr = new_ast_node_cons; new_ast_node_cons = NULL;

                /* Is the head ? */
                if (ast_node_head == NULL)
                {
                    ast_node_head = ast_node_cur = new_ast_node_2;
                }
                else
                {
                    ((struct mls_ast_node_cons *)(ast_node_cur->ptr))->cdr = new_ast_node_2;
                    ast_node_cur = new_ast_node_2;
                }
                new_ast_node_2 = NULL;

                break;
            default:
                token_bak = token_cur;
                /* Create a new atom node */
                if ((new_ast_node = mls_ast_node_new(MLS_AST_NODE_ATOM)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }

                if ((new_ast_node_atom = mls_ast_node_atom_new()) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }
                if ((new_ast_node_atom->atom = token_clone(token_cur)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }

                new_ast_node->ptr = new_ast_node_atom; new_ast_node_atom = NULL;

                if ((new_ast_node_2 = mls_ast_node_new(MLS_AST_NODE_CONS)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }

                if ((new_ast_node_cons = mls_ast_node_cons_new()) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }
                if ((new_ast_node_cons->token = token_clone(token_bak)) == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }
                new_ast_node->rcar = new_ast_node_2;
                new_ast_node_cons->car = new_ast_node; new_ast_node = NULL;
                new_ast_node_cons->cdr = NULL; 

                new_ast_node_2->ptr = new_ast_node_cons; new_ast_node_cons = NULL;

                /* Is the head ? */
                if (ast_node_head == NULL)
                {
                    ast_node_head = ast_node_cur = new_ast_node_2;
                }
                else
                {
                    ((struct mls_ast_node_cons *)(ast_node_cur->ptr))->cdr = new_ast_node_2;
                    ast_node_cur = new_ast_node_2;
                }
                new_ast_node_2 = NULL;
                token_cur = token_cur->next;
                break;
        }
    }

    if (level != 0)
    {
        multiple_error_update(err, -MULTIPLE_ERR_PARSING, \
                "%d:%d: error: expected a \')\' to close \'(\'", token_cur->pos_ln, token_cur->pos_col);
        ret = -MULTIPLE_ERR_PARSING;
        goto fail;
    }

finish:
    *ast_node_out = ast_node_head;
    *token_cur_in_out = token_cur;
    goto done;
fail:
    if (ast_node_head != NULL) mls_ast_node_destroy(ast_node_head);
    if (new_ast_node_cons != NULL) mls_ast_node_cons_destroy(new_ast_node_cons);
    if (new_ast_node_atom != NULL) mls_ast_node_atom_destroy(new_ast_node_atom);
    if (new_ast_node != NULL) mls_ast_node_destroy(new_ast_node);
    if (new_ast_node_2 != NULL) mls_ast_node_destroy(new_ast_node_2);
done:
    return ret;
}

static int mls_parse_program(struct multiple_error *err, struct mls_ast_program **program_out, struct token **token_cur_in_out)
{
    int ret = 0;
    struct token *token_cur = NULL;
    struct mls_ast_program *new_program = NULL;

    if (*token_cur_in_out == NULL) 
    {
        MULTIPLE_ERROR_NULL_PTR();
        ret = -MULTIPLE_ERR_NULL_PTR;
        goto fail;
    }

    token_cur = *token_cur_in_out;
    *program_out = NULL;

    if ((new_program = mls_ast_program_new()) == NULL) 
    {
        MULTIPLE_ERROR_MALLOC();
        return -MULTIPLE_ERR_MALLOC;
    }

    ret = mls_parse_list(err, &(new_program->root), &token_cur, 0);
    if (ret != 0) { goto fail; }

    *program_out = new_program;
    *token_cur_in_out = token_cur;
    goto done;
fail:
    if (new_program != NULL) mls_ast_program_destroy(new_program);
done:
    return ret;
}

int mls_parse( \
        struct multiple_error *err, \
        struct mls_ast_program **program_out, \
        struct token_list *list)
{
    int ret = 0;
	struct mls_ast_program *new_program = NULL;
	struct mls_ast_program *new_program_exp = NULL;
    struct token *token_cur;

    *program_out = NULL;
    token_cur = list->begin;

    /* Do parsing */
    if ((ret = mls_parse_program(err, &new_program, &token_cur)) != 0)
    { goto fail; }

    /* Do macro expantion */
    if ((ret = mls_me(err, \
                    &new_program_exp, \
                    new_program)) != 0)
    { goto fail; }

    *program_out = new_program_exp;
    goto done;
fail:
    if (new_program_exp != NULL) mls_ast_program_destroy(new_program_exp);
done:
    if (new_program != NULL) mls_ast_program_destroy(new_program);
    return ret;
}

