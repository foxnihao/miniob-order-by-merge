/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 2 "yacc_sql.y"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <string>

#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/expr/expression.h"
#include "sql/parser/yacc_sql.hpp"
#include "sql/parser/lex_sql.h"

using namespace std;

string token_name(const char *sql_string, YYLTYPE *llocp)
{
  return string(sql_string + llocp->first_column, llocp->last_column - llocp->first_column + 1);
}

int yyerror(YYLTYPE *llocp, const char *sql_string, ParsedSqlResult *sql_result, yyscan_t scanner, const char *msg)
{
  std::unique_ptr<ParsedSqlNode> error_sql_node = std::make_unique<ParsedSqlNode>(SCF_ERROR);
  error_sql_node->error.error_msg = msg;
  error_sql_node->error.line = llocp->first_line;
  error_sql_node->error.column = llocp->first_column;
  sql_result->add_sql_node(std::move(error_sql_node));
  return 0;
}

ConjunctionExpr *create_conjunction_expression(ConjunctionExpr::Type type,
                                               Expression *left,
                                               Expression *right,
                                               const char *sql_string,
                                               YYLTYPE *llocp)
{
  ConjunctionExpr *expr = new ConjunctionExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

ComparisonExpr *create_comparison_expression(CompOp comp_op,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ComparisonExpr *expr = new ComparisonExpr(comp_op, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

ArithmeticExpr *create_arithmetic_expression(ArithmeticExpr::Type type,
                                             Expression *left,
                                             Expression *right,
                                             const char *sql_string,
                                             YYLTYPE *llocp)
{
  ArithmeticExpr *expr = new ArithmeticExpr(type, left, right);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

FunctionExpr *create_function_expression(FunctionExpr::Type type,
                                         std::vector<Expression *> *args,
                                         const char *sql_string,
                                         YYLTYPE *llocp)
{
  FunctionExpr *expr = new FunctionExpr(type, *args);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}

AggreExpr *create_aggre_func_expression(AggreExpr::Type type,
                                        std::vector<Expression *> *args,
                                        const char *sql_string,
                                        YYLTYPE *llocp)
{
  AggreExpr *expr = new AggreExpr(type, *args);
  expr->set_name(token_name(sql_string, llocp));
  return expr;
}


#line 158 "yacc_sql.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "yacc_sql.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_SEMICOLON = 3,                  /* SEMICOLON  */
  YYSYMBOL_CREATE = 4,                     /* CREATE  */
  YYSYMBOL_DROP = 5,                       /* DROP  */
  YYSYMBOL_TABLE = 6,                      /* TABLE  */
  YYSYMBOL_TABLES = 7,                     /* TABLES  */
  YYSYMBOL_UNIQUE = 8,                     /* UNIQUE  */
  YYSYMBOL_INDEX = 9,                      /* INDEX  */
  YYSYMBOL_CALC = 10,                      /* CALC  */
  YYSYMBOL_SELECT = 11,                    /* SELECT  */
  YYSYMBOL_DESC = 12,                      /* DESC  */
  YYSYMBOL_SHOW = 13,                      /* SHOW  */
  YYSYMBOL_SYNC = 14,                      /* SYNC  */
  YYSYMBOL_INSERT = 15,                    /* INSERT  */
  YYSYMBOL_DELETE = 16,                    /* DELETE  */
  YYSYMBOL_UPDATE = 17,                    /* UPDATE  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 19,                    /* RBRACE  */
  YYSYMBOL_COMMA = 20,                     /* COMMA  */
  YYSYMBOL_TRX_BEGIN = 21,                 /* TRX_BEGIN  */
  YYSYMBOL_TRX_COMMIT = 22,                /* TRX_COMMIT  */
  YYSYMBOL_TRX_ROLLBACK = 23,              /* TRX_ROLLBACK  */
  YYSYMBOL_INT_T = 24,                     /* INT_T  */
  YYSYMBOL_STRING_T = 25,                  /* STRING_T  */
  YYSYMBOL_FLOAT_T = 26,                   /* FLOAT_T  */
  YYSYMBOL_DATE_T = 27,                    /* DATE_T  */
  YYSYMBOL_HELP = 28,                      /* HELP  */
  YYSYMBOL_EXIT = 29,                      /* EXIT  */
  YYSYMBOL_DOT = 30,                       /* DOT  */
  YYSYMBOL_INTO = 31,                      /* INTO  */
  YYSYMBOL_VALUES = 32,                    /* VALUES  */
  YYSYMBOL_FROM = 33,                      /* FROM  */
  YYSYMBOL_WHERE = 34,                     /* WHERE  */
  YYSYMBOL_LIKE = 35,                      /* LIKE  */
  YYSYMBOL_AS = 36,                        /* AS  */
  YYSYMBOL_LENGTH = 37,                    /* LENGTH  */
  YYSYMBOL_ROUND = 38,                     /* ROUND  */
  YYSYMBOL_DATE_FORMAT = 39,               /* DATE_FORMAT  */
  YYSYMBOL_NOT = 40,                       /* NOT  */
  YYSYMBOL_IS = 41,                        /* IS  */
  YYSYMBOL_NULL_ = 42,                     /* NULL_  */
  YYSYMBOL_COUNT = 43,                     /* COUNT  */
  YYSYMBOL_SUM = 44,                       /* SUM  */
  YYSYMBOL_AVG = 45,                       /* AVG  */
  YYSYMBOL_MAX = 46,                       /* MAX  */
  YYSYMBOL_MIN = 47,                       /* MIN  */
  YYSYMBOL_INNER = 48,                     /* INNER  */
  YYSYMBOL_JOIN = 49,                      /* JOIN  */
  YYSYMBOL_AND = 50,                       /* AND  */
  YYSYMBOL_OR = 51,                        /* OR  */
  YYSYMBOL_SET = 52,                       /* SET  */
  YYSYMBOL_ON = 53,                        /* ON  */
  YYSYMBOL_LOAD = 54,                      /* LOAD  */
  YYSYMBOL_DATA = 55,                      /* DATA  */
  YYSYMBOL_INFILE = 56,                    /* INFILE  */
  YYSYMBOL_EXPLAIN = 57,                   /* EXPLAIN  */
  YYSYMBOL_GROUP = 58,                     /* GROUP  */
  YYSYMBOL_HAVING = 59,                    /* HAVING  */
  YYSYMBOL_ORDER = 60,                     /* ORDER  */
  YYSYMBOL_BY = 61,                        /* BY  */
  YYSYMBOL_ASC = 62,                       /* ASC  */
  YYSYMBOL_EQ = 63,                        /* EQ  */
  YYSYMBOL_LT = 64,                        /* LT  */
  YYSYMBOL_GT = 65,                        /* GT  */
  YYSYMBOL_LE = 66,                        /* LE  */
  YYSYMBOL_GE = 67,                        /* GE  */
  YYSYMBOL_NE = 68,                        /* NE  */
  YYSYMBOL_IN = 69,                        /* IN  */
  YYSYMBOL_EXISTS = 70,                    /* EXISTS  */
  YYSYMBOL_UNSIGNED_NUMBER = 71,           /* UNSIGNED_NUMBER  */
  YYSYMBOL_UNSIGNED_FLOAT = 72,            /* UNSIGNED_FLOAT  */
  YYSYMBOL_ID = 73,                        /* ID  */
  YYSYMBOL_DATE_STR = 74,                  /* DATE_STR  */
  YYSYMBOL_SSS = 75,                       /* SSS  */
  YYSYMBOL_76_ = 76,                       /* '+'  */
  YYSYMBOL_77_ = 77,                       /* '-'  */
  YYSYMBOL_78_ = 78,                       /* '*'  */
  YYSYMBOL_79_ = 79,                       /* '/'  */
  YYSYMBOL_UMINUS = 80,                    /* UMINUS  */
  YYSYMBOL_81_n_ = 81,                     /* '\n'  */
  YYSYMBOL_YYACCEPT = 82,                  /* $accept  */
  YYSYMBOL_commands = 83,                  /* commands  */
  YYSYMBOL_command_wrapper = 84,           /* command_wrapper  */
  YYSYMBOL_exit_stmt = 85,                 /* exit_stmt  */
  YYSYMBOL_help_stmt = 86,                 /* help_stmt  */
  YYSYMBOL_sync_stmt = 87,                 /* sync_stmt  */
  YYSYMBOL_begin_stmt = 88,                /* begin_stmt  */
  YYSYMBOL_commit_stmt = 89,               /* commit_stmt  */
  YYSYMBOL_rollback_stmt = 90,             /* rollback_stmt  */
  YYSYMBOL_drop_table_stmt = 91,           /* drop_table_stmt  */
  YYSYMBOL_show_tables_stmt = 92,          /* show_tables_stmt  */
  YYSYMBOL_desc_table_stmt = 93,           /* desc_table_stmt  */
  YYSYMBOL_create_index_stmt = 94,         /* create_index_stmt  */
  YYSYMBOL_attr_index_list = 95,           /* attr_index_list  */
  YYSYMBOL_attr_index = 96,                /* attr_index  */
  YYSYMBOL_drop_index_stmt = 97,           /* drop_index_stmt  */
  YYSYMBOL_create_table_stmt = 98,         /* create_table_stmt  */
  YYSYMBOL_attr_def_list = 99,             /* attr_def_list  */
  YYSYMBOL_attr_def = 100,                 /* attr_def  */
  YYSYMBOL_null_ = 101,                    /* null_  */
  YYSYMBOL_number = 102,                   /* number  */
  YYSYMBOL_type = 103,                     /* type  */
  YYSYMBOL_insert_stmt = 104,              /* insert_stmt  */
  YYSYMBOL_value_list = 105,               /* value_list  */
  YYSYMBOL_value = 106,                    /* value  */
  YYSYMBOL_delete_stmt = 107,              /* delete_stmt  */
  YYSYMBOL_update_stmt = 108,              /* update_stmt  */
  YYSYMBOL_update_list = 109,              /* update_list  */
  YYSYMBOL_update_item = 110,              /* update_item  */
  YYSYMBOL_select_stmt = 111,              /* select_stmt  */
  YYSYMBOL_from = 112,                     /* from  */
  YYSYMBOL_group_list = 113,               /* group_list  */
  YYSYMBOL_having = 114,                   /* having  */
  YYSYMBOL_order_list = 115,               /* order_list  */
  YYSYMBOL_order_item = 116,               /* order_item  */
  YYSYMBOL_order_flag = 117,               /* order_flag  */
  YYSYMBOL_order_items = 118,              /* order_items  */
  YYSYMBOL_join_condition = 119,           /* join_condition  */
  YYSYMBOL_select_exprs = 120,             /* select_exprs  */
  YYSYMBOL_select_expr = 121,              /* select_expr  */
  YYSYMBOL_select_expr_list = 122,         /* select_expr_list  */
  YYSYMBOL_calc_stmt = 123,                /* calc_stmt  */
  YYSYMBOL_expression_list = 124,          /* expression_list  */
  YYSYMBOL_expression = 125,               /* expression  */
  YYSYMBOL_expr1 = 126,                    /* expr1  */
  YYSYMBOL_expr2 = 127,                    /* expr2  */
  YYSYMBOL_expr3 = 128,                    /* expr3  */
  YYSYMBOL_function_expression = 129,      /* function_expression  */
  YYSYMBOL_function_type = 130,            /* function_type  */
  YYSYMBOL_aggre_type = 131,               /* aggre_type  */
  YYSYMBOL_rel_attr = 132,                 /* rel_attr  */
  YYSYMBOL_as_alias = 133,                 /* as_alias  */
  YYSYMBOL_as = 134,                       /* as  */
  YYSYMBOL_alias = 135,                    /* alias  */
  YYSYMBOL_rel_list = 136,                 /* rel_list  */
  YYSYMBOL_relation = 137,                 /* relation  */
  YYSYMBOL_where = 138,                    /* where  */
  YYSYMBOL_comp_op_binary = 139,           /* comp_op_binary  */
  YYSYMBOL_comp_op_unary = 140,            /* comp_op_unary  */
  YYSYMBOL_load_data_stmt = 141,           /* load_data_stmt  */
  YYSYMBOL_explain_stmt = 142,             /* explain_stmt  */
  YYSYMBOL_set_variable_stmt = 143,        /* set_variable_stmt  */
  YYSYMBOL_opt_semicolon = 144             /* opt_semicolon  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  87
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   339

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  82
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  63
/* YYNRULES -- Number of rules.  */
#define YYNRULES  156
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  275

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   331


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      81,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    78,    76,     2,    77,     2,    79,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    80
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   282,   282,   290,   291,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   309,   313,   319,   324,   330,   336,   342,   348,
     355,   361,   369,   387,   409,   412,   425,   432,   442,   458,
     476,   487,   490,   503,   512,   524,   527,   531,   538,   539,
     542,   543,   544,   545,   548,   564,   567,   578,   582,   586,
     590,   594,   599,   602,   608,   619,   631,   636,   648,   657,
     679,   682,   693,   696,   705,   706,   709,   712,   724,   732,
     733,   734,   738,   741,   774,   775,   778,   788,   794,   797,
     808,   818,   823,   834,   837,   838,   839,   847,   850,   851,
     854,   857,   858,   859,   860,   861,   868,   872,   878,   882,
     890,   893,   900,   904,   907,   913,   914,   915,   918,   919,
     920,   921,   922,   926,   931,   937,   942,   952,   953,   956,
     957,   960,   961,   966,   969,   978,   989,   998,  1001,  1007,
    1008,  1009,  1010,  1011,  1012,  1013,  1014,  1015,  1016,  1017,
    1018,  1022,  1026,  1039,  1047,  1059,  1060
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "SEMICOLON", "CREATE",
  "DROP", "TABLE", "TABLES", "UNIQUE", "INDEX", "CALC", "SELECT", "DESC",
  "SHOW", "SYNC", "INSERT", "DELETE", "UPDATE", "LBRACE", "RBRACE",
  "COMMA", "TRX_BEGIN", "TRX_COMMIT", "TRX_ROLLBACK", "INT_T", "STRING_T",
  "FLOAT_T", "DATE_T", "HELP", "EXIT", "DOT", "INTO", "VALUES", "FROM",
  "WHERE", "LIKE", "AS", "LENGTH", "ROUND", "DATE_FORMAT", "NOT", "IS",
  "NULL_", "COUNT", "SUM", "AVG", "MAX", "MIN", "INNER", "JOIN", "AND",
  "OR", "SET", "ON", "LOAD", "DATA", "INFILE", "EXPLAIN", "GROUP",
  "HAVING", "ORDER", "BY", "ASC", "EQ", "LT", "GT", "LE", "GE", "NE", "IN",
  "EXISTS", "UNSIGNED_NUMBER", "UNSIGNED_FLOAT", "ID", "DATE_STR", "SSS",
  "'+'", "'-'", "'*'", "'/'", "UMINUS", "'\\n'", "$accept", "commands",
  "command_wrapper", "exit_stmt", "help_stmt", "sync_stmt", "begin_stmt",
  "commit_stmt", "rollback_stmt", "drop_table_stmt", "show_tables_stmt",
  "desc_table_stmt", "create_index_stmt", "attr_index_list", "attr_index",
  "drop_index_stmt", "create_table_stmt", "attr_def_list", "attr_def",
  "null_", "number", "type", "insert_stmt", "value_list", "value",
  "delete_stmt", "update_stmt", "update_list", "update_item",
  "select_stmt", "from", "group_list", "having", "order_list",
  "order_item", "order_flag", "order_items", "join_condition",
  "select_exprs", "select_expr", "select_expr_list", "calc_stmt",
  "expression_list", "expression", "expr1", "expr2", "expr3",
  "function_expression", "function_type", "aggre_type", "rel_attr",
  "as_alias", "as", "alias", "rel_list", "relation", "where",
  "comp_op_binary", "comp_op_unary", "load_data_stmt", "explain_stmt",
  "set_variable_stmt", "opt_semicolon", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-204)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-130)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     265,   126,    67,   132,   132,   -56,    14,  -204,    43,    54,
      10,  -204,  -204,  -204,  -204,  -204,    22,    71,   265,   122,
       4,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,    66,   120,    73,    83,    89,    70,  -204,  -204,  -204,
     132,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
     133,  -204,  -204,   195,  -204,  -204,  -204,    21,  -204,   260,
    -204,  -204,   147,   148,  -204,   143,   134,   153,    50,  -204,
    -204,    95,   110,    30,   121,   135,  -204,  -204,  -204,  -204,
    -204,    -5,   119,   140,  -204,   141,   175,   177,    -3,  -204,
     -54,   132,  -204,  -204,  -204,   132,   132,   132,  -204,   -25,
     157,  -204,  -204,  -204,  -204,  -204,  -204,  -204,   143,   143,
     143,   143,   143,   132,   -10,    42,   125,   166,   132,  -204,
    -204,  -204,    63,   169,     2,   156,    56,   136,   146,   211,
     170,   151,   152,    56,  -204,  -204,  -204,  -204,  -204,  -204,
     176,  -204,  -204,  -204,     0,     0,  -204,  -204,    42,   208,
    -204,   209,     2,    -9,   132,   171,   153,  -204,  -204,  -204,
     213,   166,   162,    34,  -204,   205,   127,   217,  -204,   173,
     225,  -204,   224,  -204,  -204,  -204,   125,   199,  -204,   108,
     188,   190,  -204,    56,  -204,   189,   166,   231,   247,  -204,
    -204,  -204,  -204,    51,   146,   235,   237,   183,    56,   238,
      -9,   125,   132,   197,  -204,   224,   132,  -204,   162,   186,
      19,   218,  -204,  -204,   217,     7,   183,  -204,   242,   224,
    -204,  -204,   210,   206,   132,   245,   108,  -204,  -204,  -204,
     200,   253,  -204,  -204,   211,   242,   183,   255,  -204,   132,
      -9,   132,  -204,   263,     8,  -204,  -204,   115,  -204,   266,
     242,  -204,   108,  -204,   108,   132,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,   263,  -204
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,    25,     0,     0,
       0,    26,    27,    28,    24,    23,     0,     0,     0,     0,
       0,    22,    21,    14,    15,    16,    17,     9,    10,    11,
      12,    13,     8,     5,     7,     6,     4,     3,    18,    19,
      20,     0,     0,     0,     0,     0,     0,   115,   116,   117,
       0,    63,   118,   119,   120,   121,   122,   151,    57,    59,
     125,    62,    61,     0,   123,   106,    90,    91,    93,    97,
     100,   110,     0,     0,   111,     0,    70,    88,   127,    31,
      30,     0,     0,   129,     0,     0,   153,     1,   155,   156,
       2,   129,     0,     0,    29,     0,   106,     0,     0,    96,
       0,     0,    58,    60,   105,     0,     0,     0,   145,     0,
     147,   139,   140,   141,   142,   143,   144,   149,     0,     0,
       0,     0,     0,     0,     0,    99,     0,   137,     0,    86,
     130,    87,     0,     0,   127,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   107,   108,   126,   124,    92,    95,
      94,   146,   150,   148,   101,   102,   103,   104,    98,     0,
     114,     0,   127,   133,     0,    72,    88,   131,   132,   128,
       0,   137,     0,     0,   154,     0,     0,    41,    40,     0,
       0,    37,    55,   112,   113,   136,     0,     0,    71,   138,
       0,    76,    89,     0,    64,     0,   137,    66,     0,    50,
      51,    52,    53,    45,     0,     0,     0,     0,     0,     0,
     133,     0,     0,     0,    69,    55,     0,    65,     0,     0,
       0,     0,    46,    44,    41,    38,     0,    36,    34,    55,
     109,   134,    84,    74,     0,     0,    68,    67,   152,    48,
       0,     0,    47,    42,     0,    34,     0,     0,    56,     0,
     133,     0,    73,    82,    79,    54,    49,    45,    39,     0,
      34,    32,    85,   135,    75,     0,    77,    81,    80,    78,
      43,    33,    35,    82,    83
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -204,  -204,   271,  -204,  -204,  -204,  -204,  -204,  -204,  -204,
    -204,  -204,  -204,  -156,  -201,  -204,  -204,    60,    86,    35,
    -204,  -204,  -204,  -203,   -44,  -204,  -204,    78,  -204,   -45,
    -204,  -204,  -204,  -204,    26,  -204,    24,  -204,  -204,   174,
     137,  -204,  -101,    -4,  -204,   -69,   236,  -204,  -204,  -204,
    -204,   -78,   -88,  -204,  -196,  -171,  -116,  -204,  -204,  -204,
    -204,  -204,  -204
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,   247,   228,    31,    32,   205,   177,   223,
     241,   203,    33,   209,    65,    34,    35,   196,   197,    36,
     127,   191,   252,   214,   253,   269,   266,   250,    76,    77,
     129,    37,    66,    67,    68,    69,    70,    71,    72,    73,
      74,   131,   132,   169,   188,   163,   165,   122,    75,    38,
      39,    40,    90
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      78,    97,    96,   139,   148,   135,   125,    88,    46,   160,
     151,   186,   235,   138,   231,   210,   145,    79,  -129,   146,
     267,    80,   159,   161,   147,   245,   248,    47,    48,    49,
      50,   130,    51,    52,    53,    54,    55,    56,   130,   187,
     232,   105,    98,   130,   152,   260,    99,   106,   107,   154,
     155,   156,   157,   158,   263,   194,   171,    96,   106,   107,
      57,    58,    59,    60,    61,    62,   130,    63,    64,   220,
     268,   106,   107,    44,    81,  -129,    45,  -129,   120,   121,
     217,     4,  -127,    83,   185,    89,   130,    82,    46,   259,
     239,   221,   174,   222,   178,    84,   240,    98,    51,   182,
     106,   107,   149,   150,   272,   102,   103,    47,    48,    49,
      50,   233,    51,    52,    53,    54,    55,    56,   118,   119,
     120,   121,    87,  -129,    78,  -129,    85,    58,    59,    92,
      61,    62,    41,   173,    42,    43,   167,   244,   168,    91,
      57,    58,    59,    60,    61,    62,    93,    63,    64,   215,
      46,   199,   200,   201,   202,   221,    94,   222,   106,   107,
     189,    46,    95,   100,   229,   123,   124,   126,   133,    47,
      48,    49,    50,   128,    51,    52,    53,    54,    55,    56,
      47,    48,    49,   134,   136,    51,    52,    53,    54,    55,
      56,   137,   140,   141,   142,   143,   144,   153,   162,   258,
     164,   170,    57,    58,    59,    60,    61,    62,   172,    63,
      64,   175,   236,   101,    58,    59,    60,    61,    62,   176,
      63,    64,     4,   179,   180,   181,   106,   183,   184,   190,
     254,   193,    47,    48,    49,   195,   198,   204,    52,    53,
      54,    55,    56,   207,   208,   262,   206,   264,   211,   212,
     213,   218,   216,   219,   225,   226,   227,   230,   234,   238,
     242,   254,   246,   249,   255,   251,   102,   103,    60,     1,
       2,   256,   257,    64,   261,     3,     4,     5,     6,     7,
       8,     9,    10,   265,   243,   271,    11,    12,    13,    86,
     224,   273,   270,    14,    15,   108,   237,   274,     0,   104,
     109,   110,   166,   192,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    16,     0,    17,
       0,     0,    18,   111,   112,   113,   114,   115,   116,   117,
       0,     0,     0,     0,     0,     0,   118,   119,   120,   121
};

static const yytype_int16 yycheck[] =
{
       4,    46,    46,    91,   105,    83,    75,     3,    18,    19,
      35,    20,   215,    18,   210,   186,    19,    73,    11,    73,
      12,     7,   123,   124,    78,   226,   229,    37,    38,    39,
      40,    36,    42,    43,    44,    45,    46,    47,    36,    48,
     211,    20,    46,    36,    69,   246,    50,    50,    51,   118,
     119,   120,   121,   122,   250,   171,   134,   101,    50,    51,
      70,    71,    72,    73,    74,    75,    36,    77,    78,    18,
      62,    50,    51,     6,    31,    73,     9,    75,    78,    79,
     196,    11,    52,    73,   162,    81,    36,    33,    18,   245,
      71,    40,   136,    42,   139,    73,    77,   101,    42,   143,
      50,    51,   106,   107,   260,    71,    72,    37,    38,    39,
      40,   212,    42,    43,    44,    45,    46,    47,    76,    77,
      78,    79,     0,    73,   128,    75,    55,    71,    72,     9,
      74,    75,     6,    77,     8,     9,    73,   225,    75,    73,
      70,    71,    72,    73,    74,    75,    73,    77,    78,   193,
      18,    24,    25,    26,    27,    40,    73,    42,    50,    51,
     164,    18,    73,    30,   208,    18,    18,    33,    73,    37,
      38,    39,    40,    20,    42,    43,    44,    45,    46,    47,
      37,    38,    39,    73,    63,    42,    43,    44,    45,    46,
      47,    56,    73,    53,    53,    20,    19,    40,    73,   244,
      34,    32,    70,    71,    72,    73,    74,    75,    52,    77,
      78,    75,   216,    18,    71,    72,    73,    74,    75,    73,
      77,    78,    11,    53,    73,    73,    50,    19,    19,    58,
     234,    18,    37,    38,    39,    73,    31,    20,    43,    44,
      45,    46,    47,    18,    20,   249,    73,   251,    49,    61,
      60,    20,    63,     6,    19,    18,    73,    19,    61,    73,
      42,   265,    20,    53,    19,    59,    71,    72,    73,     4,
       5,    71,    19,    78,    19,    10,    11,    12,    13,    14,
      15,    16,    17,    20,   224,    19,    21,    22,    23,    18,
     204,   265,   257,    28,    29,    35,   218,   273,    -1,    63,
      40,    41,   128,   166,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    52,    -1,    54,
      -1,    -1,    57,    63,    64,    65,    66,    67,    68,    69,
      -1,    -1,    -1,    -1,    -1,    -1,    76,    77,    78,    79
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,    10,    11,    12,    13,    14,    15,    16,
      17,    21,    22,    23,    28,    29,    52,    54,    57,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    97,    98,   104,   107,   108,   111,   123,   141,   142,
     143,     6,     8,     9,     6,     9,    18,    37,    38,    39,
      40,    42,    43,    44,    45,    46,    47,    70,    71,    72,
      73,    74,    75,    77,    78,   106,   124,   125,   126,   127,
     128,   129,   130,   131,   132,   140,   120,   121,   125,    73,
       7,    31,    33,    73,    73,    55,    84,     0,     3,    81,
     144,    73,     9,    73,    73,    73,   106,   111,   125,   125,
      30,    18,    71,    72,   128,    20,    50,    51,    35,    40,
      41,    63,    64,    65,    66,    67,    68,    69,    76,    77,
      78,    79,   139,    18,    18,   127,    33,   112,    20,   122,
      36,   133,   134,    73,    73,   133,    63,    56,    18,   134,
      73,    53,    53,    20,    19,    19,    73,    78,   124,   125,
     125,    35,    69,    40,   127,   127,   127,   127,   127,   124,
      19,   124,    73,   137,    34,   138,   121,    73,    75,   135,
      32,   133,    52,    77,   106,    75,    73,   100,   111,    53,
      73,    73,   106,    19,    19,   133,    20,    48,   136,   125,
      58,   113,   122,    18,   138,    73,   109,   110,    31,    24,
      25,    26,    27,   103,    20,    99,    73,    18,    20,   105,
     137,    49,    61,    60,   115,   106,    63,   138,    20,     6,
      18,    40,    42,   101,   100,    19,    18,    73,    96,   106,
      19,   136,   137,   124,    61,   105,   125,   109,    73,    71,
      77,   102,    42,    99,   134,    96,    20,    95,   105,    53,
     119,    59,   114,   116,   125,    19,    71,    19,   111,    95,
      96,    19,   125,   136,   125,    20,   118,    12,    62,   117,
     101,    19,    95,   116,   118
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    82,    83,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    94,    95,    95,    96,    97,    98,    98,
      98,    99,    99,   100,   100,   101,   101,   101,   102,   102,
     103,   103,   103,   103,   104,   105,   105,   106,   106,   106,
     106,   106,   106,   106,   107,   108,   109,   109,   110,   111,
     112,   112,   113,   113,   114,   114,   115,   115,   116,   117,
     117,   117,   118,   118,   119,   119,   120,   121,   122,   122,
     123,   124,   124,   125,   125,   125,   125,   126,   126,   126,
     127,   127,   127,   127,   127,   127,   127,   127,   128,   128,
     128,   128,   129,   129,   129,   130,   130,   130,   131,   131,
     131,   131,   131,   132,   132,   132,   132,   133,   133,   134,
     134,   135,   135,   136,   136,   136,   137,   138,   138,   139,
     139,   139,   139,   139,   139,   139,   139,   139,   139,   139,
     139,   140,   141,   142,   143,   144,   144
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       2,     2,     9,    10,     0,     3,     1,     5,     7,     9,
       5,     0,     3,     6,     3,     0,     1,     2,     1,     2,
       1,     1,     1,     1,     8,     0,     3,     1,     2,     1,
       2,     1,     1,     1,     5,     6,     1,     3,     3,     6,
       0,     3,     0,     4,     0,     2,     0,     4,     2,     0,
       1,     1,     0,     3,     0,     2,     2,     2,     0,     3,
       2,     1,     3,     1,     3,     3,     2,     1,     3,     2,
       1,     3,     3,     3,     3,     2,     1,     3,     3,     6,
       1,     1,     4,     4,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     3,     0,     2,     0,
       1,     1,     1,     0,     3,     5,     2,     0,     2,     1,
       1,     1,     1,     1,     1,     1,     2,     1,     2,     1,
       2,     1,     7,     2,     4,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (&yylloc, sql_string, sql_result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

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


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
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

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location, sql_string, sql_result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp, sql_string, sql_result, scanner);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]), sql_string, sql_result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, sql_string, sql_result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  YY_USE (sql_string);
  YY_USE (sql_result);
  YY_USE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner)
{
/* Lookahead token kind.  */
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
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, &yylloc, scanner);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* commands: command_wrapper opt_semicolon  */
#line 283 "yacc_sql.y"
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[-1].sql_node));
    sql_result->add_sql_node(std::move(sql_node));
  }
#line 1914 "yacc_sql.cpp"
    break;

  case 23: /* exit_stmt: EXIT  */
#line 313 "yacc_sql.y"
         {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXIT);
    }
#line 1923 "yacc_sql.cpp"
    break;

  case 24: /* help_stmt: HELP  */
#line 319 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_HELP);
    }
#line 1931 "yacc_sql.cpp"
    break;

  case 25: /* sync_stmt: SYNC  */
#line 324 "yacc_sql.y"
         {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SYNC);
    }
#line 1939 "yacc_sql.cpp"
    break;

  case 26: /* begin_stmt: TRX_BEGIN  */
#line 330 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_BEGIN);
    }
#line 1947 "yacc_sql.cpp"
    break;

  case 27: /* commit_stmt: TRX_COMMIT  */
#line 336 "yacc_sql.y"
               {
      (yyval.sql_node) = new ParsedSqlNode(SCF_COMMIT);
    }
#line 1955 "yacc_sql.cpp"
    break;

  case 28: /* rollback_stmt: TRX_ROLLBACK  */
#line 342 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_ROLLBACK);
    }
#line 1963 "yacc_sql.cpp"
    break;

  case 29: /* drop_table_stmt: DROP TABLE ID  */
#line 348 "yacc_sql.y"
                  {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_TABLE);
      (yyval.sql_node)->drop_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 1973 "yacc_sql.cpp"
    break;

  case 30: /* show_tables_stmt: SHOW TABLES  */
#line 355 "yacc_sql.y"
                {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
#line 1981 "yacc_sql.cpp"
    break;

  case 31: /* desc_table_stmt: DESC ID  */
#line 361 "yacc_sql.y"
             {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DESC_TABLE);
      (yyval.sql_node)->desc_table.relation_name = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 1991 "yacc_sql.cpp"
    break;

  case 32: /* create_index_stmt: CREATE INDEX ID ON ID LBRACE attr_index attr_index_list RBRACE  */
#line 370 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      create_index.attribute_names.emplace_back((yyvsp[-2].attr_name));
      std::vector<std::string> *src_attrs = (yyvsp[-1].attr_names);

      if(src_attrs != nullptr){
        create_index.attribute_names.insert(create_index.attribute_names.end(), src_attrs->begin(), src_attrs->end());
        delete src_attrs;
      }
      std::reverse(create_index.attribute_names.begin(),create_index.attribute_names.end());
      create_index.is_unique = false;
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
    }
#line 2013 "yacc_sql.cpp"
    break;

  case 33: /* create_index_stmt: CREATE UNIQUE INDEX ID ON ID LBRACE attr_index attr_index_list RBRACE  */
#line 388 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = (yyval.sql_node)->create_index;
      create_index.index_name = (yyvsp[-6].string);
      create_index.relation_name = (yyvsp[-4].string);
      create_index.attribute_names.emplace_back((yyvsp[-2].attr_name));
      std::vector<std::string> *src_attrs = (yyvsp[-1].attr_names);

      if(src_attrs != nullptr){
        create_index.attribute_names.insert(create_index.attribute_names.end(), src_attrs->begin(), src_attrs->end());
        delete src_attrs;
      }
      std::reverse(create_index.attribute_names.begin(),create_index.attribute_names.end());
      create_index.is_unique = true;
      free((yyvsp[-6].string));
      free((yyvsp[-4].string));
    }
#line 2035 "yacc_sql.cpp"
    break;

  case 34: /* attr_index_list: %empty  */
#line 409 "yacc_sql.y"
    {
      (yyval.attr_names) = nullptr;
    }
#line 2043 "yacc_sql.cpp"
    break;

  case 35: /* attr_index_list: COMMA attr_index attr_index_list  */
#line 413 "yacc_sql.y"
    {
      if((yyvsp[0].attr_names) != nullptr){
        (yyval.attr_names) = (yyvsp[0].attr_names);
      }else {
        (yyval.attr_names) = new std::vector<std::string>;
      }
      (yyval.attr_names)->emplace_back(std::string((yyvsp[-1].attr_name)));
      free((yyvsp[-1].attr_name));
    }
#line 2057 "yacc_sql.cpp"
    break;

  case 36: /* attr_index: ID  */
#line 426 "yacc_sql.y"
    {
     (yyval.attr_name) =  (yyvsp[0].string);
     //free($1);
    }
#line 2066 "yacc_sql.cpp"
    break;

  case 37: /* drop_index_stmt: DROP INDEX ID ON ID  */
#line 433 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DROP_INDEX);
      (yyval.sql_node)->drop_index.index_name = (yyvsp[-2].string);
      (yyval.sql_node)->drop_index.relation_name = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2078 "yacc_sql.cpp"
    break;

  case 38: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE  */
#line 443 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-4].string);
      free((yyvsp[-4].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-1].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-2].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-2].attr_info);
    }
#line 2098 "yacc_sql.cpp"
    break;

  case 39: /* create_table_stmt: CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE as select_stmt  */
#line 459 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-6].string);
      free((yyvsp[-6].string));

      std::vector<AttrInfoSqlNode> *src_attrs = (yyvsp[-3].attr_infos);

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*(yyvsp[-4].attr_info));
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete (yyvsp[-4].attr_info);

      create_table.as_expression = new SubqueryExpr(token_name(sql_string, &(yylsp[0])));
    }
#line 2120 "yacc_sql.cpp"
    break;

  case 40: /* create_table_stmt: CREATE TABLE ID as select_stmt  */
#line 476 "yacc_sql.y"
                                    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = (yyval.sql_node)->create_table;
      create_table.relation_name = (yyvsp[-2].string);
      free((yyvsp[-2].string));

      create_table.as_expression = new SubqueryExpr(token_name(sql_string, &(yylsp[0])));
    }
#line 2133 "yacc_sql.cpp"
    break;

  case 41: /* attr_def_list: %empty  */
#line 487 "yacc_sql.y"
    {
      (yyval.attr_infos) = nullptr;
    }
#line 2141 "yacc_sql.cpp"
    break;

  case 42: /* attr_def_list: COMMA attr_def attr_def_list  */
#line 491 "yacc_sql.y"
    {
      if ((yyvsp[0].attr_infos) != nullptr) {
        (yyval.attr_infos) = (yyvsp[0].attr_infos);
      } else {
        (yyval.attr_infos) = new std::vector<AttrInfoSqlNode>;
      }
      (yyval.attr_infos)->emplace_back(*(yyvsp[-1].attr_info));
      delete (yyvsp[-1].attr_info);
    }
#line 2155 "yacc_sql.cpp"
    break;

  case 43: /* attr_def: ID type LBRACE number RBRACE null_  */
#line 504 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-4].number);
      (yyval.attr_info)->name = (yyvsp[-5].string);
      (yyval.attr_info)->length = (yyvsp[-2].number);
      (yyval.attr_info)->is_null = (yyvsp[0].BOOLEN);
      free((yyvsp[-5].string));
    }
#line 2168 "yacc_sql.cpp"
    break;

  case 44: /* attr_def: ID type null_  */
#line 513 "yacc_sql.y"
    {
      (yyval.attr_info) = new AttrInfoSqlNode;
      (yyval.attr_info)->type = (AttrType)(yyvsp[-1].number);
      (yyval.attr_info)->name = (yyvsp[-2].string);
      (yyval.attr_info)->length = 4;
      (yyval.attr_info)->is_null = (yyvsp[0].BOOLEN);
      free((yyvsp[-2].string));
    }
#line 2181 "yacc_sql.cpp"
    break;

  case 45: /* null_: %empty  */
#line 524 "yacc_sql.y"
    {
      (yyval.BOOLEN) = false;
    }
#line 2189 "yacc_sql.cpp"
    break;

  case 46: /* null_: NULL_  */
#line 528 "yacc_sql.y"
    {
      (yyval.BOOLEN) = true;
    }
#line 2197 "yacc_sql.cpp"
    break;

  case 47: /* null_: NOT NULL_  */
#line 532 "yacc_sql.y"
    {
      (yyval.BOOLEN) = false;
    }
#line 2205 "yacc_sql.cpp"
    break;

  case 48: /* number: UNSIGNED_NUMBER  */
#line 538 "yacc_sql.y"
                    {(yyval.number) = (yyvsp[0].number);}
#line 2211 "yacc_sql.cpp"
    break;

  case 49: /* number: '-' UNSIGNED_NUMBER  */
#line 539 "yacc_sql.y"
                          {(yyval.number) = -(yyvsp[0].number);}
#line 2217 "yacc_sql.cpp"
    break;

  case 50: /* type: INT_T  */
#line 542 "yacc_sql.y"
               { (yyval.number)=INTS; }
#line 2223 "yacc_sql.cpp"
    break;

  case 51: /* type: STRING_T  */
#line 543 "yacc_sql.y"
               { (yyval.number)=CHARS; }
#line 2229 "yacc_sql.cpp"
    break;

  case 52: /* type: FLOAT_T  */
#line 544 "yacc_sql.y"
               { (yyval.number)=FLOATS; }
#line 2235 "yacc_sql.cpp"
    break;

  case 53: /* type: DATE_T  */
#line 545 "yacc_sql.y"
               { (yyval.number)=DATES; }
#line 2241 "yacc_sql.cpp"
    break;

  case 54: /* insert_stmt: INSERT INTO ID VALUES LBRACE value value_list RBRACE  */
#line 549 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_INSERT);
      (yyval.sql_node)->insertion.relation_name = (yyvsp[-5].string);
      if ((yyvsp[-1].value_list) != nullptr) {
        (yyval.sql_node)->insertion.values.swap(*(yyvsp[-1].value_list));
      }
      (yyval.sql_node)->insertion.values.emplace_back(*(yyvsp[-2].value));
      std::reverse((yyval.sql_node)->insertion.values.begin(), (yyval.sql_node)->insertion.values.end());
      delete (yyvsp[-2].value);
      free((yyvsp[-5].string));
    }
#line 2257 "yacc_sql.cpp"
    break;

  case 55: /* value_list: %empty  */
#line 564 "yacc_sql.y"
    {
      (yyval.value_list) = nullptr;
    }
#line 2265 "yacc_sql.cpp"
    break;

  case 56: /* value_list: COMMA value value_list  */
#line 567 "yacc_sql.y"
                              { 
      if ((yyvsp[0].value_list) != nullptr) {
        (yyval.value_list) = (yyvsp[0].value_list);
      } else {
        (yyval.value_list) = new std::vector<Value>;
      }
      (yyval.value_list)->emplace_back(*(yyvsp[-1].value));
      delete (yyvsp[-1].value);
    }
#line 2279 "yacc_sql.cpp"
    break;

  case 57: /* value: UNSIGNED_NUMBER  */
#line 578 "yacc_sql.y"
                    {
      (yyval.value) = new Value((int)(yyvsp[0].number));
      (yyloc) = (yylsp[0]);
    }
#line 2288 "yacc_sql.cpp"
    break;

  case 58: /* value: '-' UNSIGNED_NUMBER  */
#line 582 "yacc_sql.y"
                                       {
      (yyval.value) = new Value(-(int)(yyvsp[0].number));
      (yyloc) = (yylsp[-1]);
    }
#line 2297 "yacc_sql.cpp"
    break;

  case 59: /* value: UNSIGNED_FLOAT  */
#line 586 "yacc_sql.y"
                     {
      (yyval.value) = new Value((float)(yyvsp[0].floats));
      (yyloc) = (yylsp[0]);
    }
#line 2306 "yacc_sql.cpp"
    break;

  case 60: /* value: '-' UNSIGNED_FLOAT  */
#line 590 "yacc_sql.y"
                                      {
      (yyval.value) = new Value(-(float)(yyvsp[0].floats));
      (yyloc) = (yylsp[-1]);
    }
#line 2315 "yacc_sql.cpp"
    break;

  case 61: /* value: SSS  */
#line 594 "yacc_sql.y"
          {
      char *tmp = common::substr((yyvsp[0].string),1,strlen((yyvsp[0].string))-2);
      (yyval.value) = new Value(tmp);
      free(tmp);
    }
#line 2325 "yacc_sql.cpp"
    break;

  case 62: /* value: DATE_STR  */
#line 599 "yacc_sql.y"
               {
      (yyval.value) = new Value((yyvsp[0].string), true);
    }
#line 2333 "yacc_sql.cpp"
    break;

  case 63: /* value: NULL_  */
#line 602 "yacc_sql.y"
           {
      (yyval.value) = new Value(Value::null_value());
    }
#line 2341 "yacc_sql.cpp"
    break;

  case 64: /* delete_stmt: DELETE FROM ID as_alias where  */
#line 609 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_DELETE);
      (yyval.sql_node)->deletion.relation_name = (yyvsp[-2].string);
      (yyval.sql_node)->deletion.relation_alias = (yyvsp[-2].string);
      if ((yyvsp[-1].string) != nullptr) (yyval.sql_node)->deletion.relation_alias = (yyvsp[-1].string);
      (yyval.sql_node)->deletion.condition = (yyvsp[0].expression);
    }
#line 2353 "yacc_sql.cpp"
    break;

  case 65: /* update_stmt: UPDATE ID as_alias SET update_list where  */
#line 620 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_UPDATE);
      (yyval.sql_node)->update.relation_name = (yyvsp[-4].string);
      (yyval.sql_node)->update.relation_alias = (yyvsp[-4].string);
      if ((yyvsp[-3].string) != nullptr) (yyval.sql_node)->update.relation_alias = (yyvsp[-3].string);
      std::reverse((yyvsp[-1].update_values)->begin(), (yyvsp[-1].update_values)->end());
      (yyval.sql_node)->update.update_items.swap(*(yyvsp[-1].update_values));
      (yyval.sql_node)->update.condition = (yyvsp[0].expression);
    }
#line 2367 "yacc_sql.cpp"
    break;

  case 66: /* update_list: update_item  */
#line 632 "yacc_sql.y"
    {
      (yyval.update_values) = new std::vector<ParsedUpdateItem *>;
      (yyval.update_values)->emplace_back((yyvsp[0].update_value));
    }
#line 2376 "yacc_sql.cpp"
    break;

  case 67: /* update_list: update_item COMMA update_list  */
#line 637 "yacc_sql.y"
    {
      if((yyvsp[0].update_values) != nullptr){
        (yyval.update_values) = (yyvsp[0].update_values);
      }
      else{
        (yyval.update_values) = new std::vector<ParsedUpdateItem *>;
      }
      (yyval.update_values)->emplace_back((yyvsp[-2].update_value));
    }
#line 2390 "yacc_sql.cpp"
    break;

  case 68: /* update_item: ID EQ expression  */
#line 649 "yacc_sql.y"
    {
      (yyval.update_value) = new ParsedUpdateItem();
      (yyval.update_value)->attribute_name = (yyvsp[-2].string);
      (yyval.update_value)->expression = (yyvsp[0].expression);
    }
#line 2400 "yacc_sql.cpp"
    break;

  case 69: /* select_stmt: SELECT select_exprs from where group_list order_list  */
#line 658 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SELECT);
      if ((yyvsp[-4].expression_list) != nullptr) {
        (yyval.sql_node)->selection.expressions.swap(*(yyvsp[-4].expression_list));
      }
      if ((yyvsp[-3].relation_list) != nullptr) {
        (yyval.sql_node)->selection.relations.swap(*(yyvsp[-3].relation_list));
        std::reverse((yyval.sql_node)->selection.relations.begin(), (yyval.sql_node)->selection.relations.end());
      }
      (yyval.sql_node)->selection.condition = (yyvsp[-2].expression);
      if ((yyvsp[-1].groups) != nullptr) {
        (yyval.sql_node)->selection.groups = *(yyvsp[-1].groups);
      }
      if ((yyvsp[0].order_list) != nullptr) {
        (yyval.sql_node)->selection.orders.swap(*(yyvsp[0].order_list));
        std::reverse((yyval.sql_node)->selection.orders.begin(), (yyval.sql_node)->selection.orders.end());
      }
    }
#line 2423 "yacc_sql.cpp"
    break;

  case 70: /* from: %empty  */
#line 679 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 2431 "yacc_sql.cpp"
    break;

  case 71: /* from: FROM relation rel_list  */
#line 682 "yacc_sql.y"
                             {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<RelationSqlNode>;
      }
      (yyval.relation_list)->emplace_back(*(yyvsp[-1].relation));
    }
#line 2444 "yacc_sql.cpp"
    break;

  case 72: /* group_list: %empty  */
#line 693 "yacc_sql.y"
    {
      (yyval.groups) = nullptr;
    }
#line 2452 "yacc_sql.cpp"
    break;

  case 73: /* group_list: GROUP BY expression_list having  */
#line 697 "yacc_sql.y"
    {
      (yyval.groups) = new GroupsSqlNode;
      (yyval.groups)->group_exprs.swap(*(yyvsp[-1].expression_list));
      std::reverse((yyval.groups)->group_exprs.begin(), (yyval.groups)->group_exprs.end());
      (yyval.groups)->having = (yyvsp[0].expression);
    }
#line 2463 "yacc_sql.cpp"
    break;

  case 74: /* having: %empty  */
#line 705 "yacc_sql.y"
                          { (yyval.expression) = nullptr; }
#line 2469 "yacc_sql.cpp"
    break;

  case 75: /* having: HAVING expression  */
#line 706 "yacc_sql.y"
                          { (yyval.expression) = (yyvsp[0].expression); }
#line 2475 "yacc_sql.cpp"
    break;

  case 76: /* order_list: %empty  */
#line 709 "yacc_sql.y"
    {
      (yyval.order_list) = nullptr;
    }
#line 2483 "yacc_sql.cpp"
    break;

  case 77: /* order_list: ORDER BY order_item order_items  */
#line 713 "yacc_sql.y"
    {
      if ((yyvsp[0].order_list) != nullptr) {
        (yyval.order_list) = (yyvsp[0].order_list);
      } else {
        (yyval.order_list) = new std::vector<OrderSqlNode>;
      }
      (yyval.order_list)->emplace_back(*(yyvsp[-1].order));
      delete (yyvsp[-1].order);
    }
#line 2497 "yacc_sql.cpp"
    break;

  case 78: /* order_item: expression order_flag  */
#line 725 "yacc_sql.y"
    {
      (yyval.order) = new OrderSqlNode;
      (yyval.order)->expression = (yyvsp[-1].expression);
      (yyval.order)->flag = (yyvsp[0].order_flag);
    }
#line 2507 "yacc_sql.cpp"
    break;

  case 79: /* order_flag: %empty  */
#line 732 "yacc_sql.y"
                { (yyval.order_flag) = ORDER_ASC; }
#line 2513 "yacc_sql.cpp"
    break;

  case 80: /* order_flag: ASC  */
#line 733 "yacc_sql.y"
                { (yyval.order_flag) = ORDER_ASC; }
#line 2519 "yacc_sql.cpp"
    break;

  case 81: /* order_flag: DESC  */
#line 734 "yacc_sql.y"
                { (yyval.order_flag) = ORDER_DESC; }
#line 2525 "yacc_sql.cpp"
    break;

  case 82: /* order_items: %empty  */
#line 738 "yacc_sql.y"
    {
      (yyval.order_list) = nullptr;
    }
#line 2533 "yacc_sql.cpp"
    break;

  case 83: /* order_items: COMMA order_item order_items  */
#line 742 "yacc_sql.y"
    {
      if ((yyvsp[0].order_list) != nullptr) {
        (yyval.order_list) = (yyvsp[0].order_list);
      } else {
        (yyval.order_list) = new std::vector<OrderSqlNode>;
      }
      (yyval.order_list)->emplace_back(*(yyvsp[-1].order));
      delete (yyvsp[-1].order);
    }
#line 2547 "yacc_sql.cpp"
    break;

  case 84: /* join_condition: %empty  */
#line 774 "yacc_sql.y"
                    { (yyval.expression) = nullptr; }
#line 2553 "yacc_sql.cpp"
    break;

  case 85: /* join_condition: ON expression  */
#line 775 "yacc_sql.y"
                    { (yyval.expression) = (yyvsp[0].expression); }
#line 2559 "yacc_sql.cpp"
    break;

  case 86: /* select_exprs: select_expr select_expr_list  */
#line 778 "yacc_sql.y"
                                 {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2572 "yacc_sql.cpp"
    break;

  case 87: /* select_expr: expression as_alias  */
#line 788 "yacc_sql.y"
                        {
      (yyval.expression) = (yyvsp[-1].expression);
      if ((yyvsp[0].string) != nullptr) (yyval.expression)->set_name((yyvsp[0].string));
    }
#line 2581 "yacc_sql.cpp"
    break;

  case 88: /* select_expr_list: %empty  */
#line 794 "yacc_sql.y"
    {
      (yyval.expression_list) = nullptr;
    }
#line 2589 "yacc_sql.cpp"
    break;

  case 89: /* select_expr_list: COMMA select_expr select_expr_list  */
#line 797 "yacc_sql.y"
                                         {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-1].expression));
    }
#line 2602 "yacc_sql.cpp"
    break;

  case 90: /* calc_stmt: CALC expression_list  */
#line 809 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_CALC);
      std::reverse((yyvsp[0].expression_list)->begin(), (yyvsp[0].expression_list)->end());
      (yyval.sql_node)->calc.expressions.swap(*(yyvsp[0].expression_list));
      delete (yyvsp[0].expression_list);
    }
#line 2613 "yacc_sql.cpp"
    break;

  case 91: /* expression_list: expression  */
#line 819 "yacc_sql.y"
    {
      (yyval.expression_list) = new std::vector<Expression*>;
      (yyval.expression_list)->emplace_back((yyvsp[0].expression));
    }
#line 2622 "yacc_sql.cpp"
    break;

  case 92: /* expression_list: expression COMMA expression_list  */
#line 824 "yacc_sql.y"
    {
      if ((yyvsp[0].expression_list) != nullptr) {
        (yyval.expression_list) = (yyvsp[0].expression_list);
      } else {
        (yyval.expression_list) = new std::vector<Expression *>;
      }
      (yyval.expression_list)->emplace_back((yyvsp[-2].expression));
    }
#line 2635 "yacc_sql.cpp"
    break;

  case 93: /* expression: expr1  */
#line 834 "yacc_sql.y"
          {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2643 "yacc_sql.cpp"
    break;

  case 94: /* expression: expression OR expression  */
#line 837 "yacc_sql.y"
                                { (yyval.expression) = create_conjunction_expression(ConjunctionExpr::Type::OR, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2649 "yacc_sql.cpp"
    break;

  case 95: /* expression: expression AND expression  */
#line 838 "yacc_sql.y"
                                { (yyval.expression) = create_conjunction_expression(ConjunctionExpr::Type::AND, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2655 "yacc_sql.cpp"
    break;

  case 96: /* expression: NOT expression  */
#line 840 "yacc_sql.y"
    {
      std::vector<Expression *> *args = new std::vector<Expression *>;
      args->push_back((yyvsp[0].expression));
      (yyval.expression) = create_function_expression(FunctionExpr::Type::NOT, args, sql_string, &(yyloc));
    }
#line 2665 "yacc_sql.cpp"
    break;

  case 97: /* expr1: expr2  */
#line 847 "yacc_sql.y"
          {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2673 "yacc_sql.cpp"
    break;

  case 98: /* expr1: expr2 comp_op_binary expr2  */
#line 850 "yacc_sql.y"
                                  { (yyval.expression) = create_comparison_expression((yyvsp[-1].comp), (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2679 "yacc_sql.cpp"
    break;

  case 99: /* expr1: comp_op_unary expr2  */
#line 851 "yacc_sql.y"
                                  { (yyval.expression) = create_comparison_expression((yyvsp[-1].comp), new ValueExpr(Value(0)), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2685 "yacc_sql.cpp"
    break;

  case 100: /* expr2: expr3  */
#line 854 "yacc_sql.y"
          {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2693 "yacc_sql.cpp"
    break;

  case 101: /* expr2: expr2 '+' expr2  */
#line 857 "yacc_sql.y"
                      { (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::ADD, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2699 "yacc_sql.cpp"
    break;

  case 102: /* expr2: expr2 '-' expr2  */
#line 858 "yacc_sql.y"
                      { (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::SUB, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2705 "yacc_sql.cpp"
    break;

  case 103: /* expr2: expr2 '*' expr2  */
#line 859 "yacc_sql.y"
                      { (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::MUL, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2711 "yacc_sql.cpp"
    break;

  case 104: /* expr2: expr2 '/' expr2  */
#line 860 "yacc_sql.y"
                      { (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::DIV, (yyvsp[-2].expression), (yyvsp[0].expression), sql_string, &(yyloc)); }
#line 2717 "yacc_sql.cpp"
    break;

  case 105: /* expr2: '-' expr3  */
#line 862 "yacc_sql.y"
     { (yyval.expression) = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, (yyvsp[0].expression), nullptr, sql_string, &(yyloc)); }
#line 2723 "yacc_sql.cpp"
    break;

  case 106: /* expr2: value  */
#line 868 "yacc_sql.y"
            {
      (yyval.expression) = new ValueExpr(*(yyvsp[0].value));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2732 "yacc_sql.cpp"
    break;

  case 107: /* expr2: LBRACE select_stmt RBRACE  */
#line 872 "yacc_sql.y"
                                { // 子查询
      (yyval.expression) = new SubqueryExpr(token_name(sql_string, &(yylsp[-1])));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2741 "yacc_sql.cpp"
    break;

  case 108: /* expr3: LBRACE expression RBRACE  */
#line 878 "yacc_sql.y"
                             {
      (yyval.expression) = (yyvsp[-1].expression);
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2750 "yacc_sql.cpp"
    break;

  case 109: /* expr3: LBRACE value COMMA value value_list RBRACE  */
#line 882 "yacc_sql.y"
                                                 {
      std::vector<Value> val_list;
      if ((yyvsp[-1].value_list) != nullptr) val_list.swap(*(yyvsp[-1].value_list));
      val_list.emplace_back(*(yyvsp[-2].value));
      val_list.emplace_back(*(yyvsp[-4].value));
      std::reverse(val_list.begin(), val_list.end());
      (yyval.expression) = new ValueExpr(Value(val_list));
    }
#line 2763 "yacc_sql.cpp"
    break;

  case 110: /* expr3: function_expression  */
#line 890 "yacc_sql.y"
                          {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2771 "yacc_sql.cpp"
    break;

  case 111: /* expr3: rel_attr  */
#line 893 "yacc_sql.y"
               {
      (yyval.expression) = new FieldExpr(*(yyvsp[0].rel_attr));
      (yyval.expression)->set_name(token_name(sql_string, &(yyloc)));
    }
#line 2780 "yacc_sql.cpp"
    break;

  case 112: /* function_expression: function_type LBRACE expression_list RBRACE  */
#line 900 "yacc_sql.y"
                                                {
      std::reverse((yyvsp[-1].expression_list)->begin(), (yyvsp[-1].expression_list)->end());
      (yyval.expression) = create_function_expression((yyvsp[-3].function_type), (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2789 "yacc_sql.cpp"
    break;

  case 113: /* function_expression: aggre_type LBRACE expression_list RBRACE  */
#line 904 "yacc_sql.y"
                                               {
      (yyval.expression) = create_aggre_func_expression((yyvsp[-3].aggre_type), (yyvsp[-1].expression_list), sql_string, &(yyloc));
    }
#line 2797 "yacc_sql.cpp"
    break;

  case 114: /* function_expression: aggre_type LBRACE RBRACE  */
#line 907 "yacc_sql.y"
                               { // 为了迎合提测时空括号必须输出 FAILURE 而不是 Failed to parse sql 的奇葩情况。。。
      std::vector<Expression *> *empty_exprs = new std::vector<Expression *>;
      (yyval.expression) = create_aggre_func_expression((yyvsp[-2].aggre_type), empty_exprs, sql_string, &(yyloc));
    }
#line 2806 "yacc_sql.cpp"
    break;

  case 115: /* function_type: LENGTH  */
#line 913 "yacc_sql.y"
                  { (yyval.function_type) = FunctionExpr::Type::LENGTH; }
#line 2812 "yacc_sql.cpp"
    break;

  case 116: /* function_type: ROUND  */
#line 914 "yacc_sql.y"
                  { (yyval.function_type) = FunctionExpr::Type::ROUND; }
#line 2818 "yacc_sql.cpp"
    break;

  case 117: /* function_type: DATE_FORMAT  */
#line 915 "yacc_sql.y"
                  { (yyval.function_type) = FunctionExpr::Type::DATE_FORMAT; }
#line 2824 "yacc_sql.cpp"
    break;

  case 118: /* aggre_type: COUNT  */
#line 918 "yacc_sql.y"
            { (yyval.aggre_type) = AggreExpr::Type::CNT; }
#line 2830 "yacc_sql.cpp"
    break;

  case 119: /* aggre_type: SUM  */
#line 919 "yacc_sql.y"
            { (yyval.aggre_type) = AggreExpr::Type::SUM; }
#line 2836 "yacc_sql.cpp"
    break;

  case 120: /* aggre_type: AVG  */
#line 920 "yacc_sql.y"
            { (yyval.aggre_type) = AggreExpr::Type::AVG; }
#line 2842 "yacc_sql.cpp"
    break;

  case 121: /* aggre_type: MAX  */
#line 921 "yacc_sql.y"
            { (yyval.aggre_type) = AggreExpr::Type::MAX; }
#line 2848 "yacc_sql.cpp"
    break;

  case 122: /* aggre_type: MIN  */
#line 922 "yacc_sql.y"
            { (yyval.aggre_type) = AggreExpr::Type::MIN; }
#line 2854 "yacc_sql.cpp"
    break;

  case 123: /* rel_attr: '*'  */
#line 926 "yacc_sql.y"
        {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_alias  = "";
      (yyval.rel_attr)->attribute_name  = "*";
    }
#line 2864 "yacc_sql.cpp"
    break;

  case 124: /* rel_attr: ID DOT '*'  */
#line 931 "yacc_sql.y"
                 {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_alias  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name  = "*";
      free((yyvsp[-2].string));
    }
#line 2875 "yacc_sql.cpp"
    break;

  case 125: /* rel_attr: ID  */
#line 937 "yacc_sql.y"
         {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->attribute_name  = (yyvsp[0].string);
      free((yyvsp[0].string));
    }
#line 2885 "yacc_sql.cpp"
    break;

  case 126: /* rel_attr: ID DOT ID  */
#line 942 "yacc_sql.y"
                {
      (yyval.rel_attr) = new RelAttrSqlNode;
      (yyval.rel_attr)->relation_alias  = (yyvsp[-2].string);
      (yyval.rel_attr)->attribute_name  = (yyvsp[0].string);
      free((yyvsp[-2].string));
      free((yyvsp[0].string));
    }
#line 2897 "yacc_sql.cpp"
    break;

  case 127: /* as_alias: %empty  */
#line 952 "yacc_sql.y"
                { (yyval.string) = nullptr; }
#line 2903 "yacc_sql.cpp"
    break;

  case 128: /* as_alias: as alias  */
#line 953 "yacc_sql.y"
                { (yyval.string) = (yyvsp[0].string); }
#line 2909 "yacc_sql.cpp"
    break;

  case 129: /* as: %empty  */
#line 956 "yacc_sql.y"
                { }
#line 2915 "yacc_sql.cpp"
    break;

  case 130: /* as: AS  */
#line 957 "yacc_sql.y"
                { }
#line 2921 "yacc_sql.cpp"
    break;

  case 131: /* alias: ID  */
#line 960 "yacc_sql.y"
                { (yyval.string) = (yyvsp[0].string); }
#line 2927 "yacc_sql.cpp"
    break;

  case 132: /* alias: SSS  */
#line 961 "yacc_sql.y"
                { (yyval.string) = (yyvsp[0].string); }
#line 2933 "yacc_sql.cpp"
    break;

  case 133: /* rel_list: %empty  */
#line 966 "yacc_sql.y"
    {
      (yyval.relation_list) = nullptr;
    }
#line 2941 "yacc_sql.cpp"
    break;

  case 134: /* rel_list: COMMA relation rel_list  */
#line 969 "yacc_sql.y"
                              {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<RelationSqlNode>;
      }
      (yyval.relation_list)->push_back(*(yyvsp[-1].relation));
      free((yyvsp[-1].relation));
    }
#line 2955 "yacc_sql.cpp"
    break;

  case 135: /* rel_list: INNER JOIN relation join_condition rel_list  */
#line 978 "yacc_sql.y"
                                                  {
      if ((yyvsp[0].relation_list) != nullptr) {
        (yyval.relation_list) = (yyvsp[0].relation_list);
      } else {
        (yyval.relation_list) = new std::vector<RelationSqlNode>;
      }
      (yyvsp[-2].relation)->join_condition = (yyvsp[-1].expression);
      (yyval.relation_list)->push_back(*(yyvsp[-2].relation));
    }
#line 2969 "yacc_sql.cpp"
    break;

  case 136: /* relation: ID as_alias  */
#line 989 "yacc_sql.y"
                {
      (yyval.relation) = new RelationSqlNode;
      (yyval.relation)->relation_name = (yyvsp[-1].string);
      (yyval.relation)->relation_alias = (yyvsp[-1].string);
      if ((yyvsp[0].string) != nullptr) (yyval.relation)->relation_alias = (yyvsp[0].string);
    }
#line 2980 "yacc_sql.cpp"
    break;

  case 137: /* where: %empty  */
#line 998 "yacc_sql.y"
    {
      (yyval.expression) = nullptr;
    }
#line 2988 "yacc_sql.cpp"
    break;

  case 138: /* where: WHERE expression  */
#line 1001 "yacc_sql.y"
                       {
      (yyval.expression) = (yyvsp[0].expression);
    }
#line 2996 "yacc_sql.cpp"
    break;

  case 139: /* comp_op_binary: EQ  */
#line 1007 "yacc_sql.y"
         { (yyval.comp) = EQUAL_TO; }
#line 3002 "yacc_sql.cpp"
    break;

  case 140: /* comp_op_binary: LT  */
#line 1008 "yacc_sql.y"
         { (yyval.comp) = LESS_THAN; }
#line 3008 "yacc_sql.cpp"
    break;

  case 141: /* comp_op_binary: GT  */
#line 1009 "yacc_sql.y"
         { (yyval.comp) = GREAT_THAN; }
#line 3014 "yacc_sql.cpp"
    break;

  case 142: /* comp_op_binary: LE  */
#line 1010 "yacc_sql.y"
         { (yyval.comp) = LESS_EQUAL; }
#line 3020 "yacc_sql.cpp"
    break;

  case 143: /* comp_op_binary: GE  */
#line 1011 "yacc_sql.y"
         { (yyval.comp) = GREAT_EQUAL; }
#line 3026 "yacc_sql.cpp"
    break;

  case 144: /* comp_op_binary: NE  */
#line 1012 "yacc_sql.y"
         { (yyval.comp) = NOT_EQUAL; }
#line 3032 "yacc_sql.cpp"
    break;

  case 145: /* comp_op_binary: LIKE  */
#line 1013 "yacc_sql.y"
           { (yyval.comp) = LIKE_OPER; }
#line 3038 "yacc_sql.cpp"
    break;

  case 146: /* comp_op_binary: NOT LIKE  */
#line 1014 "yacc_sql.y"
               { (yyval.comp) = NOT_LIKE_OPER; }
#line 3044 "yacc_sql.cpp"
    break;

  case 147: /* comp_op_binary: IS  */
#line 1015 "yacc_sql.y"
         { (yyval.comp) = IS_OPER; }
#line 3050 "yacc_sql.cpp"
    break;

  case 148: /* comp_op_binary: IS NOT  */
#line 1016 "yacc_sql.y"
             { (yyval.comp) = IS_NOT_OPER; }
#line 3056 "yacc_sql.cpp"
    break;

  case 149: /* comp_op_binary: IN  */
#line 1017 "yacc_sql.y"
         { (yyval.comp) = IN_OPER; }
#line 3062 "yacc_sql.cpp"
    break;

  case 150: /* comp_op_binary: NOT IN  */
#line 1018 "yacc_sql.y"
             { (yyval.comp) = NOT_IN_OPER; }
#line 3068 "yacc_sql.cpp"
    break;

  case 151: /* comp_op_unary: EXISTS  */
#line 1022 "yacc_sql.y"
             { (yyval.comp) = EXISTS_OPER; }
#line 3074 "yacc_sql.cpp"
    break;

  case 152: /* load_data_stmt: LOAD DATA INFILE SSS INTO TABLE ID  */
#line 1027 "yacc_sql.y"
    {
      char *tmp_file_name = common::substr((yyvsp[-3].string), 1, strlen((yyvsp[-3].string)) - 2);
      
      (yyval.sql_node) = new ParsedSqlNode(SCF_LOAD_DATA);
      (yyval.sql_node)->load_data.relation_name = (yyvsp[0].string);
      (yyval.sql_node)->load_data.file_name = tmp_file_name;
      free((yyvsp[0].string));
      free(tmp_file_name);
    }
#line 3088 "yacc_sql.cpp"
    break;

  case 153: /* explain_stmt: EXPLAIN command_wrapper  */
#line 1040 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_EXPLAIN);
      (yyval.sql_node)->explain.sql_node = std::unique_ptr<ParsedSqlNode>((yyvsp[0].sql_node));
    }
#line 3097 "yacc_sql.cpp"
    break;

  case 154: /* set_variable_stmt: SET ID EQ value  */
#line 1048 "yacc_sql.y"
    {
      (yyval.sql_node) = new ParsedSqlNode(SCF_SET_VARIABLE);
      (yyval.sql_node)->set_variable.name  = (yyvsp[-2].string);
      (yyval.sql_node)->set_variable.value = *(yyvsp[0].value);
      free((yyvsp[-2].string));
      delete (yyvsp[0].value);
    }
#line 3109 "yacc_sql.cpp"
    break;


#line 3113 "yacc_sql.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (&yylloc, sql_string, sql_result, scanner, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
                      yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, sql_string, sql_result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, sql_string, sql_result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp, sql_string, sql_result, scanner);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1062 "yacc_sql.y"

//_____________________________________________________________________
extern void scan_string(const char *str, yyscan_t scanner);

int sql_parse(const char *s, ParsedSqlResult *sql_result) {
  yyscan_t scanner;
  yylex_init(&scanner);
  scan_string(s, scanner);
  int result = yyparse(s, sql_result, scanner);
  yylex_destroy(scanner);
  return result;
}
