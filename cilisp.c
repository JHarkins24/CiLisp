//Joe Harkins
//11/24/17


#include "cilisp.h"
#include <math.h>
#include <string.h>

SUPERSCOPE_node *currentScope;

int main(void) {
    yyparse();
    return 0;
}

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int resolveFunc(char *func) {
    char *funcs[] = {"neg", "abs", "exp", "sqrt", "add", "sub", "mult", "div", "remainder", "log", "pow", "max", "min",
                     "exp2", "cbrt", "hypot", "let", "print" , "equal" ,"smaller" ,"larger" , ""};

    int i = 0;
    while (funcs[i][0] != '\0') {
        if (!strcmp(funcs[i], func))
            return i;

        i++;
    }
    yyerror("invalid function"); // paranoic -- should never happen
    return -1;
}

int resolveType(char *type) {
    char *types[] = {"real", "integer"};

    int i = 0;
    while (types[i][0] != '\0') {
        if (!strcmp(types[i], type))
            return i;
        i++;
    }
    yyerror("invalid function"); // paranoic -- should never happen
    return -1;
}


// create a node for a number
AST_NODE *number(double value) {
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(NUMBER_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = NUM_TYPE;
    p->data.number.type = REAL;
    p->data.number.value = value;
    //printf("number function, %lf\n",p->data.number.value);

    return p;
}

// create a node for a function
AST_NODE *function(char *funcName, AST_NODE *op1, AST_NODE *op2) {
    AST_NODE *p;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE) + sizeof(FUNCTION_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    p->type = FUNC_TYPE;
    p->data.function.name = funcName;
    p->data.function.op1 = op1;
    p->data.function.op2 = op2;

    return p;
}

AST_NODE *let(SYMBOL_AST_NODE *symbols, AST_NODE *s_expr) // Maybe
{
    AST_NODE *superFun;
    size_t nodeSize;

    SUPERSCOPE_node *localScope = malloc(sizeof(SUPERSCOPE_node));
    localScope->symbol = symbols;

    nodeSize = sizeof(AST_NODE) + sizeof(LET_AST_NODE);
    if ((superFun = malloc(nodeSize)) == NULL) {
        yyerror("out of memory");
    }
    superFun->type = LET_TYPE;

    superFun->data.let.scope = localScope;

    superFun->data.let.s_expr = s_expr;

    return superFun;
}

SYMBOL_AST_NODE *let_list(SYMBOL_AST_NODE *symbol, SYMBOL_AST_NODE *let_list) {
    SYMBOL_AST_NODE *symbolList = let_list;
    int found = 0;

    while (symbolList) {
        if (!strcmp(symbol->funcName, symbolList->funcName)) {
            yyerror("Redeclaring a variable!");
            symbolList->value = symbol->value;
            found = 1;
            symbol = let_list;
            break;
        }
        symbolList = symbolList->next;
    }
    if (!found) {
        symbol->next = let_list;
    }
    // returns linked list of symbols
    return symbol;
}


SYMBOL_AST_NODE *let_elem(char *type, char *symbol, AST_NODE *s_expr) // Looks good
{
    SYMBOL_AST_NODE *node;
    size_t nodeSize;
    nodeSize = sizeof(SYMBOL_AST_NODE);
    if ((node = malloc(nodeSize)) == NULL) {
        yyerror("out of memory");
    }
    if (type) {
        node->type = resolveType(type);
    } else {
        node->type = UNDECLARED;
    }

    node->funcName = symbol;
    node->value = s_expr;
    return node;
}

SYMBOL_AST_NODE *getThatSymbol(char *funcName) {
    SYMBOL_AST_NODE *symbol = NULL;
    int gotIt = 0;

    if (currentScope) {
        SUPERSCOPE_node *scope = currentScope;
        SYMBOL_AST_NODE *currentSymbol = scope->symbol;

        while (scope) {
            while (currentSymbol) {
                if (!strcmp(currentSymbol->funcName, funcName)) {
                    symbol = currentSymbol;
                    gotIt = 1;
                    break;
                }

                currentSymbol = currentSymbol->next;
                  }

            if (gotIt) {
                break;
            }
            scope = scope->parent;
            if (scope)
                currentSymbol = scope->symbol;
        }
    }
    if (!gotIt) {
        printf("Undeclared variable {%s}. ERROR\n", funcName);
        exit(2);
    }
    return symbol;
}

TYPE_SYM getThatType(char *funcName) {
    if (currentScope) {
        SUPERSCOPE_node *scope = currentScope;
        SYMBOL_AST_NODE *currentSymbol = scope->symbol;

        while (scope) {
            while (currentSymbol) {
                // Looking for the type
                if (!strcmp(currentSymbol->funcName, funcName) && currentSymbol->type != UNDECLARED) {
                  printf("TYPE %d",currentSymbol->type);
                    return currentSymbol->type;
                    
                }
                // Else keep on iterating through symbol
                currentSymbol = currentSymbol->next;
            }
            

            scope = scope->parent; // Go to the next scoope and keep on looking.
            if (scope) {
                currentSymbol = scope->symbol;
            }
        }


      }

        return UNDECLARED;
}

AST_NODE *symbol(char *funcName) {
    AST_NODE *p;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE) + sizeof(SYMBOL_AST_NODE);
    if ((p = malloc(nodeSize)) == NULL) {
        yyerror("out of memory");
    }

    p->type = SYM_TYPE;
    p->data.symbol.funcName = funcName;

    return p;
}

void enterScope(SUPERSCOPE_node *newScope) {
    newScope->parent = currentScope;
    currentScope = newScope;
}

void exitScope() {
    if (currentScope)
        currentScope = currentScope->parent;
}

// free a node
void freeNode(AST_NODE *p) {
    if (!p)
        return;

    if (p->type == FUNC_TYPE) {
        free(p->data.function.name);
        freeNode(p->data.function.op1);
        freeNode(p->data.function.op2);
    }
    free(p);
}
void print(AST_NODE *s_expr)
{
    NUMBER_AST_NODE *op1 = malloc(sizeof(NUMBER_AST_NODE));
    op1 = eval(s_expr->data.function.op1);
    if(op1->type == INTEGER)
    {
        printf("INTEGER \n %lf" , op1->value);
    }
    else if(op1->type == REAL)
    {
        printf("REAL \n  %.2lf" , op1->value);
    }

}
int condition(AST_NODE *s_expr , AST_NODE s_expr , AST_NODE s_expr)
{
    
}    

NUMBER_AST_NODE *eval(AST_NODE *p) {
    NUMBER_AST_NODE *result = malloc(sizeof(NUMBER_AST_NODE));
    NUMBER_AST_NODE *op1 = malloc(sizeof(NUMBER_AST_NODE));
    NUMBER_AST_NODE *op2 = malloc(sizeof(NUMBER_AST_NODE));


    if (!p)
        return result;

    else if (p->type == NUM_TYPE) {
        result = &p->data.number;
        if (result->type == INTEGER) {
          // printf("RESULT VAL BEFORE %lf \n",result->value);
            result->value = round(result->value);
             // printf("RESULT VAL AFTER %lf \n",result->value);
        }
        else
        {
          result->value = result->value;
        }
    } else if (p->type == FUNC_TYPE) {
        switch (resolveFunc(p->data.function.name)) {
            case 0: // NEG
                result = eval(p->data.function.op1);
                result->value = -(result->value);
                break;
            case 1: // ABS
                result = eval(p->data.function.op1);
                result->value = fabs(result->value);
                break;
            case 2: // EXP
                result = eval(p->data.function.op1);
                result->value = exp(result->value);
                break;
            case 3: // SQRT
                result = eval(p->data.function.op1);
                result->value = sqrt(result->value);
                break;
            case 4: // ADD
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;
                } else {
                    result->type = REAL;
                }
                result->value = op1->value + op2->value;
                printf("op1 val %lf op2 val %lf\n", op1->value, op2->value);
                break;
            case 5: // SUB
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;
                    result->value = round(op1->value - op2->value);
                } else {
                    result->type = REAL;
                    result->value = op1->value - op2->value;
                }

                break;
            case 6: // MULT
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;
                } else {
                    result->type = REAL;
                }
                result->value = op1->value * op2->value;
                break;
            case 7: // DIV
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;
                    result->value = round(op1->value / op2->value);
                } else {
                    result->type = REAL;
                    result->value = op1->value / op2->value;
                }

            case 8: // remainder
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                result->value = remainder(op1->value, op2->value);

                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;

                } else {
                    result->type = REAL;
                }
                break;
            case 9: // LOG
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);

                if (op1->value == 2) {
                    result->value = log2(op2->value);
                } else if (op1->value == 10) {
                    result->value = log10(op2->value);
                }
                break;
            case 10: // POW
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);


                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;
                    result->value = round(pow(op1->value, op2->value));
                } else {
                    result->type = REAL;
                    result->value = pow(op1->value, op2->value);
                }
                break;
            case 11: // MAX
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                result->value = fmax(op1->value, op2->value);

                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;

                } else {
                    result->type = REAL;
                }
                break;
            case 12: // MIN
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                result->value = fmin(op1->value, op2->value);

                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;

                } else {
                    result->type = REAL;
                }
                break;
            case 13: // EXP2
                result = eval(p->data.function.op1);
                result->value = exp2(result->value);
                break;
            case 14: // CBRT
                result = eval(p->data.function.op1);
                result->value = cbrt(result->value);
                break;
            case 15: // HYPOT
                op1 = eval(p->data.function.op1);
                op2 = eval(p->data.function.op2);
                result->value = hypot(op1->value, op2->value);

                if (op1->type == INTEGER && op2->type == INTEGER) {
                    result->type = INTEGER;

                } else {
                    result->type = REAL;
                }
                break;
            case 16: 
                break;
            case 17://EMPTY 
                print(p);
                break;
            case 18:
                condition(s_expr , s_expr , s_expr);
                break; 
            case 19:
                condition(s_expr , s_expr , s_expr);        
                break;
            case 20:
                condition(s_expr , s_expr , s_expr);
                break;
            case 21:
                break;        
        }

    } else if (p->type == LET_TYPE) {
        //enter the new scope, evaluate, then leave
        enterScope(p->data.let.scope);
        result = eval(p->data.let.s_expr);
        exitScope();
    } else if (p->type == SYM_TYPE) {
        p->data.symbol = *getThatSymbol(p->data.symbol.funcName);
        
         if (p->data.symbol.type == UNDECLARED) {
            p->data.symbol.type = getThatType(p->data.symbol.funcName);
          }
            
          if (p->data.symbol.type == UNDECLARED) {
          printf("Undeclared variable {%s} used Error...\n", p->data.symbol.funcName);
          exit(2);
          }
          else
          {
        op1 = eval(p->data.symbol.value);
        printf("THE TYPE %d THE NAME %s\n",p->data.symbol.type,p->data.symbol.funcName);

        if (p->data.symbol.type == INTEGER) {
            result->type = INTEGER;
            if (fmod(op1->value, 1) != 0) {
                printf("Wrong type assigned for variable.{%s} Warning...\n", p->data.symbol.funcName);
            }
            result->value = round(op1->value);
        } else {
            result->type = REAL;
            result->value = op1->value;
        }
          }


    }
    return result;
}


