#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

typedef struct sym {
    char *varP;
    struct sym *next;
    char type; // 'i': int, 'r': double
    char init; // 't': initialized, 'f': not initialized
} *symP;

int token;
extern int yylex();
extern int line_no;
yylType yylval;

symP symTable = NULL, symTable_head = NULL, symTable_tail = NULL;

int getToken() {
    token = yylex();
    return 0;
}

int match(int expected) {
    if (token == expected) {
        getToken();
        return 0;
    } else {
        printf("Syntax error: Expected %c(%d) but found %c(%d) at line %d\n", expected, expected, token, token, line_no);
        exit(1);
    }
}

symP findSymbol(char *varP) {
    symP p = symTable;
    while (p) {
        if (strcmp(p->varP, varP) == 0) return p;
        p = p->next;
    }
    return NULL;
}

int addSymbol(char *varP, char type) {
    if (findSymbol(varP)) {
        printf("Redeclaration: %s : %d\n", varP, line_no);
        return 1;
    }
    symP newSym = (symP)malloc(sizeof(struct sym));
    newSym->varP = strdup(varP);
    newSym->type = type;
    newSym->init = 'f';
    newSym->next = NULL;
    if (!symTable) {
        symTable = symTable_head = symTable_tail = newSym;
    } else {
        symTable_tail->next = newSym;
        symTable_tail = newSym;
    }
    return 0;
}

int checkVars(char *varP) {
    symP sym = findSymbol(varP);
    if (sym == NULL) {
        printf("Variable %s not declared: (%d)\n", varP, line_no);
        getToken();
        return 0;
    }
    if (sym->init == 'f') {
        printf("Variable %s not initialized: (%d)\n", varP, line_no);
        return 1;
    }
    return 0;
}

int F(), T(), E(), AC(), RC(), PC(), C(), CL(), VL(char type), TY(char *type), D(), DL(), P();

int F() {
    if (token == IC || token == FC) {
        getToken();
        return 0;
    } else if (token == ID) {
        if (checkVars(yylval.string)) return 1;
        getToken();
        return 0;
    } else if (token == '(') {
        getToken();
        if (E()) return 1;
        return match(')');
    } else {
        printf("F() error: Unexpected token %d at line %d\n", token, line_no);
        return 1;
    }
}

int T() {
    if (F()) return 1;
    while (token == BIN_OP) {
        getToken();
        if (F()) return 1;
    }
    return 0;
}

int E() {
    if (T()) return 1;
    while (token == BIN_OP) {
        getToken();
        if (T()) return 1;
    }
    return 0;
}

int AC() {
    if (token != ID) {
        return 1;
    }
    symP var = findSymbol(yylval.string);
    if (!var) {
        printf("Variable %s not declared: (%d)\n", yylval.string, line_no);
        getToken();
        return 0;
    }
    match(ID);
    if (token != '=') {
        printf("’=’ missing: (%d)\n", line_no);
        return 1;
    }
    match('=');
    if (E()) return 1;

    var->init = 't';
    return 0;
}

int RC() {
    if (match(INPUT)) return 1;
    if (token != ID) {
        return 1;
    }
    // printf("Variable: %s\n", yylval.string); //debug
    symP sym = findSymbol(yylval.string);
    if (!sym) {
        printf("Variable %s not declared: (%d)\n", yylval.string, line_no);
        getToken();
        return 0;
    }
    sym->init = 't';
    return match(ID);
}

int PC() {
    if (match(PRINT)) return 1;
    return E();
}

int C() {
    if (token == PRINT) return PC();
    if (token == INPUT) return RC();
    if (token == ID) return AC();
    printf("C() error: Unexpected token %d at line %d\n", token, line_no);
    return 1;
}

int CL() {
    while (token != '}' && token != 0) {
        if (C()) return 1;
    }
    return 0;
}

int VL(char type) {
    if (token != ID) {
        return 1;
    }
    if (addSymbol(yylval.string, type)) return 1;
    getToken();
    while (token == ',') {
        getToken();
        if (token != ID) {
            return 1;
        }
        if (addSymbol(yylval.string, type)) return 1;
        getToken();
    }
    return 0;
}

int TY(char *type) {
    if (token == ZAH) {
        *type = 'i';
    } else if (token == FLT) {
        *type = 'f';
    } else {
        printf("’flt’/’zah’ missing:%d\n", line_no);
        return 1;
    }
    getToken();
    return 0;
}

int D() {
    char type;
    if (TY(&type)) return 1;
    return VL(type);
}

int DL() {
    while (token == ZAH || token == FLT) {
        if (D()) return 1;
    }
    return 0;
}

int P() {
    if (match('{')) return 1;
    if (DL()) return 1;
    if (match(':')) return 1;
    if (CL()) return 1;
    return match('}');
}

int main() {
    getToken();
    if (P() == 0 && token == 0) {
        printf("Accept\n");
    } else {
        printf("Reject\n");
    }
    return 0;
}
