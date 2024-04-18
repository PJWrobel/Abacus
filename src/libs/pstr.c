/*
pstr.c 
custom dynamic c string library
relies on no free of memory, program runs once. only for bootstrap build
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
    digit = 9,
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
      ['0'...'9'] = digit,
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
    size_t n;
    size_t max;
    char* str;
}pstr;

pstr pstr_new() {
    return (pstr){0, PSTR_MIN_SIZE, malloc(PSTR_MIN_SIZE)};
}

_Bool pstr_free(pstr* s) {
    if(s->str == NULL)
        return false;
    free(s->str);
    return true;
}

void pstr_print(pstr s) {
    printf("%.*s.\n", (int)s.n, s.str);
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

_Bool valid_char(char c) {
    return char_type[c] != NON_VALID;
}

_Bool valid_cstr(char *s) {
    if(s == NULL)
        return false;
    int i;
    char *head;
    for(i = 0, head = s; *head; i++, head++) { // check length
        if(i >= CSTR_MAX_LEN)
            return false;
    }
    return cstr_type(s) != NON_VALID;
}

pstr cstr_to_p(char* s) {
    if(!valid_cstr(s))
        return pstr_new();
    size_t slen = strlen(s);
    size_t s_max;
    char *str;
    if(slen < 8) {
        smax = 8;
    } else { //MSB
        int msb = 0;
        for(int temp = slen; temp; temp >>= 1)
            msb++;
        s_max = 1 << (msb + 1);
    }
    str = malloc(s_max);
    memcpy(str, s, slen);
    return (pstr){slen, s_max, str};
}

char* pstr_to_c(pstr s) {
    char *cstr = malloc(s.n + 1);
    memcpy(cstr, s.str, s.n);
    cstr[s.n] = '\0';
    return cstr;
}

pstr pstr_copy(pstr s) {
    while(s.n < s.max/2)
        s.max /= 2;
    char *new_str = malloc(s.max);
    memcpy(new_str, s.str, s.n);
    s.str = new_str;
    return s; //copy of all other fields
}

pstr pstr_slice(pstr s, int head, int tail) {
    int diff = tail - head;
    int msb = 0;
    for(int msb_mask = diff; msb_mask; msb_mask >>= 1)
        msb++;
    msb = 1 << (msb+1);
    int pstr_new = malloc(msb);
    memcpy()
    return (pstr){diff, };
}

pstr pstr_remove_whitespace(pstr s) {
    if(char_type[*s.str] & WHITESPACE == 0)
        if(char_type[s.str[s.n-1]] & WHITESPACE == 0)
            return s;
    int head;
    int tail;
    for(head = 0; char_type[s.str[head]] & WHITESPACE; head++);
    for(tail = s.n-1; char_type[s.str[tail]] & WHITESPACE; tail--)
    
    return pstr_slice(s, head, tail);
}

int pstr_cmp(pstr str1, pstr str2) { //CAUTION: same behavior as strcmp() 
                                     //HOWEVER upper < lower, ie. pstr_cmp("A","a") -> -1
    str1 = pstr_remove_whitespace(str1);
    str2 = pstr_remove_whitespace(str2);

    char *a = str1.str;
    char *b = str2.str;

    if(!a && !b)
        return 0;
    else if(!a)
        return -1;
    else if(!b)
        return 1;
    
    for(; *a == *b; a++, b++) {
        if(*a == '\0')
            return 0;
        //TODO: iterates over symbols
    }
    if(*a == '\0')
        return -1;
    if(*b == '\0')
        return 1;

    ascii_type atype = char_type[*a];
    ascii_type btype = char_type[*b];
    
    if(atype & ALPHANUMERIC == false && btype & ALPHANUMERIC == false)
        return 0;
    if(atype & ALPHANUMERIC == false)
        return -1;
    if(btype & ALPHANUMERIC == false)
        return 1;
    if(atype == btype)
        return *a - *b;
    if(atype == digit)
        return -1;
    if(btype == digit)
        return 1;
    //both alpha
    char a_c = *a;
    char b_c = *b;
    if(atype == upper){
        a_c = a_c - 'A' + 'a';
        if(a_c == b_c)
            return -1;
        return a_c - b_c;
    }    
    if(btype == upper){
        b_c = b_c - 'A' + 'a';
        if(a_c == b_c)
            return 1;
        return a_c - b_c;
    }
    return ~0; //ERROR
}


int main() { // testing
    char *mystr1 = "Hello World! ";
    char *mystr2 = "Hello World!";
    char *mystr3 = "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:|ZXCVBNM<>?";
    pstr s1 = cstr_to_p(mystr1);
    pstr s2 = cstr_to_p(mystr2);
    pstr_print(pstr_remove_whitespace(s1));
    pstr_print(s2);
    printf("%s\n", pstr_cmp(s1,s2) ? "false" : "true");
    return 0;
}
