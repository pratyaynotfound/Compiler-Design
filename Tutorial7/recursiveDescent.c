#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"

int token;
extern int yylex();
extern int line_no;

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

int F();
int T();
int E();
int AC();
int RC();
int PC();
int C();
int CL();
int VL();
int TY();
int D();
int DL();
int P();

int F() {
    if (token == IC || token == FC || token == ID) {
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

int AC() {
    if (match(ID)) return 1;
    if (match('=')) return 1;
    return E();
}

int RC() {
    if (match(INPUT)) return 1;
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

int VL() {
    if (match(ID)) return 1;
    while (token == ID) {
        if (match(ID)) return 1;
    }
    return 0;
}

int TY() {
    if (token == ZAH) {
        getToken();
        return 0;
    } else if (token == FLT) {
        getToken();
        return 0;
    } else {
        printf("’flt’/’zah’ missing:%d\n", line_no);
        return 1;
    }
}

int D() {
    if (TY()) {
        printf("TY() error\n");
        return 1;
    }
    if (VL()) {
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

int P() {
    if (token != '{') {
        printf("’{’ missing:%d\n", line_no);
        return 1;
    }
    if (match('{')) return 1;
    if (DL()) return 1;
    if (token != ':') {
        printf("’:’ missing:%d\n", line_no);
        return 1;
    }
    if (match(':')) return 1;
    if (CL()) return 1;
    if (token != '}') {
        printf("’}’ missing:%d\n", line_no);
        return 1;
    }
    if (match('}')) return 1;
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