/* codegen.h — code generator interface */
#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <stdio.h>

/*
 * codegen(root, out)
 * Walks the AST rooted at 'root'.
 * Writes a complete, valid C source file to FILE* out.
 * The output C file can be compiled directly with gcc.
 */
void codegen(Node *root, FILE *out);

#endif /* CODEGEN_H */