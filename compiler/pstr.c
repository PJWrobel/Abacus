/*
pstr.c 
custom static C string library
relies on aba's allocator, program runs once. only for bootstrap build
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "abautils.h"

#define PSTR_MIN_SIZE 8 //some 2^x
#define CSTR_MAX_LEN 1000

typedef enum ascii_type { //value used for field mask compatibility. DO NOT TOUCH. powers of 2 are masks
    null_char = 0,              // 0000.0000
    digit = 9,                  // 0000.1001
    lower = 13,                 // 0000.1101
    upper = 15,                 // 0000.1111
    whitespace_formatter = 49,  // 0011.0001
    symbol = 65,                // 0100.0001
    space = 97,                 // 0110.0001
//----masks----
    VALID = 1,                  // 0000.0001
    ALPHA = 4,                  // 0000.0100
    ALPHANUMERIC = 8,           // 0000.1000
    FORMAT = 16,                // 0001.0000
    WHITESPACE = 32,            // 0010.0000
    SPECIAL = 64,               // 0100.0000
    NON_VALID = 128             // 1000.0000
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
    printf("%.*s\n", (int)s.n, s.str);
}

int pstr_msb(int n) { //most significant bit
    if(n <= 8)
        return 8;
    int msb = 0;
    for(;n;n >>= 1)
        msb++;
    return 1 << msb;
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
2
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
    
    size_t s_len = strlen(s);
    size_t s_max = pstr_msb(s_len);
    char *str;
    
    str = malloc(s_max);
    memcpy(str, s, s_len);
    return (pstr){s_len, s_max, str};
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
    int len = tail - head;
    int max = pstr_msb(len);
    char *new = malloc(max);
    memcpy(new, s.str + head, len);
    return (pstr){len, max, new};
}

pstr pstr_remove_whitespace(pstr s) {
    if((char_type[*s.str] & WHITESPACE == 0) && (char_type[s.str[s.n-1]] & WHITESPACE == 0))
        return s;
    int head;
    int tail;
    for(head = 0; char_type[s.str[head]] & WHITESPACE; head++);
    for(tail = s.n-1; char_type[s.str[tail]] & WHITESPACE; tail--)
    
    return pstr_slice(s, head, tail);
}

int pstr_cmp(pstr s1, pstr s2) { //CAUTION: same behavior as strcmp() 
                                     //HOWEVER upper < lower, ie. pstr_cmp("A","a") -> -1
    s1 = pstr_remove_whitespace(s1);
    s2 = pstr_remove_whitespace(s2);

    int i;

    for(i=0; s1.str[i] == s2.str[i]; i++) {
        if(s1.n <= i || s2.n <= i) {
            if(s1.n == i && s2.n == i)
                return 0;
            if(s1.n == i)
                return -1;
            if(s2.n == i)
                return 1;
        }
        if(char_type[s1.str[i]] & ALPHANUMERIC == 0 &&
           char_type[s1.str[i]] & SPECIAL == 0)
            return 0;
    }
    // is VALID and not end of strings

    char sort_precedence(char c) { // {space,symbol,[2..11] = '0'..'9',A,a,B,b,C,c...}
        switch(char_type[c]) {
            case lower:
                return (c - 'a') * 2 + 13;
            case upper:
                return (c - 'A') * 2 + 12;
            case digit:
                return (c - '0') + 2;
            case symbol:
                return 1;
            case space:
                return 0;
            default:
                fprintf(stderr, "Error: sort_precedence(char) incompatable type");
                return 0;
        }
    }
    return sort_precedence(s1.str[i]) - sort_precedence(s2.str[i]);
}

pstr pstr_concat(pstr s1, pstr s2) {
    int len = s1.n + s2.n;
    int max = pstr_msb(len);
    char *str = malloc(max);
    memcpy(str, s1.str, s1.n);
    memcpy(str + s1.n, s2.str, s2.n);
    return (pstr){len,max,str};
}

int main() { // testing
    char *mystr1 = "     Hello World! ";
    char *mystr2 = "Hello World";
    char *mystr3 = "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:|ZXCVBNM<>?";
    pstr s1 = cstr_to_p(mystr1);
    pstr s2 = cstr_to_p(mystr2);
    printf("%d\n", pstr_cmp(s1,s2));

}
