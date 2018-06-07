#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    Lparen, Rparen, Plus, Minus, Multi, Divi, Equal, NotEqual,
    Less, LessEqual, Great, GreatEqual, SingleQ, DoubleQ, Assign, Semicolon,
    If, Else, Puts, Ident, IntNum, String, Letter, Digit, NullKind, EofToken, Others, END_list
} Kind;

#define ID_SIZE 31
#define TEXT_SIZE 100

typedef struct {
    Kind kind;
    char text[TEXT_SIZE + 1];
    int intVal;
} Token;

void initCharType(void);
Token nextToken(void);
int nextChar(void);
int isOpe2(int c1, int c2);
Token setKind(Token t);
void errExit(char *s);

Kind ctype[256];
Token token;
FILE *fp;

struct {
    char *kText;
    Kind kKind;
} KeyWordTable[] = {
    {"if", If},
    {"else", Else},
    {"puts", Puts},
    {"(", Lparen},
    {")", Rparen},
    {"+", Plus},
    {"-", Minus},
    {"*", Multi},
    {"/", Divi},
    {"==", Equal},
    {"!=", NotEqual},
    {"<", Less},
    {"<=", LessEqual},
    {">", Great},
    {">=", GreatEqual},
    {"=", Assign},
    {";", Semicolon},
    {"", END_list},
};


int main(int argc, char *argv[])
{
    if (argc == 1) exit(1);
    if ((fp = fopen(argv[1], "r")) == NULL) exit(1);

    printf("text kind intVal\n");
    initCharType();
    for (token = nextToken(); token.kind != EofToken; token = nextToken()) {
        printf("%-10s %3d %d\n", token.text, token.kind, token.intVal);
    }
    return 0;
}


void initCharType(void)
{
    int i;
    for (i = 0; i < 256; i++) { ctype[i] = Others; }
    for (i = '0'; i <= '9'; i++) { ctype[i] = Digit; }
    for (i = 'A'; i <= 'Z'; i++) { ctype[i] = Letter; }
    for (i = 'a'; i <= 'z'; i++) { ctype[i] = Letter; }
    ctype['_'] = Letter;
    ctype['='] = Assign;
    ctype['('] = Lparen;
    ctype[')'] = Rparen;
    ctype['<'] = Less;
    ctype['>'] = Great;
    ctype['+'] = Plus;
    ctype['-'] = Minus;
    ctype['*'] = Multi;
    ctype['/'] = Divi;
    ctype['\''] = SingleQ;
    ctype['"'] = DoubleQ;
    ctype[';'] = Semicolon;
}

Token nextToken(void)
{
    Token token = {NullKind, "", 0};
    int ct, num, errF = 0;
    char *p = token.text, *p_31 = p + ID_SIZE, *p_100 = p + TEXT_SIZE;
    static int ch = ' ';

    while (isspace(ch)) { ch = nextChar(); }

    if (ch == EOF) {
        token.kind = EofToken;
        return token;
    }

    switch(ctype[ch]) {
        case Letter:
            for (; ctype[ch] == Letter || ctype[ch] == Digit; ch = nextChar()) {
                if (p < p_31) {
                    *p++ = ch;
                }
            }
            *p = '\0';
            break;
        case Digit:
            for (num = 0; ctype[ch] == Digit; ch = nextChar()) {
                num = num * 10 + (ch - '0');
            }
            token.kind = IntNum;
            token.intVal = num;
            break;
        case SingleQ:
            ct = 0;
            for (ch = nextChar(); ch != EOF && ch != '\n' && ch != '\''; ch = nextChar()) {
                if (++ct == 1) {
                    *p++ = token.intVal = ch;
                } else {
                    errF = 1;
                }
            }
            *p = '\0';

            if (ch == '\'') {
                ch = nextChar();
            } else {
                errF = 1;
            }
            if (errF) errExit("invalid literal");
            token.kind = IntNum;
            break;
        case DoubleQ:
            for (ch = nextChar(); ch != EOF && ch != '\n' && ch != '"'; ch = nextChar()) {
                if (p >= p_100) {
                    errF = 1;
                } else {
                    *p++ = ch;
                }
            }
            *p = '\0';
            if (errF) errExit("Too long length of literal");
            if (ch != '"') errExit("Not close literal end Q");
            ch = nextChar();
            token.kind = String;
            break;
        default:
            *p++ = ch;
            ch = nextChar();
            if (isOpe2(*(p - 1), ch)) {
                *p++ = ch;
                ch = nextChar();
            }
            *p = '\0';
    }

    if (token.kind == NullKind) token = setKind(token);
    if (token.kind == Others) {
        printf("Invalid token : %s\n", token.text);
        exit(1);
    }

    return token;
}

int nextChar(void)
{
    static int c = 0;
    if (c == EOF) return c;
    if ((c = fgetc(fp)) == EOF) fclose(fp);
    return c;
}

int isOpe2(int c1, int c2)
{
    char s[] = "    ";
    s[1] = c1;
    s[2] = c2;
    return strstr(" <= >= == != ", s) != NULL;
}

Token setKind(Token t)
{
    int i;
    char *s = t.text;

    t.kind = Others;
    for (i = 0; KeyWordTable[i].kKind != END_list; i++) {
        if (strcmp(s, KeyWordTable[i].kText) == 0) {
            t.kind = KeyWordTable[i].kKind;
            return t;
        }
    }

    if (ctype[*s] == Letter) {
        t.kind = Ident;
    } else if (ctype[*s] == Digit) {
        t.kind = IntNum;
    }
    return t;
}

void errExit(char *s)
{
    puts(s);
    exit(1);
}


