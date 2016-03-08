/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 40 "parser.y" /* yacc.c:316  */

    #include "instance.hpp"
    #include "log/log.hpp"

#line 68 "parser.cpp" /* yacc.c:316  */

/* Substitute the variable and function names.  */
#define yyparse         gra_parse
#define yylex           gra_lex
#define yyerror         gra_error
#define yydebug         gra_debug
#define yynerrs         gra_nerrs


/* Copy the first part of user declarations.  */

#line 80 "parser.cpp" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* In a future release of Bison, this section will be replaced
   by #include "parser.hpp".  */
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
#line 12 "parser.y" /* yacc.c:355  */

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

#line 138 "parser.cpp" /* yacc.c:355  */

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
#line 46 "parser.y" /* yacc.c:355  */

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

#line 201 "parser.cpp" /* yacc.c:355  */
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

/* Copy the second part of user declarations.  */
#line 116 "parser.y" /* yacc.c:358  */

    using namespace gorc::cog;
    using namespace gorc::cog::ast;

    int yylex(YYSTYPE* lvalp, gorc::diagnostic_context_location* llocp, void* scanner);

    void yyerror(gorc::diagnostic_context_location* llocp, gorc::cog::grammar::instance*, const char* err) {
        gorc::diagnostic_context dc(nullptr, llocp->first_line, llocp->first_col);
        LOG_ERROR(err);
    }

    #define scanner instance->get_scanner()
    #define ast instance->factory

#line 243 "parser.cpp" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  10
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   219

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  49
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  41
/* YYNRULES -- Number of rules.  */
#define YYNRULES  102
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  182

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   281

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    36,     2,     2,     2,    39,    42,    31,
      32,    33,    37,    29,    28,    30,     2,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,    47,
      41,    27,    40,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    34,     2,    35,    43,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    44,    46,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   135,   135,   139,   143,   150,   154,   161,   165,   174,
     177,   184,   188,   192,   199,   210,   223,   226,   234,   238,
     246,   250,   254,   258,   262,   266,   273,   277,   281,   285,
     289,   296,   300,   304,   311,   315,   319,   323,   330,   334,
     338,   345,   349,   356,   360,   364,   371,   375,   382,   386,
     393,   397,   404,   408,   412,   416,   423,   427,   434,   438,
     445,   449,   456,   460,   467,   471,   478,   482,   489,   493,
     500,   504,   511,   515,   522,   526,   534,   538,   546,   550,
     557,   561,   568,   572,   576,   583,   587,   595,   598,   605,
     609,   613,   620,   627,   631,   635,   639,   643,   647,   656,
     659,   667,   671
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "FLAGS", "SYMBOLS", "CODE", "END",
  "ENDLINE", "BREAK", "CALL", "DO", "ELSE", "FOR", "IF", "RETURN", "WHILE",
  "EQ_OP", "NE_OP", "GE_OP", "LE_OP", "LOGICAL_AND_OP", "LOGICAL_OR_OP",
  "IDENTIFIER", "STRING_FRAGMENT", "STRING_LITERAL", "INTEGER_LITERAL",
  "FLOAT_LITERAL", "'='", "','", "'+'", "'-'", "'\\''", "'('", "')'",
  "'['", "']'", "'!'", "'*'", "'/'", "'%'", "'>'", "'<'", "'&'", "'^'",
  "'|'", "'{'", "'}'", "';'", "':'", "$accept", "symbol_field",
  "symbol_extension", "symbol_extension_seq", "symbol_extension_part",
  "symbol", "symbol_seq", "symbol_section", "argument_expression_list",
  "vector_component", "literal_expression", "primary_expression",
  "postfix_expression", "unary_operator", "unary_expression",
  "multiplicative_operator", "multiplicative_expression",
  "additive_operator", "additive_expression", "relational_operator",
  "relational_expression", "equality_operator", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "assignment_expression", "expression", "statement_seq",
  "compound_statement", "expression_statement", "jump_statement",
  "selection_statement", "for_optional_expression", "iteration_statement",
  "labeled_statement", "statement", "code_section", "translation_unit", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,    61,    44,    43,
      45,    39,    40,    41,    91,    93,    33,    42,    47,    37,
      62,    60,    38,    94,   124,   123,   125,    59,    58
};
# endif

#define YYPACT_NINF -144

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-144)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      45,   -19,     9,    14,    20,  -144,    35,  -144,     9,    53,
    -144,    90,    13,  -144,   106,     9,    54,   124,  -144,    92,
     111,   107,   138,   124,  -144,  -144,  -144,    92,  -144,  -144,
     104,   143,   107,   139,   140,   130,   146,    24,  -144,  -144,
    -144,  -144,  -144,    98,   144,  -144,    -3,  -144,  -144,  -144,
    -144,   144,   152,    64,    48,     6,    72,   141,   142,   137,
     162,   163,  -144,   -13,   107,  -144,  -144,  -144,  -144,  -144,
    -144,  -144,   180,   182,  -144,   181,  -144,   145,   174,   144,
     144,  -144,   144,   131,   144,   107,  -144,  -144,   109,   133,
      70,   -14,    51,  -144,    61,  -144,   144,  -144,  -144,  -144,
     144,  -144,  -144,   144,  -144,  -144,  -144,  -144,   144,  -144,
    -144,   144,   144,   144,   144,   144,   144,   144,  -144,  -144,
    -144,   107,  -144,  -144,   158,   165,   147,    76,    77,  -144,
     112,  -144,    26,  -144,  -144,  -144,  -144,  -144,    98,    98,
    -144,  -144,  -144,  -144,  -144,    64,    48,     6,    72,   141,
     142,   137,   162,  -144,   185,   144,   144,   107,   107,   144,
    -144,  -144,   167,   166,  -144,   113,   149,   187,  -144,  -144,
      98,  -144,   153,   144,   107,   168,  -144,   169,  -144,  -144,
     107,  -144
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    16,     0,     0,    13,     0,    14,    17,     0,
       1,     0,     9,    15,     0,    16,     5,     0,     7,    10,
       0,    99,     0,     0,     2,     3,     4,     9,     8,    11,
       0,     0,     0,     0,     0,     0,     0,    33,    26,    27,
      28,    38,    39,     0,     0,    40,     0,    81,    31,    34,
      41,     0,    46,    50,    56,    60,    62,    64,    66,    68,
      70,    72,    74,     0,   100,    97,    93,    94,    95,    96,
      98,    76,     0,     0,     6,     0,    82,     0,     0,    87,
       0,    83,     0,     0,     0,     0,    20,    21,     0,     0,
       0,    33,     0,    78,     0,    42,     0,    43,    44,    45,
       0,    48,    49,     0,    52,    53,    54,    55,     0,    58,
      59,     0,     0,     0,     0,     0,     0,     0,    80,    77,
     101,    99,    12,    84,     0,    88,     0,     0,     0,    36,
       0,    18,     0,    92,    22,    23,    24,    25,     0,     0,
      32,    79,    73,    47,    46,    51,    57,    61,    63,    65,
      67,    69,    71,    75,     0,     0,    87,     0,     0,     0,
      37,    35,     0,     0,   102,     0,     0,    85,    89,    19,
       0,    29,     0,    87,     0,     0,    90,     0,    86,    30,
       0,    91
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -144,   178,   184,  -144,   177,   197,  -144,   191,  -144,   -88,
    -144,  -144,  -144,  -144,   -48,  -144,   105,  -144,    99,  -144,
     100,  -144,    97,   101,    96,   102,   103,  -144,   -79,   -43,
     170,  -144,  -144,  -144,  -144,  -143,  -144,  -144,   -32,    91,
    -144
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    27,    18,    19,    20,     7,     8,     9,   130,    90,
      48,    49,    50,    51,    52,   100,    53,   103,    54,   108,
      55,   111,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,   126,    69,    70,    71,    72,
       3
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      78,    92,   139,    95,   131,    30,    31,    32,     4,    33,
      34,    35,    36,   166,    10,   117,     5,   142,    83,    37,
      84,    38,    39,    40,   104,   105,    41,    42,    43,    44,
     177,     6,   119,    45,   118,    16,   125,   127,   153,   128,
      17,   132,    46,    93,    47,    11,   106,   107,     1,     2,
     162,   163,   143,   133,   117,   144,    83,    12,    84,    14,
     144,   161,   119,   144,   144,   144,   144,   144,   144,    30,
      31,    32,    85,    33,    34,    35,    36,   101,   102,   117,
     169,    23,   175,    37,   140,    38,    39,    40,   109,   110,
      41,    42,    43,    44,    15,    86,    87,    45,   138,    88,
      89,    97,    98,    99,   117,   117,    46,   141,    47,   157,
     158,    21,   165,   125,    16,    30,    31,    32,    29,    33,
      34,    35,    36,    86,    87,   167,   168,    88,    89,    37,
     125,    38,    39,    40,   134,   135,    41,    42,    43,    44,
     159,   117,   178,    45,    73,   160,   172,    24,   181,    25,
      26,    76,    46,    91,    47,    38,    39,    40,   136,   137,
      41,    42,    43,    44,   129,    77,    91,    45,    38,    39,
      40,    79,    80,    41,    42,    43,    44,    81,    82,    96,
      45,   114,   115,   112,   116,   113,   120,   121,   122,   124,
     155,   164,   123,   117,   156,   170,   173,   171,   174,   179,
     176,    74,   180,    28,    75,    13,    22,   146,   145,   148,
     150,   147,   154,     0,   149,     0,    94,   151,     0,   152
};

static const yytype_int16 yycheck[] =
{
      32,    44,    90,    51,    83,     8,     9,    10,    27,    12,
      13,    14,    15,   156,     0,    28,     7,    96,    32,    22,
      34,    24,    25,    26,    18,    19,    29,    30,    31,    32,
     173,    22,    64,    36,    47,    22,    79,    80,   117,    82,
      27,    84,    45,    46,    47,    25,    40,    41,     3,     4,
     138,   139,   100,    85,    28,   103,    32,    22,    34,     6,
     108,    35,    94,   111,   112,   113,   114,   115,   116,     8,
       9,    10,    48,    12,    13,    14,    15,    29,    30,    28,
     159,    27,   170,    22,    33,    24,    25,    26,    16,    17,
      29,    30,    31,    32,     4,    25,    26,    36,    28,    29,
      30,    37,    38,    39,    28,    28,    45,    46,    47,    33,
      33,     5,   155,   156,    22,     8,     9,    10,     7,    12,
      13,    14,    15,    25,    26,   157,   158,    29,    30,    22,
     173,    24,    25,    26,    25,    26,    29,    30,    31,    32,
      28,    28,   174,    36,     6,    33,    33,    23,   180,    25,
      26,    47,    45,    22,    47,    24,    25,    26,    25,    26,
      29,    30,    31,    32,    33,    22,    22,    36,    24,    25,
      26,    32,    32,    29,    30,    31,    32,    47,    32,    27,
      36,    44,    20,    42,    21,    43,     6,     5,     7,    15,
      32,     6,    47,    28,    47,    28,    47,    31,    11,    31,
      47,    23,    33,    19,    27,     8,    15,   108,   103,   112,
     114,   111,   121,    -1,   113,    -1,    46,   115,    -1,   116
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    89,    27,     7,    22,    54,    55,    56,
       0,    25,    22,    54,     6,     4,    22,    27,    51,    52,
      53,     5,    56,    27,    23,    25,    26,    50,    51,     7,
       8,     9,    10,    12,    13,    14,    15,    22,    24,    25,
      26,    29,    30,    31,    32,    36,    45,    47,    59,    60,
      61,    62,    63,    65,    67,    69,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    85,
      86,    87,    88,     6,    50,    53,    47,    22,    87,    32,
      32,    47,    32,    32,    34,    48,    25,    26,    29,    30,
      58,    22,    78,    46,    79,    63,    27,    37,    38,    39,
      64,    29,    30,    66,    18,    19,    40,    41,    68,    16,
      17,    70,    42,    43,    44,    20,    21,    28,    47,    87,
       6,     5,     7,    47,    15,    78,    84,    78,    78,    33,
      57,    77,    78,    87,    25,    26,    25,    26,    28,    58,
      33,    46,    77,    63,    63,    65,    67,    69,    71,    72,
      73,    74,    75,    77,    88,    32,    47,    33,    33,    28,
      33,    35,    58,    58,     6,    78,    84,    87,    87,    77,
      28,    31,    33,    47,    11,    58,    47,    84,    87,    31,
      33,    87
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    49,    50,    50,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    54,    55,    55,    56,    56,    57,    57,
      58,    58,    58,    58,    58,    58,    59,    59,    59,    59,
      59,    60,    60,    60,    61,    61,    61,    61,    62,    62,
      62,    63,    63,    64,    64,    64,    65,    65,    66,    66,
      67,    67,    68,    68,    68,    68,    69,    69,    70,    70,
      71,    71,    72,    72,    73,    73,    74,    74,    75,    75,
      76,    76,    77,    77,    78,    78,    79,    79,    80,    80,
      81,    81,    82,    82,    82,    83,    83,    84,    84,    85,
      85,    85,    86,    87,    87,    87,    87,    87,    87,    88,
      88,    89,    89
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     3,     1,     2,     0,
       1,     4,     6,     1,     1,     2,     0,     1,     1,     3,
       1,     1,     2,     2,     2,     2,     1,     1,     1,     5,
       7,     1,     3,     1,     1,     4,     3,     4,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     3,     1,     1,
       1,     3,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     3,     1,     3,     1,     3,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     2,     2,     3,
       2,     1,     2,     2,     3,     5,     7,     0,     1,     5,
       7,     9,     3,     1,     1,     1,     1,     1,     1,     0,
       1,     6,     9
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (&yylloc, instance, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, instance); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, gorc::cog::grammar::instance* instance)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  YYUSE (instance);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, gorc::cog::grammar::instance* instance)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp, instance);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, gorc::cog::grammar::instance* instance)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , instance);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, instance); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, gorc::cog::grammar::instance* instance)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (instance);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
          case 22: /* IDENTIFIER  */
#line 112 "parser.y" /* yacc.c:1257  */
      { free(((*yyvaluep).string)); }
#line 1287 "parser.cpp" /* yacc.c:1257  */
        break;

    case 23: /* STRING_FRAGMENT  */
#line 112 "parser.y" /* yacc.c:1257  */
      { free(((*yyvaluep).string)); }
#line 1293 "parser.cpp" /* yacc.c:1257  */
        break;

    case 24: /* STRING_LITERAL  */
#line 112 "parser.y" /* yacc.c:1257  */
      { free(((*yyvaluep).string)); }
#line 1299 "parser.cpp" /* yacc.c:1257  */
        break;


      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (gorc::cog::grammar::instance* instance)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 136 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_field) = ast.make_var<symbol_field, string_fragment_field>((yyloc), (yyvsp[0].string));
        }
#line 1595 "parser.cpp" /* yacc.c:1646  */
    break;

  case 3:
#line 140 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_field) = ast.make_var<symbol_field, integer_field>((yyloc), (yyvsp[0].integer));
        }
#line 1603 "parser.cpp" /* yacc.c:1646  */
    break;

  case 4:
#line 144 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_field) = ast.make_var<symbol_field, float_field>((yyloc), (yyvsp[0].floating));
        }
#line 1611 "parser.cpp" /* yacc.c:1646  */
    break;

  case 5:
#line 151 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_extension) = ast.make_var<symbol_extension, bare_extension>((yyloc), (yyvsp[0].string));
        }
#line 1619 "parser.cpp" /* yacc.c:1646  */
    break;

  case 6:
#line 155 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_extension) = ast.make_var<symbol_extension, valued_extension>((yyloc), (yyvsp[-2].string), (yyvsp[0].symbol_field));
        }
#line 1627 "parser.cpp" /* yacc.c:1646  */
    break;

  case 7:
#line 162 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_extension_list) = ast.make_list((yyvsp[0].symbol_extension));
        }
#line 1635 "parser.cpp" /* yacc.c:1646  */
    break;

  case 8:
#line 166 "parser.y" /* yacc.c:1646  */
    {
            ((yyvsp[-1].symbol_extension_list))->push_back((yyvsp[0].symbol_extension));
            (yyval.symbol_extension_list) = (yyvsp[-1].symbol_extension_list);
        }
#line 1644 "parser.cpp" /* yacc.c:1646  */
    break;

  case 9:
#line 174 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_extension_list) = ast.make_list<symbol_extension>();
        }
#line 1652 "parser.cpp" /* yacc.c:1646  */
    break;

  case 10:
#line 178 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_extension_list) = (yyvsp[0].symbol_extension_list);
        }
#line 1660 "parser.cpp" /* yacc.c:1646  */
    break;

  case 11:
#line 185 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol) = ast.make<symbol>((yyloc), (yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].symbol_extension_list));
        }
#line 1668 "parser.cpp" /* yacc.c:1646  */
    break;

  case 12:
#line 189 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol) = ast.make<symbol>((yyloc), (yyvsp[-5].string), (yyvsp[-4].string), (yyvsp[-1].symbol_extension_list), (yyvsp[-2].symbol_field));
        }
#line 1676 "parser.cpp" /* yacc.c:1646  */
    break;

  case 13:
#line 193 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol) = nullptr;
        }
#line 1684 "parser.cpp" /* yacc.c:1646  */
    break;

  case 14:
#line 200 "parser.y" /* yacc.c:1646  */
    {
            if((yyvsp[0].symbol) == nullptr)
            {
                (yyval.symbol_list) = ast.make_list<symbol>();
            }
            else
            {
                (yyval.symbol_list) = ast.make_list((yyvsp[0].symbol));
            }
        }
#line 1699 "parser.cpp" /* yacc.c:1646  */
    break;

  case 15:
#line 211 "parser.y" /* yacc.c:1646  */
    {
            if((yyvsp[0].symbol) != nullptr)
            {
                ((yyvsp[-1].symbol_list))->push_back((yyvsp[0].symbol));
            }

            (yyval.symbol_list) = (yyvsp[-1].symbol_list);
        }
#line 1712 "parser.cpp" /* yacc.c:1646  */
    break;

  case 16:
#line 223 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_list) = ast.make_list<symbol>();
        }
#line 1720 "parser.cpp" /* yacc.c:1646  */
    break;

  case 17:
#line 227 "parser.y" /* yacc.c:1646  */
    {
            (yyval.symbol_list) = (yyvsp[0].symbol_list);
        }
#line 1728 "parser.cpp" /* yacc.c:1646  */
    break;

  case 18:
#line 235 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression_list) = ast.make_list((yyvsp[0].expression));
        }
#line 1736 "parser.cpp" /* yacc.c:1646  */
    break;

  case 19:
#line 239 "parser.y" /* yacc.c:1646  */
    {
            ((yyvsp[-2].expression_list))->push_back((yyvsp[0].expression));
            (yyval.expression_list) = (yyvsp[-2].expression_list);
        }
#line 1745 "parser.cpp" /* yacc.c:1646  */
    break;

  case 20:
#line 247 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = static_cast<float>((yyvsp[0].integer));
        }
#line 1753 "parser.cpp" /* yacc.c:1646  */
    break;

  case 21:
#line 251 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = (yyvsp[0].floating);
        }
#line 1761 "parser.cpp" /* yacc.c:1646  */
    break;

  case 22:
#line 255 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = static_cast<float>((yyvsp[0].integer));
        }
#line 1769 "parser.cpp" /* yacc.c:1646  */
    break;

  case 23:
#line 259 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = (yyvsp[0].floating);
        }
#line 1777 "parser.cpp" /* yacc.c:1646  */
    break;

  case 24:
#line 263 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = -static_cast<float>((yyvsp[0].integer));
        }
#line 1785 "parser.cpp" /* yacc.c:1646  */
    break;

  case 25:
#line 267 "parser.y" /* yacc.c:1646  */
    {
            (yyval.floating) = -(yyvsp[0].floating);
        }
#line 1793 "parser.cpp" /* yacc.c:1646  */
    break;

  case 26:
#line 274 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, string_literal_expression>((yyloc), (yyvsp[0].string));
        }
#line 1801 "parser.cpp" /* yacc.c:1646  */
    break;

  case 27:
#line 278 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, integer_literal_expression>((yyloc), (yyvsp[0].integer));
        }
#line 1809 "parser.cpp" /* yacc.c:1646  */
    break;

  case 28:
#line 282 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, float_literal_expression>((yyloc), (yyvsp[0].floating));
        }
#line 1817 "parser.cpp" /* yacc.c:1646  */
    break;

  case 29:
#line 286 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, vector_literal_expression>((yyloc), (yyvsp[-3].floating), (yyvsp[-2].floating), (yyvsp[-1].floating));
        }
#line 1825 "parser.cpp" /* yacc.c:1646  */
    break;

  case 30:
#line 290 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, vector_literal_expression>((yyloc), (yyvsp[-5].floating), (yyvsp[-3].floating), (yyvsp[-1].floating));
        }
#line 1833 "parser.cpp" /* yacc.c:1646  */
    break;

  case 31:
#line 297 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 1841 "parser.cpp" /* yacc.c:1646  */
    break;

  case 32:
#line 301 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[-1].expression);
        }
#line 1849 "parser.cpp" /* yacc.c:1646  */
    break;

  case 33:
#line 305 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, identifier_expression>((yyloc), (yyvsp[0].string));
        }
#line 1857 "parser.cpp" /* yacc.c:1646  */
    break;

  case 34:
#line 312 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 1865 "parser.cpp" /* yacc.c:1646  */
    break;

  case 35:
#line 316 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, subscript_expression>((yyloc), (yyvsp[-3].string), (yyvsp[-1].expression));
        }
#line 1873 "parser.cpp" /* yacc.c:1646  */
    break;

  case 36:
#line 320 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, method_call_expression>((yyloc), (yyvsp[-2].string), ast.make_list<expression>());
        }
#line 1881 "parser.cpp" /* yacc.c:1646  */
    break;

  case 37:
#line 324 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, method_call_expression>((yyloc), (yyvsp[-3].string), (yyvsp[-1].expression_list));
        }
#line 1889 "parser.cpp" /* yacc.c:1646  */
    break;

  case 38:
#line 331 "parser.y" /* yacc.c:1646  */
    {
            (yyval.unary_operator) = unary_operator::plus;
        }
#line 1897 "parser.cpp" /* yacc.c:1646  */
    break;

  case 39:
#line 335 "parser.y" /* yacc.c:1646  */
    {
            (yyval.unary_operator) = unary_operator::minus;
        }
#line 1905 "parser.cpp" /* yacc.c:1646  */
    break;

  case 40:
#line 339 "parser.y" /* yacc.c:1646  */
    {
            (yyval.unary_operator) = unary_operator::logical_not;
        }
#line 1913 "parser.cpp" /* yacc.c:1646  */
    break;

  case 41:
#line 346 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 1921 "parser.cpp" /* yacc.c:1646  */
    break;

  case 42:
#line 350 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, unary_expression>((yyloc), (yyvsp[0].expression), (yyvsp[-1].unary_operator));
        }
#line 1929 "parser.cpp" /* yacc.c:1646  */
    break;

  case 43:
#line 357 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::multiplication;
        }
#line 1937 "parser.cpp" /* yacc.c:1646  */
    break;

  case 44:
#line 361 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::division;
        }
#line 1945 "parser.cpp" /* yacc.c:1646  */
    break;

  case 45:
#line 365 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::modulo;
        }
#line 1953 "parser.cpp" /* yacc.c:1646  */
    break;

  case 46:
#line 372 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 1961 "parser.cpp" /* yacc.c:1646  */
    break;

  case 47:
#line 376 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), (yyvsp[-1].infix_operator));
        }
#line 1969 "parser.cpp" /* yacc.c:1646  */
    break;

  case 48:
#line 383 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::addition;
        }
#line 1977 "parser.cpp" /* yacc.c:1646  */
    break;

  case 49:
#line 387 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::subtraction;
        }
#line 1985 "parser.cpp" /* yacc.c:1646  */
    break;

  case 50:
#line 394 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 1993 "parser.cpp" /* yacc.c:1646  */
    break;

  case 51:
#line 398 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), (yyvsp[-1].infix_operator));
        }
#line 2001 "parser.cpp" /* yacc.c:1646  */
    break;

  case 52:
#line 405 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::greater_equal;
        }
#line 2009 "parser.cpp" /* yacc.c:1646  */
    break;

  case 53:
#line 409 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::less_equal;
        }
#line 2017 "parser.cpp" /* yacc.c:1646  */
    break;

  case 54:
#line 413 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::greater;
        }
#line 2025 "parser.cpp" /* yacc.c:1646  */
    break;

  case 55:
#line 417 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::less;
        }
#line 2033 "parser.cpp" /* yacc.c:1646  */
    break;

  case 56:
#line 424 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2041 "parser.cpp" /* yacc.c:1646  */
    break;

  case 57:
#line 428 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), (yyvsp[-1].infix_operator));
        }
#line 2049 "parser.cpp" /* yacc.c:1646  */
    break;

  case 58:
#line 435 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::equal;
        }
#line 2057 "parser.cpp" /* yacc.c:1646  */
    break;

  case 59:
#line 439 "parser.y" /* yacc.c:1646  */
    {
            (yyval.infix_operator) = infix_operator::not_equal;
        }
#line 2065 "parser.cpp" /* yacc.c:1646  */
    break;

  case 60:
#line 446 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2073 "parser.cpp" /* yacc.c:1646  */
    break;

  case 61:
#line 450 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), (yyvsp[-1].infix_operator));
        }
#line 2081 "parser.cpp" /* yacc.c:1646  */
    break;

  case 62:
#line 457 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2089 "parser.cpp" /* yacc.c:1646  */
    break;

  case 63:
#line 461 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), infix_operator::bitwise_and);
        }
#line 2097 "parser.cpp" /* yacc.c:1646  */
    break;

  case 64:
#line 468 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2105 "parser.cpp" /* yacc.c:1646  */
    break;

  case 65:
#line 472 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), infix_operator::bitwise_xor);
        }
#line 2113 "parser.cpp" /* yacc.c:1646  */
    break;

  case 66:
#line 479 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2121 "parser.cpp" /* yacc.c:1646  */
    break;

  case 67:
#line 483 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), infix_operator::bitwise_or);
        }
#line 2129 "parser.cpp" /* yacc.c:1646  */
    break;

  case 68:
#line 490 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2137 "parser.cpp" /* yacc.c:1646  */
    break;

  case 69:
#line 494 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), infix_operator::logical_and);
        }
#line 2145 "parser.cpp" /* yacc.c:1646  */
    break;

  case 70:
#line 501 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2153 "parser.cpp" /* yacc.c:1646  */
    break;

  case 71:
#line 505 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, infix_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression), infix_operator::logical_or);
        }
#line 2161 "parser.cpp" /* yacc.c:1646  */
    break;

  case 72:
#line 512 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2169 "parser.cpp" /* yacc.c:1646  */
    break;

  case 73:
#line 516 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, assignment_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression));
        }
#line 2177 "parser.cpp" /* yacc.c:1646  */
    break;

  case 74:
#line 523 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = (yyvsp[0].expression);
        }
#line 2185 "parser.cpp" /* yacc.c:1646  */
    break;

  case 75:
#line 527 "parser.y" /* yacc.c:1646  */
    {
            (yyval.expression) = ast.make_var<expression, comma_expression>((yyloc), (yyvsp[-2].expression), (yyvsp[0].expression));
        }
#line 2193 "parser.cpp" /* yacc.c:1646  */
    break;

  case 76:
#line 535 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement_list) = ast.make_list((yyvsp[0].statement));
        }
#line 2201 "parser.cpp" /* yacc.c:1646  */
    break;

  case 77:
#line 539 "parser.y" /* yacc.c:1646  */
    {
            ((yyvsp[-1].statement_list))->push_back((yyvsp[0].statement));
            (yyval.statement_list) = (yyvsp[-1].statement_list);
        }
#line 2210 "parser.cpp" /* yacc.c:1646  */
    break;

  case 78:
#line 547 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, compound_statement>((yyloc), ast.make_list<statement>());
        }
#line 2218 "parser.cpp" /* yacc.c:1646  */
    break;

  case 79:
#line 551 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, compound_statement>((yyloc), (yyvsp[-1].statement_list));
        }
#line 2226 "parser.cpp" /* yacc.c:1646  */
    break;

  case 80:
#line 558 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, expression_statement>((yyloc), (yyvsp[-1].expression));
        }
#line 2234 "parser.cpp" /* yacc.c:1646  */
    break;

  case 81:
#line 562 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, empty_statement>((yyloc));
        }
#line 2242 "parser.cpp" /* yacc.c:1646  */
    break;

  case 82:
#line 569 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, break_statement>((yyloc));
        }
#line 2250 "parser.cpp" /* yacc.c:1646  */
    break;

  case 83:
#line 573 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, return_statement>((yyloc));
        }
#line 2258 "parser.cpp" /* yacc.c:1646  */
    break;

  case 84:
#line 577 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, call_statement>((yyloc), (yyvsp[-1].string));
        }
#line 2266 "parser.cpp" /* yacc.c:1646  */
    break;

  case 85:
#line 584 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, if_statement>((yyloc), (yyvsp[-2].expression), (yyvsp[0].statement));
        }
#line 2274 "parser.cpp" /* yacc.c:1646  */
    break;

  case 86:
#line 588 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, if_else_statement>((yyloc), (yyvsp[-4].expression), (yyvsp[-2].statement), (yyvsp[0].statement));
        }
#line 2282 "parser.cpp" /* yacc.c:1646  */
    break;

  case 87:
#line 595 "parser.y" /* yacc.c:1646  */
    {
            (yyval.for_optional_expression) = ast.make_var<for_optional_expression, for_empty_expression>((yyloc));
        }
#line 2290 "parser.cpp" /* yacc.c:1646  */
    break;

  case 88:
#line 599 "parser.y" /* yacc.c:1646  */
    {
            (yyval.for_optional_expression) = ast.make_var<for_optional_expression, for_expression>((yyloc), (yyvsp[0].expression));
        }
#line 2298 "parser.cpp" /* yacc.c:1646  */
    break;

  case 89:
#line 606 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, while_statement>((yyloc), (yyvsp[-2].expression), (yyvsp[0].statement));
        }
#line 2306 "parser.cpp" /* yacc.c:1646  */
    break;

  case 90:
#line 610 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, do_statement>((yyloc), (yyvsp[-5].statement), (yyvsp[-2].expression));
        }
#line 2314 "parser.cpp" /* yacc.c:1646  */
    break;

  case 91:
#line 614 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, for_statement>((yyloc), (yyvsp[-6].for_optional_expression), (yyvsp[-4].for_optional_expression), (yyvsp[-2].for_optional_expression), (yyvsp[0].statement));
        }
#line 2322 "parser.cpp" /* yacc.c:1646  */
    break;

  case 92:
#line 621 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = ast.make_var<statement, labeled_statement>((yyloc), (yyvsp[-2].string), (yyvsp[0].statement));
        }
#line 2330 "parser.cpp" /* yacc.c:1646  */
    break;

  case 93:
#line 628 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2338 "parser.cpp" /* yacc.c:1646  */
    break;

  case 94:
#line 632 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2346 "parser.cpp" /* yacc.c:1646  */
    break;

  case 95:
#line 636 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2354 "parser.cpp" /* yacc.c:1646  */
    break;

  case 96:
#line 640 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2362 "parser.cpp" /* yacc.c:1646  */
    break;

  case 97:
#line 644 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2370 "parser.cpp" /* yacc.c:1646  */
    break;

  case 98:
#line 648 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement) = (yyvsp[0].statement);
        }
#line 2378 "parser.cpp" /* yacc.c:1646  */
    break;

  case 99:
#line 656 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement_list) = ast.make_list<statement>();
        }
#line 2386 "parser.cpp" /* yacc.c:1646  */
    break;

  case 100:
#line 660 "parser.y" /* yacc.c:1646  */
    {
            (yyval.statement_list) = (yyvsp[0].statement_list);
        }
#line 2394 "parser.cpp" /* yacc.c:1646  */
    break;

  case 101:
#line 668 "parser.y" /* yacc.c:1646  */
    {
            instance->set_return_value((yyval.translation_unit) = ast.make<translation_unit>((yyloc), 0, (yyvsp[-4].symbol_list), (yyvsp[-1].statement_list)));
        }
#line 2402 "parser.cpp" /* yacc.c:1646  */
    break;

  case 102:
#line 672 "parser.y" /* yacc.c:1646  */
    {
            instance->set_return_value((yyval.translation_unit) = ast.make<translation_unit>((yyloc), (yyvsp[-6].integer), (yyvsp[-4].symbol_list), (yyvsp[-1].statement_list)));
        }
#line 2410 "parser.cpp" /* yacc.c:1646  */
    break;


#line 2414 "parser.cpp" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, instance, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, instance, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc, instance);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, instance);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, instance, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, instance);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp, instance);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
