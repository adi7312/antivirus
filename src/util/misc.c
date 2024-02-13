#include "misc.h"

char* get_substring(const char* string){
    char* substring = (char*)malloc(7); 
    strncpy(substring, string, 6);
    substring[6] = '\0';
    return substring;
}