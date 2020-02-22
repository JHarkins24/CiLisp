//Joe Harkins
//11/24/17

#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "cilisp.tab.h"

int yyparse(void);
int yylex(void);
void yyerror(char *);

typedef enum { NUM_TYPE, FUNC_TYPE, LET_TYPE, SYM_TYPE } AST_NODE_TYPE;
typedef enum {REAL, INTEGER, UNDECLARED} TYPE_SYM;
typedef enum {NEG = 0, ABS, EXP, SQRT, EXP2, CBRT, ADD, SUB, MULT, DIV, MOD, LOG, POW, MAX, MIN, HYPOT , PRINT , EQUAL , SMALLER , LARGER} FUNC_NAMES;

typedef struct
{
    double value;
    TYPE_SYM type;
} NUMBER_AST_NODE;

typedef struct
{
   char *name;
   struct ast_node *op1;
   struct ast_node *op2;
} FUNCTION_AST_NODE;
typedef struct symbol_ast_node
{
   char *funcName;
   struct ast_node* value;
   struct symbol_ast_node* next;
   TYPE_SYM type;
} SYMBOL_AST_NODE;

typedef struct SUPERSCOPE_node
{
   struct symbol_ast_node* symbol;
   struct SUPERSCOPE_node* parent;
} SUPERSCOPE_node;

typedef struct let_ast_node
{
   SUPERSCOPE_node* scope;
   struct ast_node* s_expr;
} LET_AST_NODE;
typedef struct ast_node
{
   AST_NODE_TYPE type;
   union
   {
      NUMBER_AST_NODE number;
      FUNCTION_AST_NODE function;
      SYMBOL_AST_NODE symbol;
      LET_AST_NODE let;
   } data;
} AST_NODE;

AST_NODE *number(double value);
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2);
void freeNode(AST_NODE *p);
double getSymbolValue(char* funcName);
void leaveScope();
void enterScope(SUPERSCOPE_node* newScope);
AST_NODE* let(SYMBOL_AST_NODE *symbols, AST_NODE *s_expr);
SYMBOL_AST_NODE* let_list(SYMBOL_AST_NODE *symbol, SYMBOL_AST_NODE *let_list);
SYMBOL_AST_NODE* let_elem(char* type, char *symbol, AST_NODE *s_expr);
AST_NODE* symbol(char *name);
NUMBER_AST_NODE* eval(AST_NODE *ast); 
TYPE_SYM getThatType(char* funcName);
void print(AST_NODE *s_expr);
#endif
