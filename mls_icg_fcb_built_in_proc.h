/* Multiple Scheme Programming Language : Intermediate Code Generator
 * Built-in procedures
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

#ifndef _MLS_ICG_FCB_BUILT_IN_PROC_H_
#define _MLS_ICG_FCB_BUILT_IN_PROC_H_

static const char *customizable_built_in_procedures[] = 
{
    "abs", 
    "gcd",
    "lcm",
    "expt",
    "sqrt",
    "y-combinator",
    "make-vector",
    "vector-fill!",
    "vector->list",
    "list->vector",
    "map",
    "apply",
    "for-each",
    "reverse",
    "null?",
    "list-tail",
    "set-car!",
    "set-cdr!",
    "char-upcase",
    "char-downcase",
    "string",
    "make-string",
    "string-length",
    "string-append",
    "max", "min",
    "sin", "cos", "tan",
    "asin", "acos", "atan",
    "exp", "log"
};
#define CUSTOMIZABLE_BUILT_IN_PROCEDURE_COUNT (sizeof(customizable_built_in_procedures)/sizeof(const char *))

#endif

