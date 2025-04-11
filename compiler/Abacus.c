#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "abautils.h"

#define ABA_C_COMPILER "gcc"

const char help[] = {
        "Abacus compiler {ABA_VERSION}\n"
        "Options:\n"
        "-help              : this page\n"
        "-L <LIB_NAME>      : Library compilation\n"
    };

int main(int argc, char *argv[]) {
    
    if ( argc == 2 && strcmp(argv[1], "-help") == 0 ) {
        printf("%s", help);
        exit(0);
    }

    int ret = 0;
    char c_cmp_instr[500];

    printf("End Abacus compilation. Executing %s\n", ABA_C_COMPILER);

    printf("%s\n", c_cmp_instr);

    int c_cmp_err = system( c_cmp_instr ); //execute c compilation

    if( c_cmp_err ) {
        printf("ABACUS: Fatal Error - C compilation failed. STOP.\n");
        exit(c_cmp_err);
    }
    

    return ret;
}
