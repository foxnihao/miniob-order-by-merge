/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YACC_SQL_HPP_INCLUDED
# define YY_YY_YACC_SQL_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SEMICOLON = 258,               /* SEMICOLON  */
    CREATE = 259,                  /* CREATE  */
    DROP = 260,                    /* DROP  */
    TABLE = 261,                   /* TABLE  */
    TABLES = 262,                  /* TABLES  */
    UNIQUE = 263,                  /* UNIQUE  */
    INDEX = 264,                   /* INDEX  */
    CALC = 265,                    /* CALC  */
    SELECT = 266,                  /* SELECT  */
    DESC = 267,                    /* DESC  */
    SHOW = 268,                    /* SHOW  */
    SYNC = 269,                    /* SYNC  */
    INSERT = 270,                  /* INSERT  */
    DELETE = 271,                  /* DELETE  */
    UPDATE = 272,                  /* UPDATE  */
    LBRACE = 273,                  /* LBRACE  */
    RBRACE = 274,                  /* RBRACE  */
    COMMA = 275,                   /* COMMA  */
    TRX_BEGIN = 276,               /* TRX_BEGIN  */
    TRX_COMMIT = 277,              /* TRX_COMMIT  */
    TRX_ROLLBACK = 278,            /* TRX_ROLLBACK  */
    INT_T = 279,                   /* INT_T  */
    STRING_T = 280,                /* STRING_T  */
    FLOAT_T = 281,                 /* FLOAT_T  */
    DATE_T = 282,                  /* DATE_T  */
    HELP = 283,                    /* HELP  */
    EXIT = 284,                    /* EXIT  */
    DOT = 285,                     /* DOT  */
    INTO = 286,                    /* INTO  */
    VALUES = 287,                  /* VALUES  */
    FROM = 288,                    /* FROM  */
    WHERE = 289,                   /* WHERE  */
    LIKE = 290,                    /* LIKE  */
    AS = 291,                      /* AS  */
    LENGTH = 292,                  /* LENGTH  */
    ROUND = 293,                   /* ROUND  */
    DATE_FORMAT = 294,             /* DATE_FORMAT  */
    NOT = 295,                     /* NOT  */
    IS = 296,                      /* IS  */
    NULL_ = 297,                   /* NULL_  */
    COUNT = 298,                   /* COUNT  */
    SUM = 299,                     /* SUM  */
    AVG = 300,                     /* AVG  */
    MAX = 301,                     /* MAX  */
    MIN = 302,                     /* MIN  */
    INNER = 303,                   /* INNER  */
    JOIN = 304,                    /* JOIN  */
    AND = 305,                     /* AND  */
    OR = 306,                      /* OR  */
    SET = 307,                     /* SET  */
    ON = 308,                      /* ON  */
    LOAD = 309,                    /* LOAD  */
    DATA = 310,                    /* DATA  */
    INFILE = 311,                  /* INFILE  */
    EXPLAIN = 312,                 /* EXPLAIN  */
    GROUP = 313,                   /* GROUP  */
    HAVING = 314,                  /* HAVING  */
    ORDER = 315,                   /* ORDER  */
    BY = 316,                      /* BY  */
    ASC = 317,                     /* ASC  */
    EQ = 318,                      /* EQ  */
    LT = 319,                      /* LT  */
    GT = 320,                      /* GT  */
    LE = 321,                      /* LE  */
    GE = 322,                      /* GE  */
    NE = 323,                      /* NE  */
    IN = 324,                      /* IN  */
    EXISTS = 325,                  /* EXISTS  */
    UNSIGNED_NUMBER = 326,         /* UNSIGNED_NUMBER  */
    UNSIGNED_FLOAT = 327,          /* UNSIGNED_FLOAT  */
    ID = 328,                      /* ID  */
    DATE_STR = 329,                /* DATE_STR  */
    SSS = 330,                     /* SSS  */
    UMINUS = 331                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 170 "yacc_sql.y"

  ParsedSqlNode *                   sql_node;
  OrderFlag                         order_flag;
  Value *                           value;
  enum CompOp                       comp;
  RelAttrSqlNode *                  rel_attr;
  RelationSqlNode *                 relation;
  RelationSqlNode *                 join;
  GroupsSqlNode *                   groups;
  OrderSqlNode *                    order;
  std::vector<AttrInfoSqlNode> *    attr_infos;
  AttrInfoSqlNode *                 attr_info;
  char *                            attr_name;
  std::vector<std::string> *        attr_names;
  Expression *                      expression;
  std::vector<Expression *> *       expression_list;
  enum FunctionExpr::Type           function_type;
  enum AggreExpr::Type              aggre_type;
  std::vector<Value> *              value_list;
  ParsedUpdateItem  *               update_value;
  std::vector<ParsedUpdateItem *> * update_values;
  std::vector<RelAttrSqlNode> *     rel_attr_list;
  std::vector<RelationSqlNode> *    relation_list;
  std::vector<RelationSqlNode> *    join_list;
  std::vector<OrderSqlNode> *       order_list;
  char *                            string;
  int                               number;
  float                             floats;
  bool                              BOOLEN;

#line 171 "yacc_sql.hpp"

};
typedef union YYSTYPE YYSTYPE;
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




int yyparse (const char * sql_string, ParsedSqlResult * sql_result, void * scanner);


#endif /* !YY_YY_YACC_SQL_HPP_INCLUDED  */
