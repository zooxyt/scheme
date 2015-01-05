/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Standard Procedures
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

#ifndef _MLS_ICG_PROC_STD_H_
#define _MLS_ICG_PROC_STD_H_

#include "mls_icg_proc_lut.h"

enum 
{
    MLS_ICG_PROC_TYPE_CONSTRUCTION = 1,
    MLS_ICG_PROC_TYPE_EQU_P,
    MLS_ICG_PROC_TYPE_TYPE_CONVERT,
    MLS_ICG_PROC_TYPE_NUMBERS,
    MLS_ICG_PROC_TYPE_STRINGS,
    MLS_ICG_PROC_TYPE_CHARACTERS,
    MLS_ICG_PROC_TYPE_VECTORS,
    MLS_ICG_PROC_TYPE_SYMBOLS,
    MLS_ICG_PROC_TYPE_PAIRS_LISTS,
    MLS_ICG_PROC_TYPE_IDENTITY_P,
    MLS_ICG_PROC_TYPE_CONTINUATIONS,
    MLS_ICG_PROC_TYPE_ENV,
    MLS_ICG_PROC_TYPE_IO,
    MLS_ICG_PROC_TYPE_SYSTEM,
    MLS_ICG_PROC_TYPE_FUNCTIONAL,
    MLS_ICG_PROC_TYPE_BOOLEANS,
};

enum 
{
    /* Strings */
    MLS_ICG_STRINGP = 0, 
    MLS_ICG_MAKE_STRING, 
    MLS_ICG_STRING, 
    MLS_ICG_STRING_LENGTH, 
    MLS_ICG_STRING_REF, 
    MLS_ICG_STRING_SET, 
    MLS_ICG_STRING_EQP, MLS_ICG_STRING_CI_EQP, 
    MLS_ICG_STRING_LP, MLS_ICG_STRING_CI_LP, 
    MLS_ICG_STRING_LEP, MLS_ICG_STRING_CI_LEP, 
    MLS_ICG_STRING_GP, MLS_ICG_STRING_CI_GP, 
    MLS_ICG_STRING_GEP, MLS_ICG_STRING_CI_GEP, 
    MLS_ICG_SUBSTRING,
    MLS_ICG_STRING_APPEND, 
    MLS_ICG_STRING_COPY, 
    MLS_ICG_STRING_FILL, 

    /* Characters */
    MLS_ICG_CHARP, 
    MLS_ICG_CHAR_EQP, MLS_ICG_CHAR_CI_EQP, 
    MLS_ICG_CHAR_LP, MLS_ICG_CHAR_CI_LP, 
    MLS_ICG_CHAR_LEP, MLS_ICG_CHAR_CI_LEP,
    MLS_ICG_CHAR_GP, MLS_ICG_CHAR_CI_GP, 
    MLS_ICG_CHAR_GEP, MLS_ICG_CHAR_CI_GEP,
    MLS_ICG_CHAR_ALPHABETICP, MLS_ICG_CHAR_NUMERIC, MLS_ICG_CHAR_WHITESPACEP, 
    MLS_ICG_CHAR_UPPER_CASEP, MLS_ICG_CHAR_LOWER_CASEP, 
    MLS_ICG_CHAR_TO_INTEGER, MLS_ICG_INTEGER_TO_CHAR, 
    MLS_ICG_CHAR_UPCASE, MLS_ICG_CHAR_DOWNCASE, 

    /* Vectors */
    MLS_ICG_MAKE_VECTOR, MLS_ICG_VECTOR, MLS_ICG_VECTORP, MLS_ICG_VECTOR_LENGTH,
    MLS_ICG_VECTOR_REF, MLS_ICG_VECTOR_SET, 
    MLS_ICG_VECTOR_VECTOR_TO_LIST, MLS_ICG_VECTOR_LIST_TO_VECTOR, 
    MLS_ICG_VECTOR_VECTOR_FILL, 

    /* Symbols */
    MLS_ICG_SYMBOL_TO_STRING, MLS_ICG_STRING_TO_SYMBOL, MLS_ICG_SYMBOLP, 

    /* Pairs & Lists */
    MLS_ICG_PAIRP, MLS_ICG_CONS, MLS_ICG_CAR, MLS_ICG_CDR, MLS_ICG_SET_CAR, MLS_ICG_SET_CDR, 
    MLS_ICG_NULLP, MLS_ICG_LISTP, MLS_ICG_LIST, 
    MLS_ICG_LENGTH, MLS_ICG_APPEND, MLS_ICG_REVERSE, 
    MLS_ICG_LIST_TAIL, MLS_ICG_LIST_REF, 
    MLS_ICG_MEMQ, MLS_ICG_MEMV, MLS_ICG_MEMBER, MLS_ICG_ASSQ, MLS_ICG_ASSV, MLS_ICG_ASSOC, 
    MLS_ICG_LIST_TO_STRING, MLS_ICG_STRING_TO_LIST, 

    /* Continuations */
    MLS_ICG_CALL_WITH_CURRENT_CONTINUATION, 
    MLS_ICG_CALL_CC, 
    MLS_ICG_VALUES, 
    MLS_ICG_CALL_WITH_VALUES,
    MLS_ICG_DYNAMIC_WIND,

    /* Functional */
    MLS_ICG_PROCEDUREP, MLS_ICG_APPLY, MLS_ICG_MAP, MLS_ICG_FOR_EACH, 

    /* Booleans */
    MLS_ICG_BOOLEANP, MLS_ICG_NOT,
};

static const struct mls_icg_proc_lut_item mls_icg_proc_lut_items_std[] =
{
    /* Strings */
    { MLS_ICG_STRINGP       , "string?"       , 7  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_MAKE_STRING   , "make-string"   , 11 , 1 , 2 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING        , "string"        , 6  , 0 ,-1 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_LENGTH , "string-length" , 13 , 1 , 1 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_REF    , "string-ref"    , 10 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_SET    , "string-set!"   , 10 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_EQP    , "string=?"      , 8  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_CI_EQP , "string-ci=?"   , 11 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_LP     , "string<?"      , 8  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_CI_LP  , "string-ci<?"   , 11 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_LEP    , "string<=?"     , 9  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_CI_LEP , "string-ci<=?"  , 12 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_GP     , "string>?"      , 8  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_CI_GP  , "string-ci>?"   , 11 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_GEP    , "string>=?"     , 9  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_CI_GEP , "string-ci>=?"  , 12 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_SUBSTRING     , "substring"     , 9  , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_APPEND , "string-append" , 13 , 0 ,-1 , MLS_ICG_PROC_OP_CALL   , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_COPY   , "string-copy"   , 11 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,
    { MLS_ICG_STRING_FILL   , "string-fill"   , 11 , 1 , 1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_STRINGS } ,

    /* Characters */
    { MLS_ICG_CHARP                 , "char?"         , 5  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_EQP              , "char=?"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_CI_EQP           , "char-ci=?"     , 9  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_LP               , "char<?"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_CI_LP            , "char-ci<?"     , 9  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_LEP              , "char<=?"       , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_CI_LEP           , "char-ci<=?"    , 9  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_GP               , "char>?"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_CI_GP            , "char-ci>?"     , 9  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_GEP              , "char>=?"       , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_CI_GEP           , "char-ci>=?"    , 9  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_ALPHABETICP      , "char-alphabetic?", 16 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_NUMERIC          , "char-numeric?" , 13 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_WHITESPACEP      , "char-whitespace?", 16 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_UPPER_CASEP      , "char-upper-case?", 16 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_LOWER_CASEP      , "char-lower-case?", 16 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_TO_INTEGER       , "char->integer" , 13 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_INTEGER_TO_CHAR       , "integer->char" , 13 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_UPCASE           , "char-upcase"   , 11 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,
    { MLS_ICG_CHAR_DOWNCASE         , "char-downcase" , 13 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CHARACTERS } ,

    /* Vectors */
    { MLS_ICG_MAKE_VECTOR           , "make-vector"   , 11 , 1 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR                , "vector"        , 6  , 0 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTORP               , "vector?"       , 7  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_LENGTH         , "vector-length" , 13 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_REF            , "vector-ref"    , 10 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_SET            , "vector-set!"   , 11 , 3 , 3  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_VECTOR_TO_LIST , "vector->list"  , 12 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_LIST_TO_VECTOR , "list->vector"  , 12 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,
    { MLS_ICG_VECTOR_VECTOR_FILL    , "vector-fill!"  , 12 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_VECTORS } ,

    /* Symbol */
    { MLS_ICG_SYMBOL_TO_STRING      , "symbol->string", 14 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_SYMBOLS } ,
    { MLS_ICG_STRING_TO_SYMBOL      , "string->symbol", 14 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_SYMBOLS } ,
    { MLS_ICG_SYMBOLP               , "symbol?"       ,  7 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_SYMBOLS } ,

    /* Pairs and Lists */
    { MLS_ICG_PAIRP                 , "pair?"         , 5  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_CONS                  , "cons"          , 4  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_CAR                   , "car"           , 3  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_CDR                   , "cdr"           , 3  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_SET_CAR               , "set-car!"      , 8  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_SET_CDR               , "set-cdr!"      , 8  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_NULLP                 , "null?"         , 5  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LISTP                 , "list?"         , 5  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LIST                  , "list"          , 4  , 0 , -1 , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LENGTH                , "length"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_APPEND                , "append"        , 6  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_REVERSE               , "reverse"       , 7  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LIST_TAIL             , "list-tail"     , 9  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LIST_REF              , "list-ref"      , 8  , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_MEMQ                  , "memq"          , 4  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_MEMV                  , "memv"          , 4  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_MEMBER                , "member"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_ASSQ                  , "assq"          , 4  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_ASSV                  , "assv"          , 4  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_ASSOC                 , "assoc"         , 5  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_LIST_TO_STRING        , "list->string"  , 12 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,
    { MLS_ICG_STRING_TO_LIST        , "string->list"  , 12 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_PAIRS_LISTS } ,

    /* Continuations */
    { MLS_ICG_CALL_WITH_CURRENT_CONTINUATION, "call-with-current-continuation", 30  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CONTINUATIONS } ,
    { MLS_ICG_CALL_CC               , "call/cc"       , 7  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CONTINUATIONS } ,
    { MLS_ICG_VALUES                , "values"        , 6  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CONTINUATIONS } ,
    { MLS_ICG_CALL_WITH_VALUES      , "call-with-values", 16  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CONTINUATIONS } ,
    { MLS_ICG_DYNAMIC_WIND          , "dynamic-wind"  , 12  , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_CONTINUATIONS } ,

    /* Functional */
    { MLS_ICG_PROCEDUREP            , "procedure?"    , 10 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_FUNCTIONAL } ,
    { MLS_ICG_APPLY                 , "apply"         ,  5 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_FUNCTIONAL } ,
    { MLS_ICG_MAP                   , "map"           ,  3 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_FUNCTIONAL } ,
    { MLS_ICG_FOR_EACH              , "for-each"      ,  8 , 2 , 2  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_FUNCTIONAL } ,

    /* Boolean */
    { MLS_ICG_BOOLEANP              , "boolean?"      ,  8 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_BOOLEANS } ,
    { MLS_ICG_NOT                   , "not"           ,  3 , 1 , 1  , MLS_ICG_PROC_OP_MANUAL , MLS_ICG_PROC_TYPE_BOOLEANS } ,

    /*{ MLS_ICG_MAKE_STRING        , "make-string"      , 11 , 0 , -1 , MLS_ICG_PROC_OP_MANUAL    , MLS_ICG_PROC_TYPE_CONSTRUCTION }    ,*/
    /*{ MLS_ICG_LIST               , "list"             , 4  , 0 , -1 , MLS_ICG_PROC_OP_MANUAL    , MLS_ICG_PROC_TYPE_CONSTRUCTION }    ,*/
};
#define MLS_PROC_LUT_ITEMS_STD_COUNT ((sizeof(mls_icg_proc_lut_items_std))/(sizeof(struct mls_icg_proc_lut_item)))

int mls_icodegen_node_proc_std(struct multiple_error *err, \
        struct mls_icg_context *context, \
        struct mls_icg_fcb_block *icg_fcb_block, \
        struct mls_ast_node *ast_node_proc, \
        struct mls_ast_node *ast_node_args, \
        const int procedure_idx);

#endif

