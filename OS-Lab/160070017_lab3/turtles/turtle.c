#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

void sighandler();

char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for(i =0; i < strlen(line); i++){

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0){
                tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0; 
            }
        } 
        else {
            token[tokenIndex++] = readChar;
        }
    }
 
    free(token);
    tokens[tokenNo] = NULL ;
    return tokens;
}

// This function is specifically designed to handle cd
void cd_handle(char **tokens){
    int count = 0;
    while(1){
        if (tokens[count]!= NULL) count = count + 1;
        else break;
    }
    if (count != 2){
        printf("Wrong number of arguments\n");
        return;
    }
    int t = chdir(tokens[1]);
    if (t == -1) printf("Some error\n");
}

// This is used to handle all the other simple commands
void handle_other(char **tokens){
    int temp;
    temp = fork();
    if (temp == 0){
        int t = execvp(tokens[0], tokens);
        if (t == -1) printf("command not found\n");
        exit(0);
    }
    else{
        wait(NULL);
    }
}

//This is used to handle redirecting output to file.
void redirect_out(char **tokens, int pos){
    int temp;
    temp = fork();
    if (temp == 0){
        char *tokens_new[pos+1];
        for(int i=0;i<pos;i++){
            tokens_new[i] = tokens[i];
        }
        tokens_new[pos] = NULL;
        fclose(stdout);
        FILE* fp = fopen(tokens[pos + 1], "w");
        execvp(tokens_new[0],tokens_new);
        fclose(fp);
        exit(0);
    }
    else{
        wait(NULL);
    }

}

//This is used to run commands sequentially in the foreground when they are separated by ;;
void sequential(char **tokens, int colon){
    //printf("Colon %i\n",colon );
        
    char *tokens_new[colon+1];
    for (int i=0;i<colon;i++){
        tokens_new[i] = tokens[i];
    }
    tokens_new[colon] = NULL;    
    int temp = fork();
    if (temp == 0){
        int t = execvp(tokens_new[0], tokens_new);
        if (t == -1) printf("Some error\n");
        exit(0);
    }
    else{
        wait(NULL);
    }

}

// this parses the tokens
void parse(char **tokens){

    int red = 0, colon = 0;
    for(int i=0;tokens[i]!=NULL;i++){
            if (tokens[i][0] == '>') red = i;
            if (tokens[i][0] == ';' && tokens[i][1] == ';') colon = i;
    }

    if (colon > 0) {
        for (int i=0;tokens[i]!=NULL;i++){
            if (tokens[i][0] == ';' && tokens[i][1] == ';'){
                colon = i;
                break;
            }
        }
        sequential(tokens,colon);
        parse(tokens+colon + 1);
    }
    else if (tokens[0][0] == 'c' && tokens[0][1] == 'd') cd_handle(tokens);
    else if (strcmp(tokens[0],"exit") == 0 ) exit(0);
    else if (red > 0) redirect_out(tokens,red);
    else handle_other(tokens);

}

// This function continuosly reads from the user
void reading(){

    signal(SIGINT, sighandler);
    char  line[MAX_INPUT_SIZE];            
    char  **tokens;              
    int i;
    while (1) {           
       
        printf("Bazinga>");     
        bzero(line, MAX_INPUT_SIZE);
        gets(line);           
        line[strlen(line)] = '\n'; //terminate with new line
        tokens = tokenize(line);
   
        if (tokens[0] == NULL) continue;

        parse(tokens);

        // Freeing the allocated memory 
        for(i=0;tokens[i]!=NULL;i++){
            free(tokens[i]);
        }
        free(tokens);
    }
    return;
}

void sighandler(){
    printf("\n");
    return;
}


void main(void)
{
    signal(SIGINT, sighandler);
    reading();
    
}

                
