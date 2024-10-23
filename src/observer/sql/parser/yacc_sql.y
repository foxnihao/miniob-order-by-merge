
%{

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

%}

%define api.pure full
%define parse.error verbose
/** 启用位置标识 **/
%locations
%lex-param { yyscan_t scanner }
/** 这些定义了在yyparse函数中的参数 **/
%parse-param { const char * sql_string }
%parse-param { ParsedSqlResult * sql_result }
%parse-param { void * scanner }

//标识tokens
%token  SEMICOLON
        CREATE
        DROP
        TABLE
        TABLES
        UNIQUE
        INDEX
        CALC
        SELECT
        DESC
        SHOW
        SYNC
        INSERT
        DELETE
        UPDATE
        LBRACE
        RBRACE
        COMMA
        TRX_BEGIN
        TRX_COMMIT
        TRX_ROLLBACK
        INT_T
        STRING_T
        FLOAT_T
        DATE_T
        HELP
        EXIT
        DOT //QUOTE
        INTO
        VALUES
        FROM
        WHERE
        LIKE
        AS
        LENGTH
        ROUND
        DATE_FORMAT
        NOT
        IS
        NULL_
        COUNT
        SUM
        AVG
        MAX
        MIN
        INNER
        JOIN
        AND
        OR
        SET
        ON
        LOAD
        DATA
        INFILE
        EXPLAIN
        GROUP
        HAVING
        ORDER
        BY
        ASC
        EQ
        LT
        GT
        LE
        GE
        NE
        IN
        EXISTS

/** union 中定义各种数据类型，真实生成的代码也是union类型，所以不能有非POD类型的数据 **/
%union {
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
}

%token <number> UNSIGNED_NUMBER
%token <floats> UNSIGNED_FLOAT
%token <string> ID
%token <string> DATE_STR
%token <string> SSS
//非终结符

/** type 定义了各种解析后的结果输出的是什么类型。类型对应了 union 中的定义的成员变量名称 **/
%type <number>              type
%type <value>               value
%type <number>              number
%type <comp>                comp_op_binary
%type <comp>                comp_op_unary
%type <expression>          having
%type <groups>              group_list
%type <order_flag>          order_flag
%type <order_list>          order_items
%type <order>               order_item
%type <order_list>          order_list
%type <expression>          join_condition
%type <join>                join_table
%type <join_list>           join_list
%type <rel_attr>            rel_attr
%type <attr_infos>          attr_def_list
%type <attr_info>           attr_def
%type <BOOLEN>              null_;
%type <attr_names>          attr_index_list
%type <attr_name>           attr_index
%type <update_value>        update_item
%type <update_values>       update_list
%type <value_list>          value_list
%type <expression>          where
%type <expression_list>     select_exprs
%type <expression>          select_expr
%type <expression_list>     select_expr_list
%type <relation_list>       from
%type <relation_list>       rel_list
%type <expression>          expression
%type <expression>          expr1;
%type <expression>          expr2;
%type <expression>          expr3;
%type <expression_list>     expression_list
%type <expression>          function_expression
%type <function_type>       function_type
%type <aggre_type>          aggre_type
%type <relation>            relation
%type <string>              as_alias
%type <string>              alias
%type <sql_node>            calc_stmt
%type <sql_node>            select_stmt
%type <sql_node>            insert_stmt
%type <sql_node>            update_stmt
%type <sql_node>            delete_stmt
%type <sql_node>            create_table_stmt
%type <sql_node>            drop_table_stmt
%type <sql_node>            show_tables_stmt
%type <sql_node>            desc_table_stmt
%type <sql_node>            create_index_stmt
%type <sql_node>            drop_index_stmt
%type <sql_node>            sync_stmt
%type <sql_node>            begin_stmt
%type <sql_node>            commit_stmt
%type <sql_node>            rollback_stmt
%type <sql_node>            load_data_stmt
%type <sql_node>            explain_stmt
%type <sql_node>            set_variable_stmt
%type <sql_node>            help_stmt
%type <sql_node>            exit_stmt
%type <sql_node>            command_wrapper
// commands should be a list but I use a single command instead
%type <sql_node>            commands

%left OR
%left AND
%right NOT
%nonassoc EQ LT GT LE GE NE
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS
%%

commands: command_wrapper opt_semicolon //commands or sqls. parser starts here.
  {
    std::unique_ptr<ParsedSqlNode> sql_node = std::unique_ptr<ParsedSqlNode>($1);
    sql_result->add_sql_node(std::move(sql_node));
  }
  ;

command_wrapper:
    calc_stmt
  | select_stmt
  | insert_stmt
  | update_stmt
  | delete_stmt
  | create_table_stmt
  | drop_table_stmt
  | show_tables_stmt
  | desc_table_stmt
  | create_index_stmt
  | drop_index_stmt
  | sync_stmt
  | begin_stmt
  | commit_stmt
  | rollback_stmt
  | load_data_stmt
  | explain_stmt
  | set_variable_stmt
  | help_stmt
  | exit_stmt
    ;

exit_stmt:      
    EXIT {
      (void)yynerrs;  // 这么写为了消除yynerrs未使用的告警。如果你有更好的方法欢迎提PR
      $$ = new ParsedSqlNode(SCF_EXIT);
    };

help_stmt:
    HELP {
      $$ = new ParsedSqlNode(SCF_HELP);
    };

sync_stmt:
    SYNC {
      $$ = new ParsedSqlNode(SCF_SYNC);
    }
    ;

begin_stmt:
    TRX_BEGIN  {
      $$ = new ParsedSqlNode(SCF_BEGIN);
    }
    ;

commit_stmt:
    TRX_COMMIT {
      $$ = new ParsedSqlNode(SCF_COMMIT);
    }
    ;

rollback_stmt:
    TRX_ROLLBACK  {
      $$ = new ParsedSqlNode(SCF_ROLLBACK);
    }
    ;

drop_table_stmt:    /*drop table 语句的语法解析树*/
    DROP TABLE ID {
      $$ = new ParsedSqlNode(SCF_DROP_TABLE);
      $$->drop_table.relation_name = $3;
      free($3);
    };

show_tables_stmt:
    SHOW TABLES {
      $$ = new ParsedSqlNode(SCF_SHOW_TABLES);
    }
    ;

desc_table_stmt:
    DESC ID  {
      $$ = new ParsedSqlNode(SCF_DESC_TABLE);
      $$->desc_table.relation_name = $2;
      free($2);
    }
    ;

create_index_stmt:    /*create index 语句的语法解析树*/
    CREATE INDEX ID ON ID LBRACE attr_index attr_index_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $3;
      create_index.relation_name = $5;
      create_index.attribute_names.emplace_back($7);
      std::vector<std::string> *src_attrs = $8;

      if(src_attrs != nullptr){
        create_index.attribute_names.insert(create_index.attribute_names.end(), src_attrs->begin(), src_attrs->end());
        delete src_attrs;
      }
      std::reverse(create_index.attribute_names.begin(),create_index.attribute_names.end());
      create_index.is_unique = false;
      free($3);
      free($5);
    }
    |CREATE UNIQUE INDEX ID ON ID LBRACE attr_index attr_index_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_INDEX);
      CreateIndexSqlNode &create_index = $$->create_index;
      create_index.index_name = $4;
      create_index.relation_name = $6;
      create_index.attribute_names.emplace_back($8);
      std::vector<std::string> *src_attrs = $9;

      if(src_attrs != nullptr){
        create_index.attribute_names.insert(create_index.attribute_names.end(), src_attrs->begin(), src_attrs->end());
        delete src_attrs;
      }
      std::reverse(create_index.attribute_names.begin(),create_index.attribute_names.end());
      create_index.is_unique = true;
      free($4);
      free($6);
    }
    ;

attr_index_list: 
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA attr_index attr_index_list
    {
      if($3 != nullptr){
        $$ = $3;
      }else {
        $$ = new std::vector<std::string>;
      }
      $$->emplace_back(std::string($2));
      free($2);
    }
    ;
  
attr_index:
    ID
    {
     $$ =  $1;
     //free($1);
    }
    ;
drop_index_stmt:      /*drop index 语句的语法解析树*/
    DROP INDEX ID ON ID
    {
      $$ = new ParsedSqlNode(SCF_DROP_INDEX);
      $$->drop_index.index_name = $3;
      $$->drop_index.relation_name = $5;
      free($3);
      free($5);
    }
    ;
create_table_stmt:    /*create table 语句的语法解析树*/
    CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;
    }
    | CREATE TABLE ID LBRACE attr_def attr_def_list RBRACE as select_stmt
    {
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      std::vector<AttrInfoSqlNode> *src_attrs = $6;

      if (src_attrs != nullptr) {
        create_table.attr_infos.swap(*src_attrs);
      }
      create_table.attr_infos.emplace_back(*$5);
      std::reverse(create_table.attr_infos.begin(), create_table.attr_infos.end());
      delete $5;

      create_table.as_expression = new SubqueryExpr(token_name(sql_string, &@9));
    }
    | CREATE TABLE ID as select_stmt{
      $$ = new ParsedSqlNode(SCF_CREATE_TABLE);
      CreateTableSqlNode &create_table = $$->create_table;
      create_table.relation_name = $3;
      free($3);

      create_table.as_expression = new SubqueryExpr(token_name(sql_string, &@5));
    }
    ;
attr_def_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA attr_def attr_def_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<AttrInfoSqlNode>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
    
attr_def:
    ID type LBRACE number RBRACE null_
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = $4;
      $$->is_null = $6;
      free($1);
    }
    | ID type null_
    {
      $$ = new AttrInfoSqlNode;
      $$->type = (AttrType)$2;
      $$->name = $1;
      $$->length = 4;
      $$->is_null = $3;
      free($1);
    }
    ;
null_:
    /* empty */ 
    {
      $$ = false;
    }
    |NULL_ 
    {
      $$ = true;
    }
    |NOT NULL_
    {
      $$ = false;
    }
    ;

number:
    UNSIGNED_NUMBER {$$ = $1;}
    | '-' UNSIGNED_NUMBER {$$ = -$2;}
    ;
type:
    INT_T      { $$=INTS; }
    | STRING_T { $$=CHARS; }
    | FLOAT_T  { $$=FLOATS; }
    | DATE_T   { $$=DATES; }
    ;
insert_stmt:        /*insert   语句的语法解析树*/
    INSERT INTO ID VALUES LBRACE value value_list RBRACE 
    {
      $$ = new ParsedSqlNode(SCF_INSERT);
      $$->insertion.relation_name = $3;
      if ($7 != nullptr) {
        $$->insertion.values.swap(*$7);
      }
      $$->insertion.values.emplace_back(*$6);
      std::reverse($$->insertion.values.begin(), $$->insertion.values.end());
      delete $6;
      free($3);
    }
    ;

value_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA value value_list  { 
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Value>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
value:
    UNSIGNED_NUMBER {
      $$ = new Value((int)$1);
      @$ = @1;
    }
    | '-' UNSIGNED_NUMBER %prec UMINUS {
      $$ = new Value(-(int)$2);
      @$ = @1;
    }
    | UNSIGNED_FLOAT {
      $$ = new Value((float)$1);
      @$ = @1;
    }
    | '-' UNSIGNED_FLOAT %prec UMINUS {
      $$ = new Value(-(float)$2);
      @$ = @1;
    }
    | SSS {
      char *tmp = common::substr($1,1,strlen($1)-2);
      $$ = new Value(tmp);
      free(tmp);
    }
    | DATE_STR {
      $$ = new Value($1, true);
    }
    |NULL_ {
      $$ = new Value(Value::null_value());
    }
    ;
    
delete_stmt:    /*  delete 语句的语法解析树*/
    DELETE FROM ID as_alias where 
    {
      $$ = new ParsedSqlNode(SCF_DELETE);
      $$->deletion.relation_name = $3;
      $$->deletion.relation_alias = $3;
      if ($4 != nullptr) $$->deletion.relation_alias = $4;
      $$->deletion.condition = $5;
    }
    ;

update_stmt:      /*  update 语句的语法解析树*/
    UPDATE ID as_alias SET update_list where 
    {
      $$ = new ParsedSqlNode(SCF_UPDATE);
      $$->update.relation_name = $2;
      $$->update.relation_alias = $2;
      if ($3 != nullptr) $$->update.relation_alias = $3;
      std::reverse($5->begin(), $5->end());
      $$->update.update_items.swap(*$5);
      $$->update.condition = $6;
    }
    ;
update_list:
    update_item
    {
      $$ = new std::vector<ParsedUpdateItem *>;
      $$->emplace_back($1);
    } 
    | update_item COMMA  update_list
    {
      if($3 != nullptr){
        $$ = $3;
      }
      else{
        $$ = new std::vector<ParsedUpdateItem *>;
      }
      $$->emplace_back($1);
    }
    ;
update_item:
    ID EQ expression
    {
      $$ = new ParsedUpdateItem();
      $$->attribute_name = $1;
      $$->expression = $3;
    }
    ;

select_stmt:        /*  select 语句的语法解析树*/    // TODO join
    SELECT select_exprs from where group_list order_list // `FROM ...` is optional
    {
      $$ = new ParsedSqlNode(SCF_SELECT);
      if ($2 != nullptr) {
        $$->selection.expressions.swap(*$2);
      }
      if ($3 != nullptr) {
        $$->selection.relations.swap(*$3);
        std::reverse($$->selection.relations.begin(), $$->selection.relations.end());
      }
      $$->selection.condition = $4;
      if ($5 != nullptr) {
        $$->selection.groups = *$5;
      }
      if ($6 != nullptr) {
        $$->selection.orders.swap(*$6);
        std::reverse($$->selection.orders.begin(), $$->selection.orders.end());
      }
    }
    ;
from:
    /* empty */
    {
      $$ = nullptr;
    }
    | FROM relation rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelationSqlNode>;
      }
      $$->emplace_back(*$2);
    }
    ;
group_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | GROUP BY expression_list having
    {
      $$ = new GroupsSqlNode;
      $$->group_exprs.swap(*$3);
      std::reverse($$->group_exprs.begin(), $$->group_exprs.end());
      $$->having = $4;
    }
    ;
having:
    /* empty */           { $$ = nullptr; }
    | HAVING expression   { $$ = $2; }
order_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | ORDER BY order_item order_items
    {
      if ($4 != nullptr) {
        $$ = $4;
      } else {
        $$ = new std::vector<OrderSqlNode>;
      }
      $$->emplace_back(*$3);
      delete $3;
    }
    ;
order_item:
    expression order_flag
    {
      $$ = new OrderSqlNode;
      $$->expression = $1;
      $$->flag = $2;
    }
    ;
order_flag:
    /* empty */ { $$ = ORDER_ASC; }
    | ASC       { $$ = ORDER_ASC; }
    | DESC      { $$ = ORDER_DESC; }
    ;
order_items:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA order_item order_items
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<OrderSqlNode>;
      }
      $$->emplace_back(*$2);
      delete $2;
    }
    ;
join_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | join_table join_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<RelationSqlNode>;
      }
      $$->emplace_back(*$1);
      delete $1;
    }
    ;
join_table:
    INNER JOIN relation join_condition{
      $$ = $3;
      $$->join_condition = $4;
    }
    ;
join_condition:
    /* empty */     { $$ = nullptr; }
    | ON expression { $$ = $2; }
    ;
select_exprs:
    select_expr select_expr_list {
      if ($2 != nullptr) {
        $$ = $2;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $$->emplace_back($1);
    }
    ;
select_expr:
    expression as_alias {
      $$ = $1;
      if ($2 != nullptr) $$->set_name($2);
    }
select_expr_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA select_expr select_expr_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $$->emplace_back($2);
    }
    ;

calc_stmt:
    CALC expression_list
    {
      $$ = new ParsedSqlNode(SCF_CALC);
      std::reverse($2->begin(), $2->end());
      $$->calc.expressions.swap(*$2);
      delete $2;
    }
    ;

expression_list:
    expression
    {
      $$ = new std::vector<Expression*>;
      $$->emplace_back($1);
    }
    | expression COMMA expression_list
    {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<Expression *>;
      }
      $$->emplace_back($1);
    }
    ;
expression: // 按优先级分层分析
    expr1 {
      $$ = $1;
    }
    | expression OR expression  { $$ = create_conjunction_expression(ConjunctionExpr::Type::OR, $1, $3, sql_string, &@$); }
    | expression AND expression { $$ = create_conjunction_expression(ConjunctionExpr::Type::AND, $1, $3, sql_string, &@$); }
    | NOT expression
    {
      std::vector<Expression *> *args = new std::vector<Expression *>;
      args->push_back($2);
      $$ = create_function_expression(FunctionExpr::Type::NOT, args, sql_string, &@$);
    }
    ;
expr1:
    expr2 {
      $$ = $1;
    }
    | expr2 comp_op_binary expr2  { $$ = create_comparison_expression($2, $1, $3, sql_string, &@$); }
    | comp_op_unary expr2         { $$ = create_comparison_expression($1, new ValueExpr(Value(0)), $2, sql_string, &@$); }
    ;
expr2:
    expr3 {
      $$ = $1;
    }
    | expr2 '+' expr2 { $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, $3, sql_string, &@$); }
    | expr2 '-' expr2 { $$ = create_arithmetic_expression(ArithmeticExpr::Type::SUB, $1, $3, sql_string, &@$); }
    | expr2 '*' expr2 { $$ = create_arithmetic_expression(ArithmeticExpr::Type::MUL, $1, $3, sql_string, &@$); }
    | expr2 '/' expr2 { $$ = create_arithmetic_expression(ArithmeticExpr::Type::DIV, $1, $3, sql_string, &@$); }
    | '-' expr3 %prec UMINUS
     { $$ = create_arithmetic_expression(ArithmeticExpr::Type::NEGATIVE, $2, nullptr, sql_string, &@$); }
    /*| expr2 negative_value { // 减法的特殊情况，即一个表达式和一个“负数”（实际上是减号和一个正数）
      ValueExpr *expr = new ValueExpr(*$2);
      expr->set_name($2->to_string());
      $$ = create_arithmetic_expression(ArithmeticExpr::Type::ADD, $1, expr, sql_string, &@$);
    }*/
    | value {
      $$ = new ValueExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
    }
    | LBRACE select_stmt RBRACE { // 子查询
      $$ = new SubqueryExpr(token_name(sql_string, &@2));
      $$->set_name(token_name(sql_string, &@$));
    }
    ;
expr3:
    LBRACE expression RBRACE {
      $$ = $2;
      $$->set_name(token_name(sql_string, &@$));
    }
    | LBRACE value COMMA value value_list RBRACE {
      std::vector<Value> val_list;
      if ($5 != nullptr) val_list.swap(*$5);
      val_list.emplace_back(*$4);
      val_list.emplace_back(*$2);
      std::reverse(val_list.begin(), val_list.end());
      $$ = new ValueExpr(Value(val_list));
    }
    | function_expression {
      $$ = $1;
    }
    | rel_attr {
      $$ = new FieldExpr(*$1);
      $$->set_name(token_name(sql_string, &@$));
    }
    ;

function_expression:
    function_type LBRACE expression_list RBRACE {
      std::reverse($3->begin(), $3->end());
      $$ = create_function_expression($1, $3, sql_string, &@$);
    }
    | aggre_type LBRACE expression_list RBRACE {
      $$ = create_aggre_func_expression($1, $3, sql_string, &@$);
    }
    | aggre_type LBRACE RBRACE { // 为了迎合提测时空括号必须输出 FAILURE 而不是 Failed to parse sql 的奇葩情况。。。
      std::vector<Expression *> *empty_exprs = new std::vector<Expression *>;
      $$ = create_aggre_func_expression($1, empty_exprs, sql_string, &@$);
    }
    ;
function_type:
      LENGTH      { $$ = FunctionExpr::Type::LENGTH; }
    | ROUND       { $$ = FunctionExpr::Type::ROUND; }
    | DATE_FORMAT { $$ = FunctionExpr::Type::DATE_FORMAT; } 
    ;
aggre_type:
      COUNT { $$ = AggreExpr::Type::CNT; }
    | SUM   { $$ = AggreExpr::Type::SUM; }
    | AVG   { $$ = AggreExpr::Type::AVG; }
    | MAX   { $$ = AggreExpr::Type::MAX; }
    | MIN   { $$ = AggreExpr::Type::MIN; }
    ;
    
rel_attr:
    '*' {
      $$ = new RelAttrSqlNode;
      $$->relation_alias  = "";
      $$->attribute_name  = "*";
    }
    | ID DOT '*' {
      $$ = new RelAttrSqlNode;
      $$->relation_alias  = $1;
      $$->attribute_name  = "*";
      free($1);
    }
    | ID {
      $$ = new RelAttrSqlNode;
      $$->attribute_name  = $1;
      free($1);
    }
    | ID DOT ID {
      $$ = new RelAttrSqlNode;
      $$->relation_alias  = $1;
      $$->attribute_name  = $3;
      free($1);
      free($3);
    }
    ;

as_alias:
    /* empty */ { $$ = nullptr; }
    | as alias  { $$ = $2; }
    ;
as:
    /* empty */ { }
    | AS        { }
    ;
alias:
    ID          { $$ = $1; }
    | SSS       { $$ = $1; }
    ;

rel_list:
    /* empty */
    {
      $$ = nullptr;
    }
    | COMMA relation rel_list {
      if ($3 != nullptr) {
        $$ = $3;
      } else {
        $$ = new std::vector<RelationSqlNode>;
      }
      $$->push_back(*$2);
      free($2);
    }
    | INNER JOIN relation join_condition rel_list {
      if ($5 != nullptr) {
        $$ = $5;
      } else {
        $$ = new std::vector<RelationSqlNode>;
      }
      $3->join_condition = $4;
      $$->push_back(*$3);
    }
    ;
relation:
    ID as_alias {
      $$ = new RelationSqlNode;
      $$->relation_name = $1;
      $$->relation_alias = $1;
      if ($2 != nullptr) $$->relation_alias = $2;
    }

where:
    /* empty */
    {
      $$ = nullptr;
    }
    | WHERE expression {
      $$ = $2;
    }
    ;

comp_op_binary:
      EQ { $$ = EQUAL_TO; }
    | LT { $$ = LESS_THAN; }
    | GT { $$ = GREAT_THAN; }
    | LE { $$ = LESS_EQUAL; }
    | GE { $$ = GREAT_EQUAL; }
    | NE { $$ = NOT_EQUAL; }
    | LIKE { $$ = LIKE_OPER; }
    | NOT LIKE { $$ = NOT_LIKE_OPER; }
    | IS { $$ = IS_OPER; }
    | IS NOT { $$ = IS_NOT_OPER; }
    | IN { $$ = IN_OPER; }
    | NOT IN { $$ = NOT_IN_OPER; }
    ;

comp_op_unary:
      EXISTS { $$ = EXISTS_OPER; }
    ;

load_data_stmt:
    LOAD DATA INFILE SSS INTO TABLE ID 
    {
      char *tmp_file_name = common::substr($4, 1, strlen($4) - 2);
      
      $$ = new ParsedSqlNode(SCF_LOAD_DATA);
      $$->load_data.relation_name = $7;
      $$->load_data.file_name = tmp_file_name;
      free($7);
      free(tmp_file_name);
    }
    ;

explain_stmt:
    EXPLAIN command_wrapper
    {
      $$ = new ParsedSqlNode(SCF_EXPLAIN);
      $$->explain.sql_node = std::unique_ptr<ParsedSqlNode>($2);
    }
    ;

set_variable_stmt:
    SET ID EQ value
    {
      $$ = new ParsedSqlNode(SCF_SET_VARIABLE);
      $$->set_variable.name  = $2;
      $$->set_variable.value = *$4;
      free($2);
      delete $4;
    }
    ;

opt_semicolon: /*empty*/
    //| SEMICOLON
    SEMICOLON
    | '\n'
    ;
%%
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
