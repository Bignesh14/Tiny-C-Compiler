/*In lex.c we split it across two pages for readability. It defines the helpr functions and has the main scanning loop.*/
/* lex.c — TinyC-Lite lexer implementation */
#include "lex.h"

/* ── Helper: is this char still part of an identifier? ── */
static int is_ident_char(char c) {
    return isalpha(c) || isdigit(c) || c == '_';
}

/* ── Helper: map a word to a keyword token type ── */
static TokenType keyword_or_ident(const char *w) {
    if (strcmp(w, "int")   == 0) return TOK_INT;
    if (strcmp(w, "if")    == 0) return TOK_IF;
    if (strcmp(w, "else")  == 0) return TOK_ELSE;
    if (strcmp(w, "print") == 0) return TOK_PRINT;
    return TOK_IDENT;
}

/* ── Helper: add one completed token to the array ── */
static void add_token(Token arr[], int *count,
                      TokenType t, const char *val, int line) {
    if (*count >= MAX_TOKENS) {
        fprintf(stderr, "Error: too many tokens\n");
        exit(1);
    }
    arr[*count].type = t;
    strncpy(arr[*count].value, val, MAX_TOKEN_LEN - 1);
    arr[*count].value[MAX_TOKEN_LEN - 1] = '\0';
    arr[*count].line = line;
    (*count)++;
}


/*Why static? These helpers are internal details of lex.c. Marking them static prevents name
collisions with other files. Always make helpers static unless another file needs them*/



/* ── Main tokenise function ──  contains Setup + Identifiers + Numbers*/
int tokenise(const char *filename, Token tokens[], int max) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open '%s'\n", filename);
        return -1;
    }

    int count = 0;
    int line  = 1;
    int c;

    while ((c = fgetc(fp)) != EOF) {

        /* Track line numbers */
        if (c == '\n') { line++; continue; }

        /* Skip whitespace */
        if (isspace(c)) continue;

        /* Skip line comments (# to end of line) */
        if (c == '#') {
            while ((c = fgetc(fp)) != '\n' && c != EOF);
            line++;
            continue;
        }

        /* ── Identifiers and keywords ── */
        if (isalpha(c) || c == '_') {
            char word[MAX_TOKEN_LEN] = {0};
            int  wi = 0;
            word[wi++] = (char)c;

            int next;
            while ((next = fgetc(fp)) != EOF &&
                    is_ident_char((char)next)) {
                if (wi < MAX_TOKEN_LEN - 1)
                    word[wi++] = (char)next;
            }
            if (next != EOF) ungetc(next, fp);
            word[wi] = '\0';

            TokenType t = keyword_or_ident(word);
            add_token(tokens, &count, t, word, line);
            continue;
        }

        /* ── Integer literals ── */
        if (isdigit(c)) {
            char num[MAX_TOKEN_LEN] = {0};
            int  ni = 0;
            num[ni++] = (char)c;

            int d;
            while ((d = fgetc(fp)) != EOF && isdigit(d))
                num[ni++] = (char)d;
            if (d != EOF) ungetc(d, fp);
            num[ni] = '\0';

            add_token(tokens, &count, TOK_NUM, num, line);
            continue;
        }

        /* ── Operators and punctuation ── contains Operators, Switch and EOF*/
        switch (c) {
            case '+': add_token(tokens,&count,TOK_PLUS,  "+", line); break;
            case '-': add_token(tokens,&count,TOK_MINUS, "-", line); break;
            case '*': add_token(tokens,&count,TOK_STAR,  "*", line); break;
            case '/': add_token(tokens,&count,TOK_SLASH, "/", line); break;
            case ';': add_token(tokens,&count,TOK_SEMI,  ";", line); break;
            case '(': add_token(tokens,&count,TOK_LPAREN,"(", line); break;
            case ')': add_token(tokens,&count,TOK_RPAREN,")", line); break;
            case '{': add_token(tokens,&count,TOK_LBRACE,"{", line); break;
            case '}': add_token(tokens,&count,TOK_RBRACE,"}", line); break;

            case '=': {
                int nxt = fgetc(fp);
                if (nxt == '=')
                    add_token(tokens,&count,TOK_EQEQ,"==",line);
                else {
                    if (nxt != EOF) ungetc(nxt, fp);
                    add_token(tokens,&count,TOK_EQ,  "=", line);
                }
                break;
            }
            case '!': {
                int nxt = fgetc(fp);
                if (nxt == '=')
                    add_token(tokens,&count,TOK_NEQ,"!=",line);
                else
                    if (nxt != EOF) ungetc(nxt, fp);
                break;
            }
            case '>': {
                int nxt = fgetc(fp);
                if (nxt == '=')
                    add_token(tokens,&count,TOK_GTE,">=",line);
                else {
                    if (nxt != EOF) ungetc(nxt, fp);
                    add_token(tokens,&count,TOK_GT, ">", line);
                }
                break;
            }
            case '<': {
                int nxt = fgetc(fp);
                if (nxt == '=')
                    add_token(tokens,&count,TOK_LTE,"<=",line);
                else {
                    if (nxt != EOF) ungetc(nxt, fp);
                    add_token(tokens,&count,TOK_LT, "<", line);
                }
                break;
            }
            default:
                fprintf(stderr,
                    "Warning: unknown character '%c' at line %d\n",
                    c, line);
        }
    }

    /* Always end with EOF token */
    add_token(tokens, &count, TOK_EOF, "", line);
    fclose(fp);
    return count;
}

/* ── Debug: print all tokens ── */
/* print_tokens() + Fixed Identifier Loop*/
/*The scanning loopabove had one simplification. Here is the complete, correct identifier-reading loop and the print_tokens() debug function*/
void print_tokens(Token tokens[], int n) {
    const char *names[] = {
        "INT","IF","ELSE","PRINT",
        "IDENT","NUM",
        "PLUS","MINUS","STAR","SLASH",
        "EQ","EQEQ","NEQ",
        "GT","LT","GTE","LTE",
        "SEMI","LPAREN","RPAREN",
        "LBRACE","RBRACE","EOF"
    };
    for (int i = 0; i < n; i++) {
        printf("[%3d] %-8s  '%s'\n",
            tokens[i].line,
            names[tokens[i].type],
            tokens[i].value);
    }
}
/*Why int next = fgetc() instead of char? fgetc returns an int so it can return EOF (-1). If
 you store it in a char, EOF may be misread on some platforms. Always use int for fgetc
 return values — this is a classic C gotcha*/
