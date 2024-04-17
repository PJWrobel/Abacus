/*
pstr.c 
custom dynamic c string library
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#include "abautils.h"

#define PSTR_MIN_SIZE 8 //some 2^n
#define CSTR_MAX_LEN 1000

typedef enum ascii_type { //value used for field mask compatibility. DO NOT TOUCH. powers if 2 are masks
    null_char = 0,
    numeric = 9,
    lower = 13,
    upper = 15,
    whitespace_formatter = 49,
    symbol = 65,
    space = 97,
//----masks----
    VALID = 1,
    ALPHA = 4,
    ALPHANUMERIC = 8,
    FORMAT = 16,
    WHITESPACE = 32,
    SPECIAL = 64,
    NON_VALID = 128
} ascii_type;

const ascii_type char_type[128] = 
    { ['A'...'Z'] = upper,
      ['a'...'z'] = lower,
      ['0'...'9'] = numeric,
      ['!'...'/'] = symbol,
      [':'...'@'] = symbol,
      ['['...'`'] = symbol,
      ['{'...'~'] = symbol,
      [' ']       = space,
      ['\t']      = whitespace_formatter,
      ['\n']      = whitespace_formatter,
      ['\0']      = null_char,
      [1 ... 8]   = NON_VALID,
      [11 ... 31] = NON_VALID
    };

typedef struct pstr {
    int n;
    char* str;
}pstr;

pstr pstr_new() {
    return (pstr){0, malloc(PSTR_MIN_SIZE)};
}

_Bool pstr_free(pstr* s) {
    if(s->str == NULL)
        return false;
    free(s->str);
    return true;
}

ascii_type cstr_type(char *s) { //return compund type?
    ascii_type compound_type = 0;
    for(;*s;s++) {
        if(char_type[*s] == NON_VALID)
            return NON_VALID;
        compound_type |= char_type[*s];
    }
    return compound_type;
}

_Bool valid_cstr(char *s) {
    int i;
    char *head;
    for(i = 0, head = s; *head; i++, head++) { // check length
        if(i >= CSTR_MAX_LEN)
            return false;
    }
    return cstr_type(s) != NON_VALID;
}

_Bool valid_char(char c) {
    return char_type[c] != NON_VALID;
}

pstr cstr_to_p(char* s) {
    if(s == NULL)
        return pstr_new();
    int size = strlen(s);
}

//char* pstr_to_c() {}

/*int pstr_cmp(pstr str1, pstr str2) { //CAUTION: same behavior as strcmp() 
                                     //HOWEVER upper < lower, ie. pstr_cmp("A","a") -> -1
    char *a = str1.str;
    char *b = str2.str;

    if(!a && !b)
        return 0;
    else if(!a)
        return -1;
    else if(!b)
        return 1;
    }
    
    //map 
}*/

int main() { // testing
    char *mystr1 = "!@#$%^&*()";
    char *mystr2 = "Hello World!";
    char *mystr3 = "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:|ZXCVBNM<>?";
}
