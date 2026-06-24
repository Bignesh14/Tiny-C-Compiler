//The header declares the functions defined in lex.c
//Any file that wants to call the lexer just includes this.
/* lex.h — lexer interface */
#ifndef LEX_H                                                   //Include Gaurd
#define LEX_H

#include "tcc.h"                                                 //Pull in Token, TokenType definations

/*
 * tokenise(filename, tokens, max_tokens)                       //Main lexer function - call this once
 * Reads the .tcl source file, fills tokens[].
 * Returns number of tokens found, or -1 on error.              //Return value = How many tokens were made
 */
int tokenise(const char *filename,                               //filename:path to source .tcl file
             Token tokens[],                                     //tokens[]: Output array we pass in
             int   max_tokens);                                  //max_tokens: size of that array(safety limit

/* Print token stream — for debugging */
void print_tokens(Token tokens[], int n);                        //n is the number of tokens returned by tokenise()

#endif /* LEX_H */
/*Why a separate .h file? So that later, parser.c can call tokenise() by just writing #include
 "lex.h" — it doesn't need to know how it works internally*/
