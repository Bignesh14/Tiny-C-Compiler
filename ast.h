/* ast.h — Abstract Syntax Tree node definitions */
#ifndef AST_H
#define AST_H

#include "tcc.h"

/* All the node types we need */
typedef enum {
    NODE_PROGRAM,   /* root: holds all statements    */
    NODE_VARDECL,   /* int x = expr;                 */
    NODE_ASSIGN,    /* x = expr;                     */
    NODE_IF,        /* if/else statement             */
    NODE_PRINT,     /* print expr;                   */
    NODE_BINOP,     /* +  -  *  /                    */
    NODE_UNARY,     /* -expr  (unary minus)          */
    NODE_COMPARE,   /* >  <  ==  !=  >=  <=          */
    NODE_NUM,       /* integer literal: 42           */
    NODE_IDENT      /* variable name: x              */
} NodeType;

/* Most nodes need at most 3 children
   (if-node needs: condition, then-block, else-block) */
#define MAX_CHILDREN 4

/* One tree node */
typedef struct Node {
    NodeType      type;
    char          name[MAX_TOKEN_LEN]; /* variable name or operator */
    int           ival;                /* value for NUM nodes       */
    struct Node  *children[MAX_CHILDREN];
    int           nchildren;          /* how many children used    */
    int           line;               /* source line number        */
} Node;

/* Allocate a new zeroed node on the heap */
Node *new_node(NodeType type, int line);

/* Append a child to a parent node */
void node_add_child(Node *parent, Node *child);

/* Print the tree with indentation — for debugging */
void print_ast(Node *node, int depth);

#endif /* AST_H */