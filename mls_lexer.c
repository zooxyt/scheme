/* Multiple Scheme Programming Language : Lexical Scanner
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

/* Reference Documents :
 * R5RS Standard : http://www.schemers.org/Documents/Standards/R5RS/
 * <<The Scheme Programming Language>> 
 *   by R. Kent Dybvig : http://www.scheme.com/tspl4/
 */

/* disable conditional expression is constant warning */
#ifdef _MSC_VER
#pragma warning( disable: 4127 ) 
#endif

#include <stdlib.h>
#include <string.h>

#include "multiple_err.h"

#include "multiply_lexer.h"

#include "mls_lexer.h"

static struct lexical_item mls_char_tokens[] = 
{
    /* value, literal, len */
    {'('                    , "("   , 1} ,
    {')'                    , ")"   , 1} ,
    {'`'                    , "`"   , 1} ,
    {'\''                   , "\'"  , 1} ,
    {','                    , ","   , 1} ,
    {'.'                    , "."   , 1} ,
};
#define MLS_CHAR_TOKENS_COUNT (sizeof(mls_char_tokens)/sizeof(struct lexical_item))

enum {
    LEX_STATUS_INIT = 0,

    LEX_STATUS_COMMENT, /* #.*?<EOL> */
    LEX_STATUS_EOL, /* EOL of Windows? Mac? */
    LEX_STATUS_IDENTIFIER_P_1, /* Identifier ? */
    LEX_STATUS_STRING, /* " */
    LEX_STATUS_HASH, /* Hash */
    LEX_STATUS_CHARACTER_P_1, /* #\ */

    LEX_STATUS_MINUS, /* - */

    LEX_STATUS_INTEGER_BINARY, /* #b??? */
    LEX_STATUS_INTEGER_OCTAL, /* #o??? */
    LEX_STATUS_INTEGER_DECIMAL, /* #d??? | ??? */
    LEX_STATUS_INTEGER_HEXADECIMAL, /* #h??? */

    LEX_STATUS_INTEGER_BINARY_1, /* Numbers' at least one numeric version */
    LEX_STATUS_INTEGER_OCTAL_1,
    LEX_STATUS_INTEGER_DECIMAL_1,
    LEX_STATUS_INTEGER_HEXADECIMAL_1,

    LEX_STATUS_FLOAT_DOT_B, /* #b???.??? */
    LEX_STATUS_FLOAT_DOT_O, /* #o???.??? */
    LEX_STATUS_FLOAT_DOT_D, /* .??? | #d???.??? */
    LEX_STATUS_FLOAT_DOT_H, /* #x???.??? */

    LEX_STATUS_BACK_FINISH, /* Finished, and break */
    LEX_STATUS_FINISH, /* Finished */
    LEX_STATUS_ERROR, /* Error */
};

#define JMP(status,dst) do{(status)=(dst);}while(0);
#define FIN(x) do{(x)=LEX_STATUS_FINISH;}while(0);
#define BFIN(x) do{(x)=LEX_STATUS_BACK_FINISH;}while(0);
#define UND(x) do{(x)=LEX_STATUS_ERROR;}while(0);
#define KEEP() do{}while(0);

#define ENOUGH_SPACE(p, endp, delta) ((((size_t)((endp)-(p)))>=((size_t)delta))?(1):(0))
#define MATCH(symbol,symbol_len,p,endp) ((ENOUGH_SPACE(p,endp,symbol_len)!=0)&&(memcmp(symbol,p,symbol_len)==0))

/* Get one token from the char stream */
static int eat_token(struct multiple_error *err, struct token *new_token, const char *p, const char *endp, uint32_t *pos_col, uint32_t *pos_ln, const int eol_type, size_t *move_on)
{
    int ret = 0;
    const char *p_init = p;
    int status = LEX_STATUS_INIT;
    int ch = 0;
    size_t idx = 0;
    size_t bytes_number;
    size_t prefix_strip = 0, postfix_strip = 0;

    int is_eol = 0; /* For updating EOL and Ln */

    /* Clean template */
    new_token->value = TOKEN_UNDEFINED;
    new_token->str = (char *)p_init;
    new_token->len = 0;
    new_token->pos_col = *pos_col;
    new_token->pos_ln = *pos_ln;

    while (p != endp)
    {
        ch = *p;
        switch (status)
        {
            case LEX_STATUS_EOL:
                if (ch == CHAR_LF) { FIN(status); } else { BFIN(status); }
                break;
            case LEX_STATUS_COMMENT:
                if (IS_EOL(ch)) 
                {
                    /* Reset location */
                    *pos_col = 1;
                    *pos_ln += 1;
                    is_eol = 1;
                    /* "" (Null String) */
                    new_token->value = TOKEN_WHITESPACE;
                    FIN(status);
                }
                else
                {
                    KEEP();
                }
                break;
            case LEX_STATUS_INIT:
                if (IS_EOL(ch)) 
                {
                    /* Reset location */
                    *pos_col = 1;
                    *pos_ln += 1;
                    is_eol = 1;

                    new_token->value = TOKEN_WHITESPACE; 
                    switch (eol_type)
                    {
                        case EOL_UNIX:
                        case EOL_MAC:
                            FIN(status);
                            break;
                        case EOL_DOS:
                            JMP(status, LEX_STATUS_EOL);
                            break;
                    }
                }

                /* Comments */
                if ((ch == ';')||((ch == '#') && (*pos_col == 1) && (*pos_ln == 1)))
                {
                    JMP(status, LEX_STATUS_COMMENT);
                    goto finish_init;
                }

                /* Whitespace */
                if (IS_MLS_WHITESPACE(ch))
                {
                    new_token->value = TOKEN_WHITESPACE; FIN(status);
                    goto finish_init;
                }

                /* Hash */
                if (ch == '#') { JMP(status, LEX_STATUS_HASH); goto finish_init; }

                /* Minus */
                if (ch == '-') { JMP(status, LEX_STATUS_MINUS); goto finish_init; }

                /* Decimal Float */
                if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_D); goto finish_init; }

                /* Integer */
                if (IS_MLS_DIGIT_10(ch)) { JMP(status, LEX_STATUS_INTEGER_DECIMAL_1); goto finish_init; }

                /* Char Tokens */
                for (idx = 0; idx != MLS_CHAR_TOKENS_COUNT; idx++)
                {
                    if (MATCH(mls_char_tokens[idx].literal,mls_char_tokens[idx].len,p,endp) != 0)
                    {
                        new_token->value = mls_char_tokens[idx].value; 
                        FIN(status);
                        p += (mls_char_tokens[idx].len - 1);
                        goto finish_init;
                    }
                }

                /* String */
                if (ch == '\"')
                {
                    JMP(status, LEX_STATUS_STRING);
                    goto finish_init;
                }

                if (IS_MLS_INITIAL(ch)||IS_MLS_PECULIA_IDENTIFIER(ch))
                {
                    /* Identifier ? */
                    new_token->value = TOKEN_IDENTIFIER;
                    JMP(status, LEX_STATUS_IDENTIFIER_P_1);
                    goto finish_init;
                }

                if (IS_ID_HYPER(ch)) 
                {
                    bytes_number = id_hyper_length((char)ch);
                    if ((bytes_number == 0) || ((size_t)(endp - p) < bytes_number))
                    {
                        MULTIPLE_ERROR_INTERNAL();
                        return -MULTIPLE_ERR_LEXICAL;
                    }
                    bytes_number--;
                    while (bytes_number-- != 0) { p += 1; }
                    /* Identifier ? */
                    new_token->value = TOKEN_IDENTIFIER;
                    JMP(status, LEX_STATUS_IDENTIFIER_P_1);
                    goto finish_init;
                }

                /* Undefined */
                {new_token->value = TOKEN_UNDEFINED; UND(status);} /* Undefined! */
finish_init:
                break;
            case LEX_STATUS_IDENTIFIER_P_1:
                if (IS_MLS_SUBSEQUENT(ch)||IS_MLS_PECULIA_IDENTIFIER(ch)) {KEEP();}
                else if (IS_ID_HYPER(ch)) 
                {
                    bytes_number = id_hyper_length((char)ch);
                    if ((bytes_number == 0) || ((size_t)(endp - p) < bytes_number))
                    {
                        MULTIPLE_ERROR_INTERNAL();
                        return -MULTIPLE_ERR_LEXICAL;
                    }
                    bytes_number--;
                    while (bytes_number-- != 0) { p += 1; }
                }
                else {new_token->value = TOKEN_IDENTIFIER; BFIN(status);} /* Identifier! */
                break;
            case LEX_STATUS_HASH:
                if (ch == '\\') { JMP(status, LEX_STATUS_CHARACTER_P_1); goto finish_init; }
                else if (ch == 'f') {new_token->value = TOKEN_CONSTANT_FALSE; FIN(status);} /* False  */
                else if (ch == 't') {new_token->value = TOKEN_CONSTANT_TRUE; FIN(status);} /* True  */
                else if (ch == 'b') { JMP(status, LEX_STATUS_INTEGER_BINARY); goto finish_init; }
                else if (ch == 'o') { JMP(status, LEX_STATUS_INTEGER_OCTAL); goto finish_init; }
                else if (ch == 'x') { JMP(status, LEX_STATUS_INTEGER_HEXADECIMAL); goto finish_init; }
                else {new_token->value = TOKEN_UNDEFINED; UND(status);} /* Undefined! */
                break;

            case LEX_STATUS_MINUS:
                /* Decimal Float */
                if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_D); goto finish_init; }
                /* Integer */
                else if (IS_MLS_DIGIT_10(ch)) { JMP(status, LEX_STATUS_INTEGER_DECIMAL_1); goto finish_init; }
                else {new_token->value = TOKEN_IDENTIFIER; BFIN(status);} /* Identifier! */

                break;

            case LEX_STATUS_INTEGER_BINARY:
                if (IS_MLS_DIGIT_2(ch)) { JMP(status, LEX_STATUS_INTEGER_BINARY_1); goto finish_init; }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_B); goto finish_init; }
                else 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: no binary digits", *pos_ln, *pos_col);
                    ret = -MULTIPLE_ERR_LEXICAL;
                    goto fail;
                }
                break;
            case LEX_STATUS_INTEGER_BINARY_1:
                if (IS_MLS_DIGIT_2(ch)) { KEEP(); }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_B); goto finish_init; }
                else {new_token->value = TOKEN_CONSTANT_INTEGER_BINARY; BFIN(status);} /* Binary! */
                break;
            case LEX_STATUS_INTEGER_OCTAL:
                if (IS_MLS_DIGIT_8(ch)) { JMP(status, LEX_STATUS_INTEGER_OCTAL_1); goto finish_init; }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_O); goto finish_init; }
                else 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: no octal digits", *pos_ln, *pos_col);
                    ret = -MULTIPLE_ERR_LEXICAL;
                    goto fail;
                }
                break;
            case LEX_STATUS_INTEGER_OCTAL_1:
                if (IS_MLS_DIGIT_8(ch)) { KEEP(); }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_O); goto finish_init; }
                else {new_token->value = TOKEN_CONSTANT_INTEGER_OCTAL; BFIN(status);} /* Octal! */
                break;
            case LEX_STATUS_INTEGER_DECIMAL:
                if (IS_MLS_DIGIT_10(ch)) { JMP(status, LEX_STATUS_INTEGER_DECIMAL_1); goto finish_init; }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_D); goto finish_init; }
                else 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: no decimal digits", *pos_ln, *pos_col);
                    ret = -MULTIPLE_ERR_LEXICAL;
                    goto fail;
                }
                break;
            case LEX_STATUS_INTEGER_DECIMAL_1:
                if (IS_MLS_DIGIT_10(ch)) { KEEP(); }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_D); goto finish_init; }
                else {new_token->value = TOKEN_CONSTANT_INTEGER_DECIMAL; BFIN(status);} /* Decimal! */
                break;
            case LEX_STATUS_INTEGER_HEXADECIMAL:
                if (IS_MLS_DIGIT_16(ch)) { JMP(status, LEX_STATUS_INTEGER_HEXADECIMAL_1); goto finish_init; }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_H); goto finish_init; }
                else 
                {
                    multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: no hexadecimal digits", *pos_ln, *pos_col);
                    ret = -MULTIPLE_ERR_LEXICAL;
                    goto fail;
                }
                break;
            case LEX_STATUS_INTEGER_HEXADECIMAL_1:
                if (IS_MLS_DIGIT_16(ch)) { KEEP(); }
                else if (ch == '.') { JMP(status, LEX_STATUS_FLOAT_DOT_H); goto finish_init; }
                else {new_token->value = TOKEN_CONSTANT_INTEGER_HEXADECIMAL; BFIN(status);} /* Hexadecimal! */
                break;

            case LEX_STATUS_FLOAT_DOT_B: /* #b???.??? */
                if (IS_MLS_DIGIT_2(ch)) { KEEP(); }
                else {new_token->value = TOKEN_CONSTANT_FLOAT_BINARY; BFIN(status);} /* Binary! */
                break;
            case LEX_STATUS_FLOAT_DOT_O: /* #o???.??? */
                if (IS_MLS_DIGIT_8(ch)) { KEEP(); }
                else {new_token->value = TOKEN_CONSTANT_FLOAT_OCTAL; BFIN(status);} /* Octal! */
                break;
            case LEX_STATUS_FLOAT_DOT_D: /* .??? | #d???.??? */
                if (IS_MLS_DIGIT_10(ch)) { KEEP(); }
                else {new_token->value = TOKEN_CONSTANT_FLOAT_DECIMAL; BFIN(status);} /* Decimal! */
                break;
            case LEX_STATUS_FLOAT_DOT_H: /* #x???.??? */
                if (IS_MLS_DIGIT_16(ch)) { KEEP(); }
                else {new_token->value = TOKEN_CONSTANT_FLOAT_HEXADECIMAL; BFIN(status);} /* Hexadecimal! */
                break;

            case LEX_STATUS_CHARACTER_P_1:
                if (IS_MLS_DELIMITER(ch)) 
                {
                    prefix_strip = 2;

                    new_token->value = TOKEN_CONSTANT_CHARACTER; BFIN(status); 
                }
                else if (IS_ID_HYPER(ch)) 
                {
                    prefix_strip = 2;

                    bytes_number = id_hyper_length((char)ch);
                    if ((bytes_number == 0) || ((size_t)(endp - p) < bytes_number))
                    {
                        MULTIPLE_ERROR_INTERNAL();
                        return -MULTIPLE_ERR_LEXICAL;
                    }
                    while (bytes_number-- != 0) { p += 1; }

                    new_token->value = TOKEN_CONSTANT_CHARACTER; BFIN(status);  
                }
                else { KEEP(); }
                break;
            case LEX_STATUS_STRING:
                /* "<- */
                if (ch == '\"')
                {
                    /* "" (Null String) */
                    prefix_strip = 1;
                    postfix_strip = 1;
                    new_token->value = TOKEN_CONSTANT_STRING;
                    FIN(status);
                }
                else
                {
                    KEEP();
                }
                break;
            case LEX_STATUS_ERROR:
                new_token->str = NULL;
                new_token->len = 0;
                multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: undefined token", *pos_ln, *pos_col);
                ret = -MULTIPLE_ERR_LEXICAL;
                goto fail;
                break;
            case LEX_STATUS_BACK_FINISH:
                p--;
            case LEX_STATUS_FINISH:
                goto done;
                break;
            default:
                new_token->str = NULL;
                new_token->len = 0;
                multiple_error_update(err, -MULTIPLE_ERR_LEXICAL, "%d:%d: error: undefined lexical analysis state, something impossible happened", *pos_ln, *pos_col);
                ret = -MULTIPLE_ERR_LEXICAL;
                goto fail;
                break;
        }
        if (status == LEX_STATUS_BACK_FINISH) break;
        p += 1;
    }
done:
fail:
    if (!is_eol)
    {
        *pos_col += (uint32_t)(p - p_init);
    }
    if (new_token->value == TOKEN_UNDEFINED)
    {
        new_token->len = 0;
        *move_on = new_token->len;
    }
    else
    {
        new_token->len = (size_t)(p - p_init);
        *move_on = new_token->len;
        new_token->str += prefix_strip;
        new_token->len -= (size_t)(prefix_strip + postfix_strip);
    }
    return ret;
}

/* Tokenize source code with a lexical scanner */
int mls_tokenize(struct multiple_error *err, struct token_list **list_out, const char *data, const size_t data_len)
{
    int ret = 0;
    uint32_t pos_col = 1, pos_ln = 1;
    struct token_list *new_list = NULL;
    struct token *token_template = NULL;
    const char *data_p = data, *data_endp = data_p + data_len;
    size_t move_on;

    int eol_type = eol_detect(err, data, data_len);

    *list_out = NULL;

    if ((new_list = token_list_new()) == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    if ((token_template = token_new()) == NULL)
    {
        MULTIPLE_ERROR_MALLOC();
        ret = -MULTIPLE_ERR_MALLOC;
        goto fail;
    }

    while (data_p != data_endp)
    {
        if ((ret = eat_token(err, token_template, data_p, data_endp, &pos_col, &pos_ln, eol_type, &move_on)) != 0)
        {
            goto fail;
        }
        if (token_template->value != TOKEN_WHITESPACE)
        {
            if ((ret = token_list_append_token_with_template(new_list, token_template)) != 0)
            {
                goto fail;
            }
        }
        /* Move on */
        data_p += move_on;
    }
    ret = token_list_append_token_with_configure(new_list, TOKEN_FINISH, NULL, 0, pos_col, pos_ln);
    if (ret != 0) goto fail;

    *list_out = new_list;
    ret = 0;
fail:
    if (token_template != NULL)
	{
		token_template->str = NULL;
		free(token_template);
	}
    if (ret != 0)
    {
        if (new_list != NULL) token_list_destroy(new_list);
    }
    return ret;
}

