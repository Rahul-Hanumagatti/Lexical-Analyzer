#ifndef LEXER_H
#define LEXER_H

#define MAX_KEYWORDS 20
#define MAX_TOKEN_SIZE 500

typedef enum {
    KEYWORD,   // 0
    OPERATOR,   //1
    SPECIAL_CHARACTER,  //2
    CONSTANT,  //3
    IDENTIFIER,   //4
    UNKNOWN,    //5
    ERROR     // 6
} TokenType; 

typedef struct {
    char lexeme[MAX_TOKEN_SIZE]; // string where we store the token
    TokenType type;  // type of token
} Token;

int initializeLexer(const char* filename);
Token getNextToken();
void categorizeToken(Token* token);
int isKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(char ch);
//int isSpecialCharacter(const char *str);
int isConstant(const char* str);
int isIdentifier(const char* str);

// Forms a 2-character string and checks if it's a valid multi-char operator (like <=, ==)
int isMultiCharOperator(const char* base, char first, char second) ;


#endif
