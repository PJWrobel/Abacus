/*
pstr.c 
custom static? C string library
fully implemented on stack memory, no malloc or free, program runs once. only for bootstrap build
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "abautils.h"

#define PSTR_MAX 1000
#define CSTR_MAX_LEN 1001

#define CSTR_ITER(S) ;*s;s++

typedef enum ascii_type { //value used for field mask compatibility. DO NOT TOUCH. powers of 2 are masks
    //----masks----
    PSTR_VALID =         1,          // ...................................... 0000.0001
    PSTR_ALPHA =         1 << 2,     // ...................................... 0000.0100
    PSTR_ALPHANUMERIC =  1 << 3,     // ...................................... 0000.1000
    PSTR_FORMAT =        1 << 4,     // ...................................... 0001.0000
    PSTR_WHITESPACE =    1 << 5,     // ...................................... 0010.0000
    PSTR_SPECIAL =       1 << 6,     // ...................................... 0100.0000
    PSTR_NON_VALID =     1 << 7,     // ...................................... 1000.0000
    //----fields----
    pstr_null_char = 0,    // ................................................ 0000.0000
    pstr_digit = PSTR_ALPHANUMERIC | PSTR_VALID,    // ....................... 0000.1001
    pstr_lower = PSTR_ALPHANUMERIC | PSTR_ALPHA | PSTR_VALID,    // .......... 0000.1101
    pstr_upper = PSTR_ALPHANUMERIC | PSTR_ALPHA | PSTR_VALID | 2,    // ...... 0000.1111
    pstr_whitespace_formatter = PSTR_FORMAT | PSTR_WHITESPACE | PSTR_VALID, // 0011.0001
    pstr_symbol = PSTR_SPECIAL | PSTR_VALID,    // ........................... 0100.0001
    pstr_space = PSTR_SPECIAL | PSTR_WHITESPACE | PSTR_VALID     // .......... 0110.0001
} ascii_type;

const ascii_type char_type[128] = 
    { ['A'...'Z'] = pstr_upper,
      ['a'...'z'] = pstr_lower,
      ['0'...'9'] = pstr_digit,
      ['!'...'/'] = pstr_symbol,
      [':'...'@'] = pstr_symbol,
      ['['...'`'] = pstr_symbol,
      ['{'...'~'] = pstr_symbol,
      [' ']       = pstr_space,
      ['\t']      = pstr_whitespace_formatter,
      ['\n']      = pstr_whitespace_formatter,
      ['\0']      = pstr_null_char,
      [1 ... 8]   = PSTR_NON_VALID,
      [11 ... 31] = PSTR_NON_VALID
    };

typedef struct pstr {
    size_t n;
    char str[PSTR_MAX];
}pstr;

pstr pstr_new() {
    return (pstr){0,{0}};
}

void pstr_print(pstr s) {
    printf("%.*s\n", (int)s.n, s.str);
}

ascii_type cstr_type(char *s) { //return compund type?
    ascii_type compound_type = 0;
    for(;*s;s++) {
        if(char_type[*s] == PSTR_NON_VALID)
            return PSTR_NON_VALID;
        compound_type |= char_type[*s];
    }
    return compound_type;
}

_Bool valid_char(char c) {
    return char_type[c] != PSTR_NON_VALID;
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
    return cstr_type(s) != PSTR_NON_VALID;
}

pstr cstr_to_p(char* s) {
    if(!valid_cstr(s))
        return pstr_new();

    pstr new_pstr = pstr_new();
    new_pstr.n = strlen(s);
    memcpy(new_pstr.str, s, new_pstr.n);
    return new_pstr;
}

char* pstr_to_c(pstr s) {
    char *cstr = malloc(s.n + 1);
    memcpy(cstr, s.str, s.n);
    cstr[s.n] = '\0';
    return cstr;
}

pstr pstr_slice(pstr s, int head, int tail) {
    pstr new_pstr;
    new_pstr.n = tail-head +1;
    memcpy(new_pstr.str, s.str + head, new_pstr.n);
    return new_pstr;
}

pstr pstr_remove_whitespace(pstr s) {
    if((char_type[*s.str] & PSTR_WHITESPACE == 0) && (char_type[s.str[s.n-1]] & PSTR_WHITESPACE == 0))
        return s;
    int head;
    int tail;
    for(head = 0; char_type[s.str[head]] & PSTR_WHITESPACE; head++);
    for(tail = s.n-1; char_type[s.str[tail]] & PSTR_WHITESPACE; tail--);

    return pstr_slice(s, head, tail);
}

int pstr_cmp(pstr s1, pstr s2) { //CAUTION: same behavior as strcmp() 
                                     //HOWEVER pstr_upper < pstr_lower, ie. pstr_cmp("A","a") -> -1
    s1 = pstr_remove_whitespace(s1);
    s2 = pstr_remove_whitespace(s2);

    int i;

    for(i=0; s1.str[i] == s2.str[i]; i++) {
        if(s1.n <= i || s2.n <= i)
            if(s1.n == i && s2.n == i)
                return 0;

        if(char_type[s1.str[i]] & PSTR_ALPHANUMERIC == 0 &&
           char_type[s1.str[i]] & PSTR_SPECIAL == 0)
            return 0;
    }
    if(s1.n == i)
        return -1;
    if(s2.n == i)
        return 1;
    // is PSTR_VALID and not end of strings

    char sort_precedence(char c) { // {pstr_space,pstr_symbol,[2..11] = '0'..'9',A,a,B,b,C,c...}
        switch(char_type[c]) {
            case pstr_lower:
                return (c - 'a') * 2 + 13;
            case pstr_upper:
                return (c - 'A') * 2 + 12;
            case pstr_digit:
                return (c - '0') + 2;
            case pstr_symbol:
                return 1;
            case pstr_space:
                return 0;
            default:
                fprintf(stderr, "Error: sort_precedence(char) incompatable type:\n\"%c\":%d at %d in %s , %s  ", c, c, i, pstr_to_c(s1), pstr_to_c(s2));
                return 0;
        }
    }

    return sort_precedence(s1.str[i]) - sort_precedence(s2.str[i]);
}

pstr pstr_concat(pstr s1, pstr s2) { //TODO: TEST
    int len = s1.n + s2.n;
    memcpy(&s1.str[s1.n], s2.str, s2.n);
    return s1;
}

int main() { // testing
    char* mystr1 = "    Hello World!         ";
    char* mystr2 = "Hello World";
    char* mystr3 = "`1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,./~!@#$%^&*()_+QWERTYUIOP{}ASDFGHJKL:|ZXCVBNM<>?";
    pstr s1 = cstr_to_p(mystr1);
    pstr s2 = cstr_to_p(mystr2);
    pstr s3 = cstr_to_p(mystr3);
    printf("%d\n", pstr_cmp(s1,s2));
}
