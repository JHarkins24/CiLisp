%option noyywrap

%{
    #include "cilisp.h"
%}

digit [0-9]
number [+|-]?{digit}+(\.{digit}+)?
func "neg"|"abs"|"exp"|"sqrt"|"add"|"sub"|"mult"|"div"|"remainder"|"log"|"pow"|"max"|"min"|"exp2"|"hypot"|"cbrt"|"print"| "equal"|"smaller"|"larger"
letter [a-zA-Z]
type "integer"|"real"
symbol {letter}+

%%

{number}    {
                yylval.dval = strtod(yytext, NULL);
                //printf("lex: NUMBER dval = %lf\n", yylval.dval);
                return NUMBER;
            }

"quit"      { 
                return QUIT; 
            }
            
"let"       {
                //printf("lex: let\n");
                return LET;
            }

{func}      {
                yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
                strcpy(yylval.sval, yytext);
                //printf("lex: FUNC sval = %s\n", yylval.sval);
                return FUNC;
            }

{type}      {
                yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
                strcpy(yylval.sval, yytext);
                //printf("lex: TYPE sval = %s\n", yylval.sval);
                return TYPE;
            }  
                        
{symbol}    {
                yylval.sval = (char *) malloc(strlen(yytext)*sizeof(char));
                strcpy(yylval.sval, yytext);
                //printf("lex: SYMBOL ");
                return SYMBOL;
            }
          

"("         { 
                //printf("lex: LPAREN\n"); 
                return LPAREN;
            }

")"         { 
                //printf("lex: RPAREN\n"); 
                return RPAREN;
            }
"\n"        {
                //printf("lex: RPAREN\n"); 
                return EOL;
            }

[ \t]       ; /* skip whitespace */

.           // yyerror("lex: invalid character");

%%
