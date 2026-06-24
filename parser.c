/* parser.c — TinyC-Lite recursive descent parser */
#include "parser.h"
#include <stdlib.h>
#include <string.h>

/* ══════════════════════════════════════════════
   PART 1 — Parser state + helper functions
   ══════════════════════════════════════════════ */

/* Module-level state shared by all parse functions */
static Token *g_tokens;   /* pointer to the token array     */
static int    g_count;    /* total number of tokens          */
static int    g_pos;      /* current read position (index)   */

/* peek: look at current token without consuming it */
static Token *peek(void) {
    if (g_pos >= g_count) return NULL;
    return &g_tokens[g_pos];
}

/* advance: consume current token and return it */
static Token *advance(void) {
    if (g_pos >= g_count) return NULL;
    return &g_tokens[g_pos++];
}

/* expect: consume a token of a specific type, or exit with error */
static Token *expect(TokenType t) {
    Token *tok = peek();
    if (!tok || tok->type != t) {
        fprintf(stderr,
            "Parse error at line %d: expected token %d but got %d\n",
            tok ? tok->line : -1,
            t,
            tok ? (int)tok->type : -1);
        exit(1);
    }
    return advance();
}

/* new_node: allocate a zeroed Node on the heap */
Node *new_node(NodeType type, int line) {
    Node *n = calloc(1, sizeof(Node));
    if (!n) {
        perror("calloc");
        exit(1);
    }
    n->type = type;
    n->line = line;
    return n;
}

/* node_add_child: append a child to a parent */
void node_add_child(Node *parent, Node *child) {
    if (parent->nchildren >= MAX_CHILDREN) {
        fprintf(stderr, "Error: too many children on node\n");
        exit(1);
    }
    parent->children[parent->nchildren++] = child;
}

/* print_ast: debug dump of the tree with indentation */
void print_ast(Node *node, int depth) {
    if (!node) return;

    /* print indent */
    for (int i = 0; i < depth * 2; i++) printf(" ");

    /* node type names matching the NodeType enum */
    const char *names[] = {
        "PROGRAM", "VARDECL", "ASSIGN", "IF", "PRINT",
        "BINOP", "UNARY", "COMPARE", "NUM", "IDENT"
    };

    printf("[%s]", names[node->type]);

    if (node->name[0])
        printf(" name=%s", node->name);

    if (node->type == NODE_NUM)
        printf(" val=%d", node->ival);

    printf(" (line %d)\n", node->line);

    /* recurse into children */
    for (int i = 0; i < node->nchildren; i++)
        print_ast(node->children[i], depth + 1);
}


/* ══════════════════════════════════════════════
   PART 2 — Expression parsers
   (factor → term → expr → cond)
   ══════════════════════════════════════════════ */

/* Forward declarations — functions call each other */
static Node *parse_expr(void);
static Node *parse_statement(void);

/* parse_factor: handles NUM, IDENT, (expr), unary minus
   This is the HIGHEST precedence level */
static Node *parse_factor(void) {
    Token *t = peek();
    if (!t) {
        fprintf(stderr, "Unexpected end of input\n");
        exit(1);
    }

    /* Integer literal: 42 */
    if (t->type == TOK_NUM) {
        advance();
        Node *n = new_node(NODE_NUM, t->line);
        n->ival = atoi(t->value);    /* "42" → 42 */
        return n;
    }

    /* Variable name: x */
    if (t->type == TOK_IDENT) {
        advance();
        Node *n = new_node(NODE_IDENT, t->line);
        strncpy(n->name, t->value, MAX_TOKEN_LEN - 1);
        return n;
    }

    /* Parenthesised expression: ( expr ) */
    if (t->type == TOK_LPAREN) {
        advance();                   /* consume '(' */
        Node *n = parse_expr();      /* parse inside */
        expect(TOK_RPAREN);          /* consume ')' */
        return n;
    }

    /* Unary minus: -expr */
    if (t->type == TOK_MINUS) {
        advance();
        Node *n = new_node(NODE_UNARY, t->line);
        n->name[0] = '-';
        n->name[1] = '\0';
        node_add_child(n, parse_factor());
        return n;
    }

    fprintf(stderr, "Parse error line %d: unexpected token '%s'\n",
            t->line, t->value);
    exit(1);
}

/* parse_term: handles * and /   (higher precedence than + -) */
static Node *parse_term(void) {
    Node *left = parse_factor();

    while (peek() &&
           (peek()->type == TOK_STAR || peek()->type == TOK_SLASH)) {

        Token *op    = advance();        /* consume * or / */
        Node  *right = parse_factor();

        Node *n = new_node(NODE_BINOP, op->line);
        n->name[0] = (op->type == TOK_STAR) ? '*' : '/';
        n->name[1] = '\0';

        node_add_child(n, left);
        node_add_child(n, right);
        left = n;                        /* chain: 2*3*4 works */
    }
    return left;
}

/* parse_expr: handles + and -   (lower precedence than * /) */
static Node *parse_expr(void) {
    Node *left = parse_term();

    while (peek() &&
           (peek()->type == TOK_PLUS || peek()->type == TOK_MINUS)) {

        Token *op    = advance();
        Node  *right = parse_term();

        Node *n = new_node(NODE_BINOP, op->line);
        n->name[0] = (op->type == TOK_PLUS) ? '+' : '-';
        n->name[1] = '\0';

        node_add_child(n, left);
        node_add_child(n, right);
        left = n;
    }
    return left;
}

/* parse_cond: handles comparison operators > < == != >= <=
   Used only inside if ( ... ) */
static Node *parse_cond(void) {
    Node  *left = parse_expr();
    Token *op   = peek();

    if (!op) return left;

    /* Check if next token is a comparison operator */
    if (op->type == TOK_EQEQ || op->type == TOK_NEQ  ||
        op->type == TOK_GT   || op->type == TOK_LT   ||
        op->type == TOK_GTE  || op->type == TOK_LTE) {

        advance();                         /* consume the operator */
        Node *right = parse_expr();

        Node *n = new_node(NODE_COMPARE, op->line);
        strncpy(n->name, op->value, MAX_TOKEN_LEN - 1);

        node_add_child(n, left);
        node_add_child(n, right);
        return n;
    }

    return left;   /* no comparison: just return the expression */
}


/* ══════════════════════════════════════════════
   PART 3 — Statement parsers
   ══════════════════════════════════════════════ */

/* parse_block: handles { statement* } */
static Node *parse_block(void) {
    expect(TOK_LBRACE);                          /* consume '{' */
    Node *block = new_node(NODE_PROGRAM, 0);     /* block container */

    while (peek() && peek()->type != TOK_RBRACE) {
        node_add_child(block, parse_statement());
    }

    expect(TOK_RBRACE);                          /* consume '}' */
    return block;
}

/* parse_statement: dispatches to the right parser
   based on the next token */
static Node *parse_statement(void) {
    Token *t = peek();
    if (!t || t->type == TOK_EOF) return NULL;

    /* ── int x = expr; ── */
    if (t->type == TOK_INT) {
        advance();                               /* consume 'int'  */
        Token *name = expect(TOK_IDENT);         /* must have name */
        expect(TOK_EQ);                          /* must have '='  */
        Node  *val  = parse_expr();              /* right-hand side */
        expect(TOK_SEMI);                        /* must have ';'  */

        Node *n = new_node(NODE_VARDECL, name->line);
        strncpy(n->name, name->value, MAX_TOKEN_LEN - 1);
        node_add_child(n, val);
        return n;
    }

    /* ── x = expr; ── (assignment) */
    if (t->type == TOK_IDENT) {
        Token *name = advance();                 /* consume name   */
        expect(TOK_EQ);
        Node  *val  = parse_expr();
        expect(TOK_SEMI);

        Node *n = new_node(NODE_ASSIGN, name->line);
        strncpy(n->name, name->value, MAX_TOKEN_LEN - 1);
        node_add_child(n, val);
        return n;
    }

    /* ── if (cond) { } else { } ── */
    if (t->type == TOK_IF) {
        advance();                               /* consume 'if'   */
        expect(TOK_LPAREN);                      /* consume '('    */
        Node *cond = parse_cond();               /* the condition  */
        expect(TOK_RPAREN);                      /* consume ')'    */
        Node *then = parse_block();              /* then-block     */

        Node *n = new_node(NODE_IF, t->line);
        node_add_child(n, cond);                 /* children[0]    */
        node_add_child(n, then);                 /* children[1]    */

        /* optional else */
        if (peek() && peek()->type == TOK_ELSE) {
            advance();                           /* consume 'else' */
            node_add_child(n, parse_block());    /* children[2]    */
        }
        return n;
    }

    /* ── print expr; ── */
    if (t->type == TOK_PRINT) {
        advance();                               /* consume 'print' */
        Node *val = parse_expr();
        expect(TOK_SEMI);

        Node *n = new_node(NODE_PRINT, t->line);
        node_add_child(n, val);
        return n;
    }

    fprintf(stderr,
        "Parse error line %d: unknown statement starting with '%s'\n",
        t->line, t->value);
    exit(1);
}


/* ══════════════════════════════════════════════
   PART 4 — Public entry point
   ══════════════════════════════════════════════ */

Node *parse(Token tokens[], int n) {
    g_tokens = tokens;      /* set module state */
    g_count  = n;
    g_pos    = 0;           /* start at first token */

    Node *root = new_node(NODE_PROGRAM, 0);

    while (peek() && peek()->type != TOK_EOF) {
        Node *stmt = parse_statement();
        if (stmt) node_add_child(root, stmt);
    }

    return root;
}