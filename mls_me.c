/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Macro Expander
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "mls_ast.h"

#include "mls_me.h"


/*static int mls_icodegen_me_node( \*/
/*struct multiple_error *err, \*/
/*struct mls_ast_node **ast_node_dst, \*/
/*struct mls_ast_node *ast_node_src)*/
/*{*/
/*int ret = 0;*/
/*struct mls_ast_node *new_ast_node = NULL;*/

/*switch (ast_node_src->type)*/
/*{*/
/*case MLS_AST_NODE_ATOM:*/
/*if ((new_ast_node = mls_ast_node_new(MLS_AST_NODE_ATOM))== NULL) */
/*{ MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }*/
/*if ((new_ast_node->ptr = mls_ast_node_atom_clone(ast_node_src->ptr)) == NULL)*/
/*{ MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }*/
/*break;*/

/*case MLS_AST_NODE_CONS:*/
/*if ((new_ast_node = mls_ast_node_new(MLS_AST_NODE_CONS))== NULL) */
/*{ MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }*/
/*if ((new_ast_node->ptr = mls_ast_node_cons_clone(ast_node_src->ptr))== NULL) */
/*{ MULTIPLE_ERROR_MALLOC(); ret = -MULTIPLE_ERR_MALLOC; goto fail; }*/
/*break;*/
/*}*/

/*if (ast_node_src->rcar != NULL)*/
/*{*/
/*if ((ret = mls_icodegen_me_node( \*/
/*err, \*/
/*new_ast_node->rcar, \*/
/*ast_node->rcar)) != 0)*/
/*{ goto fail; }*/
/*}*/

/**ast_node_dst = new_ast_node;*/

/*goto done;*/
/*fail:*/
/*if (new_ast_node != NULL)*/
/*{*/
/*mls_ast_node_destroy(new_ast_node);*/
/*}*/
/*done:*/
/*return ret;*/
/*}*/


static int mls_icodegen_me_program( \
        struct multiple_error *err, \
        struct mls_ast_program **program_out, \
        struct mls_ast_program *ast_program)
{
    int ret = 0;
	struct mls_ast_program *new_program = NULL;

    if ((new_program = mls_ast_program_new()) == NULL) 
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    if ((new_program->root = mls_ast_node_clone( \
                    ast_program->root)) == NULL)
    { goto fail; }

    *program_out = new_program;

    goto done;
fail:
    if (new_program != NULL)
    {
        mls_ast_program_destroy(new_program);
    }
done:
    return ret;
}

int mls_me( \
        struct multiple_error *err, \
        struct mls_ast_program **program_out, \
        struct mls_ast_program *program_src)
{
    int ret = 0;
	struct mls_ast_program *new_ast_program = NULL;

    if ((ret = mls_icodegen_me_program( \
                    err, \
                    &new_ast_program, \
                    program_src)) != 0)
    { goto fail; }

    *program_out = new_ast_program;

    goto done;
fail:
done:
    return ret;
}

