/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_GRA_PARSER_HPP_INCLUDED
# define YY_GRA_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int gra_debug;
#endif
/* "%code requires" blocks.  */
#line 12 "parser.y" /* yacc.c:1909  */

    #pragma GCC diagnostic ignored "-Wconversion"

    #include "log/diagnostic_context_location.hpp"
    #define YYLTYPE YYLTYPE
    typedef gorc::diagnostic_context_location YYLTYPE;

    #define YYLLOC_DEFAULT(Cur, Rhs, N)                        \
     do                                                        \
       if (N)                                                  \
         {                                                     \
           (Cur).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
           (Cur).first_col    = YYRHSLOC(Rhs, 1).first_col;    \
           (Cur).last_line    = YYRHSLOC(Rhs, N).last_line;    \
           (Cur).last_col     = YYRHSLOC(Rhs, N).last_col;     \
           (Cur).filename     = YYRHSLOC(Rhs, N).filename;     \
         }                                                     \
       else                                                    \
         {                                                     \
           (Cur).first_line   = (Cur).last_line   =            \
             YYRHSLOC(Rhs, 0).last_line;                       \
           (Cur).first_col = (Cur).last_col =                  \
             YYRHSLOC(Rhs, 0).last_col;                        \
           (Cur).filename = YYRHSLOC(Rhs, 0).filename;         \
         }                                                     \
     while (0)

#line 72 "parser.hpp" /* yacc.c:1909  */

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    FLAGS = 258,
    SYMBOLS = 259,
    CODE = 260,
    END = 261,
    ENDLINE = 262,
    BREAK = 263,
    CALL = 264,
    DO = 265,
    ELSE = 266,
    FOR = 267,
    IF = 268,
    RETURN = 269,
    WHILE = 270,
    EQ_OP = 271,
    NE_OP = 272,
    GE_OP = 273,
    LE_OP = 274,
    LOGICAL_AND_OP = 275,
    LOGICAL_OR_OP = 276,
    IDENTIFIER = 277,
    STRING_FRAGMENT = 278,
    STRING_LITERAL = 279,
    INTEGER_LITERAL = 280,
    FLOAT_LITERAL = 281
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 46 "parser.y" /* yacc.c:1909  */

    char* string;
    char character;
    int integer;
    float floating;

    gorc::cog::ast::symbol_field* symbol_field;
    gorc::cog::ast::symbol_extension* symbol_extension;
    std::vector<gorc::cog::ast::symbol_extension*>* symbol_extension_list;
    gorc::cog::ast::symbol* symbol;
    std::vector<gorc::cog::ast::symbol*>* symbol_list;

    gorc::cog::ast::unary_operator unary_operator;
    gorc::cog::ast::infix_operator infix_operator;
    gorc::cog::ast::expression* expression;
    std::vector<gorc::cog::ast::expression*>* expression_list;

    gorc::cog::ast::statement* statement;
    std::vector<gorc::cog::ast::statement*>* statement_list;
    gorc::cog::ast::for_optional_expression* for_optional_expression;

    gorc::cog::ast::translation_unit* translation_unit;

#line 135 "parser.hpp" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



int gra_parse (gorc::cog::grammar::instance* instance);

#endif /* !YY_GRA_PARSER_HPP_INCLUDED  */
