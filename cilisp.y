/*

s-expressions calculator

program ::= | program s_expr EOL
s_expr ::= number
| symbol
| ( func s_expr )
| ( func s_expr s_expr )
| ( cond s_expr s_expr s_expr )
| ( ( let let_list ) s_expr )
let_section ::= ( let let_list )
let_list ::= let_elem | let_list let_elem
let_elem ::= ( type symbol s_expr )
type ::= | integer | real
symbol ::= letter+
letter ::= [a-zA-Z]
func ::= neg|abs|exp|sqrt|add|sub|mult|div|remainder|log|pow|max|min|exp2|cbrt|hypot|print|equal|smaller|larger
number ::= [+|-]?digit+[.number+]
digit ::= [0-9]




*/

%{
#include "cilisp.h"
%}

%union
{
   double dval;
   char *sval;
   struct ast_node *astNode;
   struct symbol_ast_node* symNode;
};

%token <sval> TYPE
%token <sval> FUNC
%token <sval> COND
%token <sval> SYMBOL
%token <dval> NUMBER
%token LPAREN RPAREN EOL QUIT LET

%type <astNode> s_expr
%type <symNode> let_elem
%type <symNode> let_list

%%

program:/* empty */ { 
                       printf("> ");
                    }
        | program s_expr EOL {
                              // printf("yacc: program expr\n"); 
                                 printf("\n%lf",eval($2)->value);
                              freeNode($2);
                              printf("\n> "); 
                           }
        ;

s_expr:
        NUMBER { 
                  printf("yacc: NUMBER%lf", $1); 
                  $$ = number($1); 
               }
        | SYMBOL
        {
          printf("SYMBOL %s\n", $1);
          $$ = symbol($1);
        }
        | LPAREN FUNC s_expr RPAREN { 
                                      printf("yacc: LPAREN FUNC expr RPAREN\n"); 
                                     $$ = function($2, $3, 0);  
                                     //printf("%s(%lf)", $2, $3);
                                  }
        | LPAREN FUNC s_expr s_expr RPAREN {
                                           printf("LPAREN FUNC expr expr RPAREN\n"); 
                                          
                                          $$ = function($2, $3, $4);
                                       }
        | LPAREN COND s_expr s_expr s_expr RPAREN {
                                        printf("LPAREN COND expr expr expr RPAREN\n ");

                                        $$ = cond($3 , $4 , $5)
        
                                        }                                
        | LPAREN LPAREN LET let_list RPAREN s_expr RPAREN
        {
          printf("LPAREN LPAREN LET let_list RPAREN s_expr RPAREN\n");
          $$ = let($4, $6);
        }       
                                
        | QUIT { 
                  //printf("QUIT\n"); 
                  exit(0);
               }
        
        | error { 
                        printf("error\n"); 
                        //printf("> ");
                    }

        ;

let_list:
        let_elem
        {
          printf("let_elem\n");
          $$ = let_list($1,NULL);
        }

        | let_list let_elem
        {
            printf("let_list let_elem\n");
            $$ = let_list($2, $1);
        }
        ;

let_elem:

        LPAREN TYPE SYMBOL s_expr RPAREN
        {
        printf("let_elem type\n");
        $$ = let_elem($2, $3, $4);
        
        }
        | LPAREN SYMBOL s_expr RPAREN
        {
        $$ = let_elem(NULL, $2, $3);
        }
        ;        


%%

