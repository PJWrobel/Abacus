#include<stdio.h>

#include "pstr.h"
#include "abatype.h"



i32 parse_num(pstr s, enum type) {
    i32 num = 0;
    pstr_lstrip(s);
    for(int i=0; i < s.n; i++) {
        if(char_type[s.str[i]] != digit) {
            
        }
    }
}
