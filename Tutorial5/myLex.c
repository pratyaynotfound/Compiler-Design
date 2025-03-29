#include <stdio.h>
#include "y.tab.h"

extern int yylex();
extern int line_no;

yylType yylval;

int main() {
    int token;
    while ((token = yylex())) {
        switch (token) {
            case ID: printf("<ID, %s>\n", yylval.string); break;
            case IC: printf("<IC, %d>\n", yylval.integer); break;
            case FC: printf("<FC, %lf>\n", yylval.real); break;
            case ZAH: printf("<zah>\n"); break;
            case FLT: printf("<flt>\n"); break;
            case INPUT: printf("<input>\n"); break;
            case PRINT: printf("<print>\n"); break;
            case BIN_OP: printf("<%s>\n", yylval.string); break;
            case NEWLINE: printf("\n"); break;
            default: printf("<%c>\n", token); break;
        }
    }
    return 0;
}
