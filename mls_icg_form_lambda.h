/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Form : lambda
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

#ifndef _MLS_ICG_FORM_LAMBDA_H_
#define _MLS_ICG_FORM_LAMBDA_H_

/* (lambda (args-list) body) */ 
int mls_icodegen_node_form_lambda_style1(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_args_list, \
        struct mls_ast_node *ast_node_body);

/* (lambda (arg) body) */ 
int mls_icodegen_node_form_lambda_style2(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_arg, \
        struct mls_ast_node *ast_node_body);

/* (lambda ? ) */
int mls_icodegen_node_form_lambda(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node);

#endif

