/* semcheck.c — semantic analyser for TinyC-Lite */
#include "semcheck.h"
#include <stdlib.h>
#include <string.h>

/* ══════════════════════════════════════════════
   Symbol table — private to this file
   ══════════════════════════════════════════════ */

/* One entry per declared variable */
typedef struct {
    char name[MAX_TOKEN_LEN];
    int  line;               /* line where it was declared */
} Symbol;

#define MAX_SYMBOLS 256

static Symbol symtab[MAX_SYMBOLS];   /* the table itself        */
static int    sym_count = 0;         /* how many entries used   */

/* Look up a name — returns index if found, -1 if not */
static int sym_find(const char *name) {
    for (int i = 0; i < sym_count; i++)
        if (strcmp(symtab[i].name, name) == 0)
            return i;
    return -1;
}

/* Add a new variable — error if it already exists */
static void sym_add(const char *name, int line) {
    if (sym_find(name) >= 0) {
        fprintf(stderr,
            "Semantic error line %d: "
            "variable '%s' already declared\n",
            line, name);
        exit(1);
    }
    if (sym_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Error: too many variables\n");
        exit(1);
    }
    strncpy(symtab[sym_count].name, name, MAX_TOKEN_LEN - 1);
    symtab[sym_count].name[MAX_TOKEN_LEN - 1] = '\0';
    symtab[sym_count].line = line;
    sym_count++;
}

/* Require that a variable exists — error if it doesn't */
static void sym_require(const char *name, int line) {
    if (sym_find(name) < 0) {
        fprintf(stderr,
            "Semantic error line %d: "
            "variable '%s' used but not declared\n",
            line, name);
        exit(1);
    }
}

/* ══════════════════════════════════════════════
   Recursive tree walker
   ══════════════════════════════════════════════ */

static void check_node(Node *n) {
    if (!n) return;

    switch (n->type) {

        case NODE_VARDECL:
            /* Check the right-hand expression FIRST,
               then declare the variable.
               This makes  int x = x + 1;  an error
               (x is not declared yet when RHS is checked) */
            check_node(n->children[0]);
            sym_add(n->name, n->line);
            break;

        case NODE_ASSIGN:
            /* Variable must already exist */
            sym_require(n->name, n->line);
            check_node(n->children[0]);
            break;

        case NODE_IDENT:
            /* Any plain variable reference must be declared */
            sym_require(n->name, n->line);
            break;

        case NODE_IF:
            check_node(n->children[0]);   /* condition  */
            check_node(n->children[1]);   /* then-block */
            if (n->nchildren > 2)
                check_node(n->children[2]); /* else-block */
            break;

        case NODE_PRINT:
            check_node(n->children[0]);
            break;

        default:
            /* NODE_BINOP, NODE_COMPARE, NODE_UNARY,
               NODE_NUM, NODE_PROGRAM — just recurse */
            for (int i = 0; i < n->nchildren; i++)
                check_node(n->children[i]);
            break;
    }
}

/* ══════════════════════════════════════════════
   Public entry point
   ══════════════════════════════════════════════ */

void sem_check(Node *root) {
    sym_count = 0;          /* reset table for each new file */
    check_node(root);
    printf("Semantic check passed: %d variable(s) declared.\n",
           sym_count);
}