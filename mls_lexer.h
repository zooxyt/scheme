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

/* R5RS Standard
 * Section 7.1.1 Lexical structure */
#ifndef _MLS_LEXER_H_
#define _MLS_LEXER_H_

#include <stdio.h>

#include "multiply_lexer.h"

/* Token Types */
enum
{
    TOKEN_UNKNOWN = CUSTOM_TOKEN_STARTPOINT,
};

#define IS_MLS_WHITESPACE(x) \
    (((x)==' ')||((x)=='\r')||((x)=='\n'))
#define IS_MLS_LETTER(x) ((('a'<=(x))&&((x)<='z'))||(('A'<=(x))&&((x)<='Z')))
#define IS_MLS_SPECIAL_INITIAL(x) \
    (((x)=='!')||((x)=='$')||((x)=='%')||((x)=='&')||((x)=='*')||\
     ((x)=='/')||((x)==':')||((x)=='<')||((x)=='=')||((x)=='>')||\
     ((x)=='?')||((x)=='^')||((x)=='_')||((x)=='~'))
#define IS_MLS_INITIAL(x) \
    (IS_MLS_LETTER(x)||IS_MLS_SPECIAL_INITIAL(x))
#define IS_MLS_PECULIA_IDENTIFIER(x) \
    (((x)=='+')||((x)=='-'))
#define IS_MLS_DIGIT(x) \
    (('0'<=(x))&&((x)<='9'))
#define IS_MLS_DIGIT_2(x) \
    (('0'==(x))||((x)=='1'))
#define IS_MLS_DIGIT_8(x) \
    (('0'<=(x))&&((x)<='7'))
#define IS_MLS_DIGIT_10(x) \
    (('0'<=(x))&&((x)<='9'))
#define IS_MLS_DIGIT_16(x) \
    ((('0'<=(x))&&((x)<='9'))||(('a'<=(x))&&((x)<='f'))||(('A'<=(x))&&((x)<='F')))
#define IS_MLS_SPECIAL_SUBSEQUENT(x) \
    (((x)=='+')||((x)=='-')||((x)=='.')||((x)=='@'))
#define IS_MLS_SUBSEQUENT(x) \
    (IS_MLS_INITIAL(x)||IS_MLS_DIGIT(x)||IS_MLS_SPECIAL_SUBSEQUENT(x))
#define IS_MLS_DELIMITER(x) \
    (IS_MLS_WHITESPACE(x)||(x)=='('||(x)==')'||(x)=='\"'||(x)==';')

int mls_tokenize(struct multiple_error *err, struct token_list **list_out, const char *data, const size_t data_len);

#endif

