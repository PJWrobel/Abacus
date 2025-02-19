#use std.pl as std


fn main(argc: int, argv: Vec<String>) -> Err {
    a := 3
    s1: String = "Hello"
    s2 := "World!" //&str
    s1.push(',')
      .push(' ')
      .append(s2)  //s1 = "Hello, World!"
    println(s1.split(' ')[0] + s2) // Hello, World!
}

//transpile:

#include<abautils.h>

int main(void) {
    i32 a = 3;
    struct pString s1 = pString_from_const_charptr("Hello");
    char* s2 = "World!";

    pString_push(&s1,s2)
}
