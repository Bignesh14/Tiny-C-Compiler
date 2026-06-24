/* main.c — Day 1 stub: tests the lexer */
/*#include <stdio.h>
#include "tcc.h"
#include "lex.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tcc <source.tcl>\n");
        return 1;
    }

    Token tokens[MAX_TOKENS];
    int n = tokenise(argv[1], tokens, MAX_TOKENS);
    if (n < 0) return 1;

    print_tokens(tokens, n);
    printf("\nTotal: %d tokens\n", n);
    return 0;
}*/




/* main.c — Day 2: lexer + parser test */
/*#include <stdio.h>
#include "tcc.h"
#include "lex.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tcc <source.tcl>\n");
        return 1;
    }

    // Phase 1: Lex
    Token tokens[MAX_TOKENS];
    int n = tokenise(argv[1], tokens, MAX_TOKENS);
    if (n < 0) return 1;

    // Phase 2: Parse into AST
    Node *root = parse(tokens, n);
    if (!root) {
        fprintf(stderr, "Parsing failed\n");
        return 1;
    }

    // Print both for now
    printf("=== TOKEN STREAM ===\n");
    print_tokens(tokens, n);

    printf("\n=== ABSTRACT SYNTAX TREE ===\n");
    print_ast(root, 0);

    return 0;
}*/





/* main.c — Day 3: lex + parse + semantic check */
/*#include <stdio.h>
#include "tcc.h"
#include "lex.h"
#include "parser.h"
#include "semcheck.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tcc <source.tcl>\n");
        return 1;
    }

    // Phase 1: Lex
    Token tokens[MAX_TOKENS];
    int n = tokenise(argv[1], tokens, MAX_TOKENS);
    if (n < 0) return 1;

    // Phase 2: Parse
    Node *root = parse(tokens, n);
    if (!root) return 1;

    // Phase 3: Semantic check
    sem_check(root);

    // Print AST after successful check
    printf("\n=== ABSTRACT SYNTAX TREE ===\n");
    print_ast(root, 0);

    return 0;
}*/







/* main.c — Day 4: full pipeline → generates C output */
#include <stdio.h>
#include "tcc.h"
#include "lex.h"
#include "parser.h"
#include "semcheck.h"
#include "codegen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: tcc <source.tcl>\n");
        return 1;
    }

    /* ── Phase 1: Lex ── */
    Token tokens[MAX_TOKENS];
    int n = tokenise(argv[1], tokens, MAX_TOKENS);
    if (n < 0) return 1;

    /* ── Phase 2: Parse ── */
    Node *root = parse(tokens, n);
    if (!root) return 1;

    /* ── Phase 3: Semantic check ── */
    sem_check(root);

    /* ── Phase 4: Code generation ── */
    /* Output filename is always "output.c" for now */
    FILE *out = fopen("output.c", "w");
    if (!out) {
        perror("fopen output.c");
        return 1;
    }

    codegen(root, out);
    fclose(out);

    printf("Success! Generated: output.c\n");
    printf("Now run:  gcc output.c -o myprog\n");
    printf("Then run: ./myprog\n");

    return 0;
}
