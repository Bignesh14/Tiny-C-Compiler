/* tcc.h — shared types for the TinyC-Lite compiler
 This file defines the Token Type enum and Token struct that the whole compiler shares.*/
#ifndef TCC_H                               //Include gaurd: prevents double-inclusion
#define TCC_H                               //Marks this header as included

#include <stdio.h>          //for FILE*, printf, fprintf
#include <stdlib.h>         //for malloc, free, exit
#include <string.h>         //for strcmp, strncpy, strlen
#include <ctype.h>          //for isalpha, isdigit, isspace

//  Token types
typedef enum                                                  //enum: named list of integer constants
{
    TOK_INT,        /* keyword: int   */                      //value 0 - the 'int' keyword
    TOK_IF,         /* keyword: if    */                      //value 1
    TOK_ELSE,       /* keyword: else  */                      //value 2
    TOK_PRINT,      /* keyword: print */                      //value 3
    TOK_IDENT,      /* identifier: x, a, myVar */             // Variable,i.e., any name but not a keyword
    TOK_NUM,        /* integer literal: 42  */                //Sequence of digit characters
    TOK_PLUS,       /* + */
    TOK_MINUS,      /* - */
    TOK_STAR,       /* * */
    TOK_SLASH,      /* / */
    TOK_EQ,         /* = assignment  */
    TOK_EQEQ,       /* == equality   */
    TOK_NEQ,        /* != not-equal  */
    TOK_GT,         /* >  */
    TOK_LT,         /* <  */
    TOK_GTE,        /* >= */
    TOK_LTE,        /* <= */
    TOK_SEMI,       /* ; */
    TOK_LPAREN,     /* ( */                                     //block parentheses
    TOK_RPAREN,     /* ) */                                     //block parentheses
    TOK_LBRACE,     /* { */                                    //block delimiters
    TOK_RBRACE,     /* } */                                    //block delimiters
    TOK_EOF         /* end of file */                          //Signals no more input
} TokenType;                                             //The typedef is TokenType

// Token struct    
#define MAX_TOKEN_LEN 64                                 //Maximum number of characters in identifier/number
#define MAX_TOKENS    2048                               //Maximum tokens in one file

typedef struct {                                                   // A strcut groups related fields
    TokenType type;                                                //Which kind of token this is
    char      value[MAX_TOKEN_LEN]; /* actual text */              // The actual text, e.g.: "x", "myVar", "50"
    int       line;                 /* source line number */       // For error message later
} Token;                                                           // The typedef name is Token

#endif /* TCC_H */                                             //Closes the include gaurd


/*Alternate: You could use a linked list of Token nodes instead of a fixed array.
MAX_TOKENS=2048 is simpler and more than enough for our small language*/
