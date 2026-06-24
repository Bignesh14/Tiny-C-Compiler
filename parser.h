/* parser.h — parser interface */
#ifndef PARSER_H
#define PARSER_H

#include "tcc.h"
#include "ast.h"

/*
 * parse(tokens, n)
 * Takes the token array from tokenise().
 * Returns the root Node* of the full AST.
 * Returns NULL on parse error.
 */
Node *parse(Token tokens[], int n);

#endif /* PARSER_H */