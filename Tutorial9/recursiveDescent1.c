#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

typedef struct sym {
    char *varP;
    struct sym *next;
    char type; // ’i’: int, ’r’: double, ’n’: not assigned
    char init; // ’t’: initialized, ’f’: not initialized
    union {
    int integer;
    double real;
    } val ;
} *symP;


int token;
extern int yylex();
extern int line_no;

symP symTable = NULL;

symP symTable_head = NULL;
symP symTable_tail = NULL;

yylType yylval;

int getToken() {
    token = yylex();
    // printf("Token: %d(%c)\n", token, token);  // Debug
    return 0;
}

int match(int expected) {
    if (token == expected) {
        getToken();
        return 0;
    } else {
        printf("Syntax error: Expected %c(%d) but found %c(%d)\n", expected, expected, token, token);
        exit(1);
    }
}

symP findSymbol(char *varP){
    symP p = symTable;
    while (p != NULL) {
        if (strcmp(p->varP, varP) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

int addSymbol(char *varP, char type) {
    if (findSymbol(varP) != NULL) {
        printf("Redeclaration: %s : %d\n", varP, line_no);
        return 1;
    }

    symP newSym = (symP)malloc(sizeof(struct sym));
    newSym->varP = strdup(varP); // Allocate memory for variable name
    newSym->type = type;
    newSym->init = 'f'; // Uninitialized by default
    newSym->next = NULL;

    if (symTable == NULL) {
        // First symbol in the table
        symTable = newSym;
        symTable_head = newSym;
        symTable_tail = newSym;
    } else {
        // Append to the end
        symTable_tail->next = newSym;
        symTable_tail = newSym;
    }

    return 0;
}


int F();
int T();
int E();
int AC();
int RC();
int PC();
int C();
int CL();
int VL(char type);
int TY(char *type);
int D();
int DL();
int P();

int F() {
    if (token == IC || token == FC || token == ID) {
        if(token == ID){
            symP var = findSymbol(yylval.string);
            if (var == NULL) {
                printf("Variable %s not decl.: (%d)\n", yylval.string, line_no);
                return 1;
            }
            printf("Variable: %s\n", yylval.string);
            // exit(1);
        }
        getToken();
        return 0;
    } else if (token == '(') {
        getToken();
        if (E()) return 1;
        return match(')');
    } else {
        printf("F() error: Unexpected token %d\n", token);
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

// int AC() {
//     if (token != ID) {
//         printf("Expected an identifier at line %d\n", line_no);
//         return 1;
//     }

//     symP var = findSymbol(yylval.string);
//     // printf("Variable: %s\n", yylval.string);
//     // exit(0);
//     if (var == NULL) {
//         printf("Variable %s not decl.: (%d)\n", yylval.string, line_no);
//         return 1;
//     }
//     if (match(ID)) return 1;
//     if (token != '='){
//         printf("’=’ missing: (%d)\n", line_no);
//         return 1;
//     }
//     match('=');
//     return E();
// }
int AC() {
    if (token != ID) {
        printf("Expected an identifier at line %d\n", line_no);
        return 1;
    }

    symP var = findSymbol(yylval.string);
    if (var == NULL) {
        printf("Variable %s not decl.: (%d)\n", yylval.string, line_no);
        return 1;
    }
    match(ID);  // Move to the next token before checking '='

    if (token != '=') {
        printf("’=’ missing: (%d)\n", line_no);
        return 1;
    }
    match('=');

    return E();
}


int RC() {
    if (match(INPUT)) return 1;
    if(token == ID){
        symP var = findSymbol(yylval.string);
        if (var == NULL) {
            printf("Variable %s not decl.: (%d)\n", yylval.string, line_no);
            return 1;
        }
        // printf("Variable: %s\n", yylval.string);
        // exit(0);
    }
    return match(ID);
}

int PC() {
    if (match(PRINT)) return 1;
    return E();
}

int C() {
    if (token == PRINT) {
        return PC();
    } else if (token == INPUT) {
        return RC();
    } else if (token == ID) {
        return AC();
    } else {
        printf("C() error: Unexpected token %d\n", token);
        return 1;
    }
}

int CL() {
    while (token != '}' && token != 0) {
        if (C()) return 1;
    }
    return 0;
}

// int VL() {
//     if (match(ID)) return 1;
//     while (token == ID) {
//         if (match(ID)) return 1;
//     }
//     return 0;
// }

// int VL() {
//     char varName[100];
//     if (token != ID) {
//         printf("Variable expected but found %d\n", token);
//         return 1;
//     }

//     strcpy(varName, yylval.string);
//     getToken();

//     if (addSymbol(varName, 'n')) return 1; // Add variable to symbol table

//     while (token == ID) {
//         strcpy(varName, yylval.string);
//         getToken();
//         if (addSymbol(varName, 'n')) return 1;
//     }
//     return 0;
// }

// int VL() {
//     if (match(ID)) return 1;
//     while (token == ',') {
//         getToken();
//         if (match(ID)) return 1;
//     }
//     return 0;
// }

// int VL(char type) {

//     // Insert into symbol table
//     if (addSymbol(yylval.string, type)) {
//         return 1;  // Error: redeclaration
//     }

//     getToken();  // Move to next token

//     // Parse comma-separated variables
//     while (token == ',') {
//         getToken();  // Move past comma

//         // Insert next variable into symbol table
//         if (addSymbol(yylval.string, type)) {
//             return 1;  // Error: redeclaration
//         }
//         getToken();
//     }

//     return 0;
// }
int VL(char type) {
    if (token != ID) {
        printf("Expected variable name at line %d, found token: %d\n", line_no, token);
        return 1;
    }

    if (addSymbol(yylval.string, type)) {
        return 1;  // Error: Redeclaration
    }
    getToken();  // Move to next token

    // Parse comma-separated variables
    while (token == ',') {
        getToken();  // Move past comma
        if (token != ID) {
            printf("Expected variable name after ',' at line %d\n", line_no);
            return 1;
        }
        if (addSymbol(yylval.string, type)) {
            return 1;  // Error: Redeclaration
        }
        getToken();
    }

    return 0;
}



int TY(char *type) {
    if (token == ZAH) {
        *type = 'i';
        getToken();
        return 0;
    } else if (token == FLT) {
        getToken();
        *type = 'f';
        return 0;
    } else {
        printf("’flt’/’zah’ missing:%d\n", line_no);
        return 1;
    }
}

int D() {
    char type;
    if (TY(&type)) {
        printf("TY() error\n");
        return 1;
    }

    if (VL(type)) {
        printf("VL() error\n");
        return 1;
    }
    return 0;
}

int DL() {
    if (token != ZAH && token != FLT && token != ':' && token != '}') {
        if (D()) {
            printf("D() error\n");
            printf("DL() error\n");
            return 1;
        }
    }
    while (token == ZAH || token == FLT) {
        if (D()) {
            printf("D() error\n");
            printf("DL() error\n");
            return 1;
        }
    }
    return 0;
}

// int P() {
//     if (token != '{') {
//         printf("’{’ missing:%d\n", line_no);
//         return 1;
//     }
//     if (match('{')) return 1;
//     if (DL()) return 1;
//     if (token != ':') {
//         printf("current token: %d\n", token);
//         printf("’:’ missing:%d\n", line_no);
//         return 1;
//     }
//     if (match(':')) return 1;
//     if (CL()) return 1;
//     if (token != '}') {
//         printf("’}’ missing:%d\n", line_no);
//         return 1;
//     }
//     if (match('}')) return 1;
//     return 0;
// }

int P() {
    if (token != '{') {
        printf("’{’ missing:%d\n", line_no);
        return 1;
    }
    match('{');

    if (DL()) return 1;  // Parse declarations

    if (token != ':') {
        printf("current token: %d\n", token);
        printf("’:’ missing:%d\n", line_no);
        return 1;
    }
    match(':');

    if (CL()) return 1;  // Parse command list

    if (token != '}') {
        printf("’}’ missing:%d\n", line_no);
        return 1;
    }
    match('}');

    return 0;
}


int main() {
    getToken();
    P();
    if (token == 0) {
        printf("Accept\n");
    } else {
        printf("Reject\n");
    }
    return 0;
}

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "y.tab.h"

// typedef struct sym {
//     char *varP;
//     struct sym *next;
//     char type; // 'i': int, 'r': double, 'n': not assigned
//     char init; // 't': initialized, 'f': not initialized
//     union {
//         int integer;
//         double real;
//     } val;
// } *symP;

// int token;
// extern int yylex();
// extern int line_no;

// yylType yylval;
// symP symTable = NULL, symTable_head = NULL, symTable_tail = NULL;

// int getToken() {
//     token = yylex();
//     return 0;
// }

// int match(int expected) {
//     if (token == expected) {
//         getToken();
//         return 0;
//     } else {
//         printf("Syntax error: Expected %c(%d) but found %c(%d) at line %d\n", expected, expected, token, token, line_no);
//         exit(1);
//     }
// }

// symP findSymbol(char *varP) {
//     symP p = symTable;
//     while (p) {
//         if (strcmp(p->varP, varP) == 0) return p;
//         p = p->next;
//     }
//     return NULL;
// }

// int addSymbol(char *varP, char type) {
//     if (findSymbol(varP)) {
//         printf("Redeclaration: %s : %d\n", varP, line_no);
//         return 1;
//     }
//     symP newSym = (symP)malloc(sizeof(struct sym));
//     newSym->varP = strdup(varP);
//     newSym->type = type;
//     newSym->init = 'f';
//     newSym->next = NULL;
//     if (!symTable) {
//         symTable = symTable_head = symTable_tail = newSym;
//     } else {
//         symTable_tail->next = newSym;
//         symTable_tail = newSym;
//     }
//     return 0;
// }

// int checkVars(char *varP) {
//     symP sym = findSymbol(varP);
//     if (sym == NULL) {
//         printf("Variable %s not declared: (%d)\n", varP, line_no);
//         return 1;
//     }
//     if (sym->init == 'f') {
//         printf("Variable %s not init.: (%d)\n", varP, line_no);
//         return 1;
//     }
//     return 0;
// }


// int F(), T(), E(), AC(), RC(), PC(), C(), CL(), VL(char type), TY(char *type), D(), DL(), P();

// int F() {
//     if (token == IC || token == FC || token == ID) {
//         if (token == ID && !findSymbol(yylval.string)) {
//             printf("Variable %s not declared: (%d)\n", yylval.string, line_no);
//             return 1;
//         }
//         getToken();
//         return 0;
//     } else if (token == '(') {
//         getToken();
//         if (E()) return 1;
//         return match(')');
//     } else {
//         printf("F() error: Unexpected token %d at line %d\n", token, line_no);
//         return 1;
//     }
// }

// int T() {
//     if (F()) return 1;
//     while (token == BIN_OP) {
//         getToken();
//         if (F()) return 1;
//     }
//     return 0;
// }

// int E() {
//     if (T()) return 1;
//     while (token == BIN_OP) {
//         getToken();
//         if (T()) return 1;
//     }
//     return 0;
// }

// int AC() {
//     if (token != ID) {
//         printf("Expected an identifier at line %d\n", line_no);
//         return 1;
//     }
//     symP var = findSymbol(yylval.string);
//     if (!var) {
//         printf("Variable %s not declared: (%d)\n", yylval.string, line_no);
//         return 1;
//     }
//     match(ID);
//     if (token != '=') {
//         printf("’=’ missing: (%d)\n", line_no);
//         return 1;
//     }
//     match('=');
//     return E();
// }

// int RC() {
//     if (match(INPUT)) return 1;
//     if (token == ID && !findSymbol(yylval.string)) {
//         printf("Variable %s not declared: (%d)\n", yylval.string, line_no);
//         return 1;
//     }
//     return match(ID);
// }

// int PC() {
//     if (match(PRINT)) return 1;
//     return E();
// }

// int C() {
//     if (token == PRINT) return PC();
//     if (token == INPUT) return RC();
//     if (token == ID) return AC();
//     printf("C() error: Unexpected token %d at line %d\n", token, line_no);
//     return 1;
// }

// int CL() {
//     while (token != '}' && token != 0) {
//         if (C()) return 1;
//     }
//     return 0;
// }

// int VL(char type) {
//     if (token != ID) {
//         printf("Expected variable name at line %d, found token: %d\n", line_no, token);
//         return 1;
//     }
//     if (addSymbol(yylval.string, type)) return 1;
//     getToken();
//     while (token == ',') {
//         getToken();
//         if (token != ID) {
//             printf("Expected variable name after ',' at line %d\n", line_no);
//             return 1;
//         }
//         if (addSymbol(yylval.string, type)) return 1;
//         getToken();
//     }
//     return 0;
// }

// int TY(char *type) {
//     if (token == ZAH) {
//         *type = 'i';
//     } else if (token == FLT) {
//         *type = 'f';
//     } else {
//         printf("’flt’/’zah’ missing:%d\n", line_no);
//         return 1;
//     }
//     getToken();
//     return 0;
// }

// int D() {
//     char type;
//     if (TY(&type)) return 1;
//     return VL(type);
// }

// int DL() {
//     while (token == ZAH || token == FLT) {
//         if (D()) return 1;
//     }
//     return 0;
// }

// int P() {
//     if (match('{')) return 1;
//     if (DL()) return 1;
//     if (match(':')) return 1;
//     if (CL()) return 1;
//     return match('}');
// }

// int main() {
//     getToken();
//     if (P() == 0 && token == 0) {
//         printf("Accept\n");
//     } else {
//         printf("Reject\n");
//     }
//     return 0;
// }