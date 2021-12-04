#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "general.h"

// reads a specific line from a file
// line_number represents which line to read
// filename is the name of the file
char* readLineFromFile(int line_number, char* fileName){  
    char buffer[100];
    FILE* file;
    char* toReturn;
    int buffer_length=100;
    
    file=fopen(fileName,"r");
    if(file == NULL){
        perror("Error opening file");
        return NULL;
    }
    int i=0;  
    while(fgets(buffer,buffer_length,file)){
        if(i++==line_number){
            toReturn = malloc(sizeof(buffer));
            strcpy(toReturn,buffer);
            fclose(file);
            return toReturn;}
    
    }
    return NULL;
    
}

int countNUmberOfLines(char* fileName){
    char buffer[100];
    FILE* file;
    int buffer_length=100;
    file=fopen(fileName,"r");  
    if( file == NULL){
        perror("Error opening file");
        return -1;
    }
    int i=0;  
    while(fgets(buffer,buffer_length,file)){
        i++;    
    }
    fclose(file);
    return i;
}