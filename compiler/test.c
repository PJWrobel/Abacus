#include <stdio.h>


enum test1 {
    VALID = 1,
    MAYBE = 1 << 1,
    NOPE = 1 << 2,
    //--------------
    wart = VALID | MAYBE,
    tongue = MAYBE | NOPE
};

int main() {
    printf("%ld\n", tongue);
}
