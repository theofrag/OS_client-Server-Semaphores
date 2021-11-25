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
    int i=0;  
    while(fgets(buffer,buffer_length,file)){
        if(i++==line_number){
            toReturn = malloc(sizeof(buffer));
            strcpy(toReturn,buffer);
            fclose(file);
            return toReturn;}
    
    }
    return 0;
    
}