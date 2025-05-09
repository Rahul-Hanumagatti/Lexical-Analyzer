#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

// Define a list of keywords in C language
static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

// Define a list of operators in C language
static const char *operators[] = { "+", "-", "*", "/", "%", "=", "==", "!=", "<", ">", "<=", ">=", "&&", "||"};

// Define a list of special characters used in C language
static const char specialCharacters[] = {';', ',', '{', '}', '(', ')', '[', ']', '\'', '"'};

// Global variables to keep track of unbalanced brackets
int paren_count = 0;  // Tracks '(' and ')'
int brace_count = 0;  // Tracks '{' and '}'

// Function to initialize lexer by checking if the file is a valid C source file
int initializeLexer(const char* filename)  
{
    char *dot =".c";  // Check if the file extension is ".c"
    if(strstr(filename, dot) != NULL)  
    {
        FILE *ptr = fopen(filename,"r");  // Try to open the file
        if(ptr == NULL)  
        {
            printf("File not available\n");
            return -1;  // Return error if file is not found
        }
        fclose(ptr);  // Close file if opened successfully
        return 1;  // Return success if file is valid
    }
}

// External pointer to the file being processed
extern FILE *fptr;

int num_operators = sizeof(operators) / sizeof(operators[0]);

int curly=0, sqb=0, sqq=0, dqq=0, sq=0;
Token getNextToken() {
    Token token;
    int index = 0;
    char ch;

    token.lexeme[0] = '\0';  // Initialize lexeme as empty string
    token.type = UNKNOWN;     // Set token type as unknown initially

    // Skip over whitespace and comments
    while (1) {
        ch = fgetc(fptr);  // Read the next character from the file

        if (ch == EOF) {  // End of file reached
            token.type = UNKNOWN;
            return token;  // Return end-of-file token
        }

        if (isspace(ch))  // Skip over whitespace characters
            continue;

        // Skip over single-line comments starting with "//"
        if (ch == '#')  // Preprocessor directives
        {
            while (ch != '\n' && ch != EOF)  // Skip until end of line
                ch = fgetc(fptr);
            continue;  // Go to next character
        }

        // Skip over multi-line comments starting with "/*"
        if (ch == '/') {
            char next = fgetc(fptr);
            if (next == '/')  // Single-line comment
            {
                while (ch != '\n' && ch != EOF)  // Skip to end of line
                    ch = fgetc(fptr);
                continue;
            } else if (next == '*')  // Multi-line comment
            {
                char prev = 0;
                while ((ch = fgetc(fptr)) != EOF) {
                    if (prev == '*' && ch == '/')  // End of comment
                        break;
                    prev = ch;  // Track previous character
                }
                continue;
            } else  // Not a comment, put the character back
            {
                ungetc(next, fptr);
            }
        }

        break;  // Exit the loop once valid character is found
    }

    index = 0;  // Reset index for lexeme array
    token.lexeme[index++] = ch;  // Add first character to lexeme

    // Handle string literal (anything inside double quotes)
    if (ch == '"') {
        int valid = 0;
        while ((ch = fgetc(fptr)) != EOF) {
            token.lexeme[index++] = ch;
            if (ch == '"') {  // Check for closing double quote
                valid = 1;
                break;  // End of string literal
            }
        }
        token.lexeme[index] = '\0';  // Null-terminate the string
        token.type = valid ? CONSTANT : ERROR;  // Mark as constant or error
        return token;  // Return token
    }


    // Handle character literal (anything inside single quotes)
    if (ch == '\'') {
        ch = fgetc(fptr);  // Read the next character
        token.lexeme[index++] = ch;
        ch = fgetc(fptr);  // Read the next character
        token.lexeme[index++] = ch;
        if (ch != '\'') {  // Check for closing single quote
            token.lexeme[index] = '\0';  // Null-terminate the string
            token.type = ERROR;  // Error if it's not closed
            return token;  // Return error token
        }
        token.lexeme[index] = '\0';  // Null-terminate the string
        token.type = CONSTANT;  // Mark as constant
        return token;  // Return token
    }


    // Handle multi-character operators (e.g., <=, ==)
    char next = fgetc(fptr);  // Read the next character
    if (isMultiCharOperator(token.lexeme, ch, next))  // Check if it's a multi-char operator
    {
        token.lexeme[index++] = next;
        token.lexeme[index] = '\0';  // Null-terminate the operator
        token.type = OPERATOR;  // Mark as operator
        return token;  // Return operator token
    } else {
        ungetc(next, fptr);  // Put the character back if it's not a multi-char operator
    }

    // Handle identifiers, constants, etc.
    if (isalpha(ch) || ch == '_' || isdigit(ch))  // Identifiers can start with letters or '_', constants can start with digits
    {
        while ((ch = fgetc(fptr)) != EOF && (isalnum(ch) || ch == '_' || ch == '.' || ch == 'x' || ch == 'X' || ch == 'b' || ch == 'B' || ch == 'e' || ch == 'E' || ch == '+' || ch == '-')) {
            token.lexeme[index++] = ch;  // Add character to lexeme
            if (index >= MAX_TOKEN_SIZE - 1)  // Prevent overflow
                break;
        }
        ungetc(ch, fptr);  // Put the character back
    }

    token.lexeme[index] = '\0';  // Null-terminate the lexeme

    // Classify the lexeme based on its type
    if (isKeyword(token.lexeme))
        token.type = KEYWORD;  // Mark as keyword
    else if (isIdentifier(token.lexeme))
        token.type = IDENTIFIER;  // Mark as identifier
    else if (isConstant(token.lexeme))
        token.type = CONSTANT;  // Mark as constant
    else if (isOperator(token.lexeme))
        token.type = OPERATOR;  // Mark as operator
    else if (isSpecialCharacter(token.lexeme[0]))
        token.type = SPECIAL_CHARACTER;  // Mark as special character
    else
        token.type = ERROR;  // Mark as error if no match

    return token;  // Return the token
}

// Function to check if the string is a C keyword
int isKeyword(const char* str) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0)  // Compare with each keyword
            return 1;  // Return true if it matches
    }
    return 0;  // Return false if no match
}

// Function to check if the string is a valid C identifier
int isIdentifier(const char* str) {
    if (!isalpha(str[0]) && str[0] != '_') return 0;  // Identifier must start with letter or '_'

    for (int i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_')  // Identifier must contain only alphanumeric or '_'
            return 0;
    }
    return 1;  // Return true if it's a valid identifier
}

//  Function to check if the string is a valid constant (integer, float, hex, binary)
int isConstant(const char* str) {
    
    int i = 0;
    int hasDigit = 0, hasDot = 0, hasExp = 0;
    int len = strlen(str);


    int floatSuffix = 0;
    if (len > 1 && (str[len - 1] == 'f' || str[len - 1] == 'F'))  // Check for float suffix
    {
        floatSuffix = 1;
        len--;  // Ignore the suffix for checking the number
    }
  

    // to handle increment of decrement operators
    if (isalpha(str[i]) || str[i] == '_') {
        int j = i;
    
        // Extract the identifier
        while (isalnum(str[j]) || str[j] == '_')
            j++;
    
        // Check for ++ or --
        if ((str[j] == '+' && str[j+1] == '+') ||
            (str[j] == '-' && str[j+1] == '-')) {
            return 1;
        }
    }


    // Handle invalid octal like 0008
    if (str[0] == '0' && str[1] == '0' && str[2] == '0' && isdigit(str[3]) && str[3] >= '8') {
        return 0;
    }

    if (str[0] == '+' || str[0] == '-')
        i++;  // Ignore optional sign

    if (str[i] == '0' && str[i + 1] == 'x')  // Check for hexadecimal
    {
        for (i = i + 2; i < len; i++) {
            if (!isxdigit(str[i])) return 0;  // Invalid hex digit
        }
        return 1;  // Valid hex
    }


    if ((str[i] == '0' && (str[i + 1] == 'x' || str[i + 1] == 'X'))) {
        i += 2; // Start after '0x' or '0X'
    
        while (str[i] != '\0') {
            if (!isxdigit(str[i]))  // not a valid hex digit
                return 0;
            i++;
        }
        return 1; // Valid hex constant
    }
    

    if (str[i] == '0' && (str[i + 1] == 'b' || str[i + 1] == 'B'))  // Check for binary
    {
        for (i = i + 2; i < len; i++) {
            if (str[i] != '0' && str[i] != '1') return 0;  // Invalid binary digit
        }
        return 1;  // Valid binary
    }

    for (i = 0; i < len; i++) {
        if (isdigit(str[i])) hasDigit = 1;
        else if (str[i] == '.') {
            if (hasDot || hasExp) return 0;
            hasDot = 1;
        }
        else if (str[i] == 'e' || str[i] == 'E') {
            if (hasExp || !hasDigit) return 0;
            hasExp = 1;
            hasDigit = 0;
            if (str[i + 1] == '+' || str[i + 1] == '-') i++;
            if (i + 1 >= len || !isdigit(str[i + 1])) return 0;
        }
        else {
            return 0;
        }
    }

    return hasDigit;
}

// Function to check if the string is a valid operator
int isOperator(const char* str) {
    int size = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < size; i++) {
        if (strcmp(str, operators[i]) == 0)
            return 1;
    }
    return 0;
}

// Function to check if the string is a multi-character operator
int isMultiCharOperator(const char* base, char first, char second) {
    char op[3] = {first, second, '\0'};
    return isOperator(op);
}

// Function to check if the character is a special character
int isSpecialCharacter(char ch) {
    
    int size = sizeof(specialCharacters) / sizeof(specialCharacters[0]);
    for (int i = 0; i < size; i++) {
        if (ch == specialCharacters[i]) {
            if (ch == '{') curly++;           // Opening curly brace
            else if (ch == '}') curly--;      // Closing curly brace
            else if (ch == '(') sqb++;        // Opening parenthesis
            else if (ch == ')') sqb--;        // Closing parenthesis
            else if (ch == '"') dqq = !dqq;  // Toggle double quote
            else if (ch == '\'') sqq = !sqq; // Toggle single quote
            else if (ch == '[') sq++;        // Opening [ parenthesis
            else if (ch == ']') sq--;        // Closing ] parenthesis

            // Print the current counter values for debugging
            //printf("char: %c, curly: %d, sqb: %d, dquote: %d, squote: %d\n", ch, curly, sqb, dqq, sqq);
            return 1; // Found a special character
        }
    }
    return 0; // Not a special character
}



