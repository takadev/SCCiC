#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    Print, Lparen, Rparen, Plus, Minus, Multi, Divi,
    Assign, VarName, IntNum, EofToken, Others
} Kind;

typedef struct {
    Kind kind;
    int val;
} Token;

void input(void);
void statement(void);
void expression(void);
void term(void);
void factor(void);
Token nextToken(void);
int nextChar(void);
void operate(Kind op);
void push(int n);
int pop(void);
void checkToken(Kind kd);

#define STACK_SIZE 20
int stack[STACK_SIZE + 1];
int stackp;
Token token;
char buf[80], *bufp;
int ch;
int var[26];
int errF;

int main(void)
{
    while(1) {
        input();
        token = nextToken();
        if (token.kind == EofToken) exit(1);
        statement();
        if (errF) puts("--ERROR--");
    }
    return 0;
}

void input(void)
{
    errF = 0;
    stackp = 0;
    buf[0] = '\0';
    fgets(buf, 80, stdin);
    bufp = buf;
    ch = nextChar();
}

void statement(void)
{
    int vNbr;

    switch(token.kind) {
        case VarName:
            vNbr = token.val;
            token = nextToken();
            checkToken(Assign);
            if (errF) break;
            token = nextToken();
            expression();
            var[vNbr] = pop();
            break;
        case Print:
            token = nextToken();
            expression();
            checkToken(EofToken);
            if (errF) break;
            printf("%d\n", pop());
            return;
        default:
            errF = 1;
    }
    checkToken(EofToken);
}

void expression(void)
{
    Kind op;

    term();
    while(token.kind == Plus || token.kind == Minus) {
        op = token.kind;
        token = nextToken();
        term();
        operate(op);
    }
}

void term(void)
{
    Kind op;

    factor();
    while(token.kind == Multi || token.kind == Divi) {
        op = token.kind;
        token = nextToken();
        factor();
        operate(op);
    }
}


void factor(void)
{
    switch (token.kind) {
        case VarName:
            push(var[token.val]);
            break;
        case IntNum:
            push(token.val);
            break;
        case Lparen:
            token = nextToken();
            expression();
            checkToken(Rparen);
            break;
        default:
            errF = 1;
    }
    token = nextToken();
}

Token nextToken(void)
{
    int num;
    Token tk = {Others, 0};

    while(isspace(ch)) {
        ch = nextChar();
    }

    if (isdigit(ch)) {
        for (num = 0; isdigit(ch); ch = nextChar()) {
            num = num * 10 + (ch - '0');
        }
        tk.val = num;
        tk.kind = IntNum;
    } else if (islower(ch)) {
        tk.val = ch - 'a';
        tk.kind = VarName;
        ch = nextChar();
    } else {
        switch(ch) {
            case '(': tk.kind = Lparen; break;
            case ')': tk.kind = Rparen; break;
            case '+': tk.kind = Plus; break;
            case '-': tk.kind = Minus; break;
            case '*': tk.kind = Multi; break;
            case '/': tk.kind = Divi; break;
            case '=': tk.kind = Assign; break;
            case '?': tk.kind = Print; break;
            case '\0': tk.kind = EofToken; break;
        }

        ch = nextChar();
    }
    return tk;
}

int nextChar(void)
{
    if (*bufp == '\0') return '\0';
    else return *bufp++;
}

void operate(Kind op)
{
    int d2 = pop();
    int d1 = pop();

    if (op == Divi && d2 == 0) {
        puts("division by 0");
        errF = 1;
    }

    if (errF) return;
    
    switch(op) {
        case Plus: push(d1 + d2); break;
        case Minus: push(d1 - d2); break;
        case Multi: push(d1 * d2); break;
        case Divi: push(d1 / d2); break;
        default: break;
    }
}

void push(int n)
{
    if (errF) return;
    if (stackp + 1 > STACK_SIZE) {
        puts("stack overflow");
        exit(1);
    }
    stack[++stackp] = n;
}

int pop(void)
{
    if (errF) return 1;
    if (stackp < 1) {
        puts("stack underflow");
        exit(1);
    }
    return stack[stackp--];
}

void checkToken(Kind kd)
{
    if (token.kind != kd) errF = 1;
}
