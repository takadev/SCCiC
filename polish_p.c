#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void polish(char *s);
int execute(void);
int getValue(int ch);
int order(int ch);
void push(int n);
int pop(void);

#define STACK_SIZE 20
int stack[STACK_SIZE+1];
int stackp;
char polishOut[80];

int main(void)
{
    char siki[80];
    int ans;

    printf("input : ");
    fgets(siki, 80, stdin);
    polish(siki);

    if (polishOut[0] == '\0')
        exit(1);
    ans = execute();
    printf("trasform : %s\n", polishOut);
    printf("ans : %d\n", ans);

    return 0;
}

void polish(char *s)
{
    int wk;
    char *out = polishOut;

    stackp = 0;
    while(1)
    {
        while(isspace(*s)) { ++s; }
        if (*s == '\0') {
            while(stackp > 0) {
                if ((*out++ = pop()) == '(') {
                    puts("'(' is illegal\n");
                    exit(1);
                }
            }
            break;
        }
        if (islower(*s) || isdigit(*s)) {
            *out++ = *s++;
            continue;
        }

        switch(*s) {
            case '(':
                push(*s);
                break;
            case ')':
                while((wk = pop()) != '(') {
                    *out++ = wk;
                    if (stackp == 0) {
                        puts("'(' is short\n");
                        exit(1);
                    }
                }
                break;
            default:
                if (order(*s) == -1) {
                    printf("invalid literal : %c\n", *s);
                    exit(1);
                }
                while(stackp > 0 && (order(stack[stackp]) >= order(*s))) {
                    *out++ = pop();
                }
                push(*s);
        }
        s++;
    }
    *out = '\0';
}

int execute(void)
{
    int d1, d2;
    char *s;

    stackp = 0;
    for (s = polishOut; *s; s++) {
        if (islower(*s)) {
            push(getValue(*s));
        } else if (isdigit(*s)) {
            push(*s - '0');
        } else {
            d2 = pop();
            d1 = pop();
            switch(*s) {
                case '+':
                    push(d1 + d2);
                    break;
                case '-':
                    push(d1 - d2);
                    break;
                case '*':
                    push(d1 * d2);
                    break;
                case '/':
                    if (d2 == 0) {
                        puts("zero divi");
                        exit(1);
                    }
                    push(d1 / d2);
                    break;
            }
        }
    }
    if (stackp != 1) {
        printf("Error\n");
        exit(1);
    }

    return pop();
}

int getValue(int ch)
{
    if (islower(ch)) return ch - 'a' + 1;
    return 0;
}

int order(int ch)
{
    switch(ch) {
        case '*':
        case '/':
            return 3;
        case '+':
        case '-':
            return 2;
        case '(':
            return 1;
    }
    return -1;
}

void push(int n)
{
    if (stackp + 1 > STACK_SIZE) {
        puts("stack overflow");
        exit(1);
    }
    stack[++stackp] = n;
}

int pop(void)
{
    if (stackp < 1) {
        puts("stack underflow");
        exit(1);
    }
    return stack[stackp--];
}


