/* semcheck.h — semantic analysis interface */
#ifndef SEMCHECK_H
#define SEMCHECK_H

#include "ast.h"

/*
 * sem_check(root)
 * Walks the full AST.
 * Checks: all variables declared before use,
 *         no variable declared twice.
 * Prints a clear error and exits if anything is wrong.
 */
void sem_check(Node *root);

#endif /* SEMCHECK_H */