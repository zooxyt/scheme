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

#include "selfcheck.h"

#include <stdlib.h>

#include "multiple.h"
#include "multiple_err.h"

#include "mls_lexer.h"
#include "mls_parser.h"
#include "mls_icg.h"
#include "scheme_stub.h"


int mls_stub_create(struct multiple_error *err, void **stub_out, \
        char *pathname_dst, int type_dst, \
        char *pathname_src, int type_src)
{
    int ret = 0;
    struct mls_stub *new_stub = NULL;
    FILE *fp_src;
    long size_fp;

    (void)pathname_dst;
    (void)type_dst;
    *stub_out = NULL;

    if ((new_stub = (struct mls_stub *)malloc(sizeof(struct mls_stub))) == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }
    new_stub->tokens = NULL;
    new_stub->program = NULL;
    new_stub->code = NULL;
    new_stub->len = 0;
    new_stub->debug_info = 0;

    new_stub->opt_internal_reconstruct = 0;

    if (pathname_src == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        ret = -MULTIPLE_ERR_NULL_PTR;
        goto fail;
    }
    else
    {
        switch (type_src)
        {
            case MULTIPLE_IO_PATHNAME:
                /* Open source code file */
                fp_src = fopen(pathname_src, "rb");
                if (fp_src == NULL) 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_STUB, "error: can not open file %s for reading", pathname_src);
                    ret = -MULTIPLE_ERR_STUB;
                    goto fail;
                }
                /* Get file length */
                fseek(fp_src, 0, SEEK_END);
                size_fp = ftell(fp_src);
                fseek(fp_src, 0, SEEK_SET);
                /* Allocate space */
                new_stub->code = (char *)malloc(sizeof(char) * (size_t)size_fp);
                if (new_stub->code == NULL)
                {
                    MULTIPLE_ERROR_MALLOC();
                    ret = -MULTIPLE_ERR_MALLOC;
                    goto fail;
                }
                /* Read file */
                if (fread(new_stub->code, (size_t)size_fp, 1, fp_src) < 1) 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_STUB, "error: reading data from %s failed", pathname_src);
                    ret = -MULTIPLE_ERR_STUB;
                    goto fail;
                }
                fclose(fp_src);

                new_stub->len = (size_t)size_fp;
                break;
            default:
                MULTIPLE_ERROR_INTERNAL();
                ret = -MULTIPLE_ERR_STUB;
                goto fail;
                break;
        }
    }

    *stub_out = new_stub;
    ret = 0;
    goto done;
fail:
    if (new_stub != NULL)
    {
        if (new_stub->code != NULL) free(new_stub->code);
        free(new_stub);
    }
done:
    return ret;
}

int mls_stub_destroy(void *stub)
{
    struct mls_stub *stub_ptr = (struct mls_stub *)stub;

    if (stub_ptr == NULL)
    {
        return -MULTIPLE_ERR_NULL_PTR;
    }
    if (stub_ptr->program != NULL) mls_ast_program_destroy(stub_ptr->program);
    if (stub_ptr->tokens != NULL) token_list_destroy(stub_ptr->tokens);
    if (stub_ptr->code != NULL) free(stub_ptr->code);
    free(stub_ptr);

    return 0;
}

int mls_stub_debug_info_set(void *stub, int debug_info)
{
    struct mls_stub *stub_ptr = (struct mls_stub *)stub;
    stub_ptr->debug_info = debug_info;
    return 0;
}

static int mls_stub_tokenize(struct multiple_error *err, struct mls_stub *stub)
{
    int ret = 0;

    if (stub == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }
    /* clean */
    if (stub->tokens != NULL) 
    {
        if ((ret = token_list_destroy(stub->tokens)) != 0) return ret;
        stub->tokens = NULL;
    }
    /* construct */
    if ((ret = mls_tokenize(err, &stub->tokens, stub->code, stub->len)) != 0) return ret;

    return ret;
}

static int mls_stub_parse(struct multiple_error *err, struct mls_stub *stub)
{
    int ret = 0;

    if (stub == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }
    /* dependence */
    if (stub->tokens == NULL)
    {
        if ((ret = mls_stub_tokenize(err, stub)) != 0) return ret;
    }
    /* clean */
    if (stub->program != NULL)
    {
        if ((ret = mls_ast_program_destroy(stub->program)) != 0) return ret;
        stub->program = NULL;
    }
    /* construct */
    if ((ret = mls_parse(err, &stub->program, stub->tokens)) != 0) return ret;

    return ret;
}

int mls_stub_irgen(struct multiple_error *err, struct multiple_ir **ir, void *stub)
{
    struct mls_stub *stub_ptr = (struct mls_stub *)stub;
    int ret = 0;

    if (stub_ptr == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }
    /* dependence */
    if (stub_ptr->program == NULL)
    {
        if ((ret = mls_stub_parse(err, stub_ptr)) != 0) return ret;
    }
    /* clean */
    if (*ir != NULL)
    {
        if ((ret = multiple_ir_destroy(*ir)) != 0) return ret;
        *ir = NULL;
    }
    /* construct */
    if ((ret = mls_irgen(err, ir, stub_ptr->program, stub_ptr->opt_internal_reconstruct)) != 0) return ret;
    stub_ptr->opt_internal_reconstruct = 0;

    return ret;
}

int mls_stub_reconstruct(struct multiple_error *err, struct multiple_ir **ir, void *stub)
{
    int ret = 0;
    struct mls_stub *stub_ptr = (struct mls_stub *)stub;

    if (stub == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }

    /* dependence */
    if (stub_ptr->program == NULL)
    {
        if ((ret = mls_stub_parse(err, stub_ptr)) != 0) return ret;
    }
    /* clean */
    if (*ir != NULL)
    {
        if ((ret = multiple_ir_destroy(*ir)) != 0) return ret;
        *ir = NULL;
    }
    /* construct */
    stub_ptr->opt_internal_reconstruct = 1;
    if ((ret = mls_irgen(err, ir, stub_ptr->program, stub_ptr->opt_internal_reconstruct)) != 0) return ret;

    return ret;
}

static int mls_internal_tokens_print(struct token_list *list)
{
    int ret;
    ret = token_list_walk(list);
    return ret;
}

int mls_stub_tokens_print(struct multiple_error *err, void *stub)
{
    int ret = 0;
    struct mls_stub *stub_ptr = (struct mls_stub *)stub;

    if (stub == NULL)
    {
        MULTIPLE_ERROR_NULL_PTR();
        return -MULTIPLE_ERR_NULL_PTR;
    }

    /* dependence */
    if (stub_ptr->tokens == NULL) 
    {
        if ((ret = mls_tokenize(err, &stub_ptr->tokens, stub_ptr->code, stub_ptr->len)) != 0) return ret;
    }
    /* work */
    if ((ret = mls_internal_tokens_print(stub_ptr->tokens)) != 0) return ret;

    return ret;
}


