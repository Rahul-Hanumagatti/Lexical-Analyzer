#include <stdio.h>
#include "lexer.h"

FILE *fptr;

extern int curly, sqb, sqq, dqq, sq;
int main(int argc, char *argv[]) 
{
    if(argc != 2)
    {
        printf("ERROR: Argument count should be exactly 2 : ./a.out filename.c\n");
        return -1;
    }
    if(initializeLexer(argv[1]) != 1)  // validating .c file or not
    {
        printf("Failed\n");
        return -1;
    } 
    printf("Success\n");
    printf("Parsing Started\n");
    
    fptr = fopen(argv[1],"r");  // opening the file in read mode
    fseek(fptr,0,SEEK_SET); // setting the file pointer to 1st position
    
    Token token;  // variable of struct Token
    // while ((token = getNextToken()).type != UNKNOWN) {  // checking the token type is not UNKNOWN , loop runs till the .type is UNKNOWN
    //     printf("Token: %s, Type: %d\n", token.lexeme, token.type);  // printing the token name and its type
    // }

    while (1)   // infinite loop till EOF
    {
        token = getNextToken(); // function call to get token

        if (feof(fptr))  // Check if we've actually reached the end of the file
            break;

            if (token.type != UNKNOWN)  // Only print valid tokens
            {
                printf("+------------------------------+\n");
                printf("| Token: %-22s |\n", token.lexeme);  // Left-align token name
                printf("| Type : ");
            
                if(token.type == KEYWORD)
                    printf("%-21s |\n", "KEYWORD");
                else if(token.type == OPERATOR)
                    printf("%-21s |\n", "OPERATOR");
                else if(token.type == SPECIAL_CHARACTER)
                    printf("%-21s |\n", "SPECIAL_CHARACTER");
                else if(token.type == CONSTANT)
                    printf("%-21s |\n", "CONSTANT");
                else if(token.type == IDENTIFIER)
                    printf("%-21s |\n", "IDENTIFIER");
                else if(token.type == ERROR)
                {
                    printf("%-21s |\n", "ERROR");
                    printf("| NOTE  : We can't declare     |\n");
                    printf("|         like this %s |\n", token.lexeme);
                }
                else if(token.type == UNKNOWN)
                    printf("%-21s |\n", "UNKNOWN");
            
                printf("+------------------------------+\n\n");
            }
            
            
    }

    if(curly != 0 || sqb != 0 ||  sq !=0 ||  sqq != 0 || dqq != 0)
        printf("This code has an error\n");
    // Print the current counter values for debugging
    printf("curly: %d, sqb: %d, dquote: %d, squote: %d, sq: %d\n",curly, sqb, dqq, sqq, sq);


    fclose(fptr);

    return 0;
}
