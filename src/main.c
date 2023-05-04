/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#include <stdio.h>
#include <stdlib.h>

#include "symbol_table.h"
#include "pl1_handlers.h"
#include "pl1.tab.h"

extern FILE * yyin;
extern FILE * yyout;

FILE * fopen_checked(const char * const file_name, const char * const mode) {
    FILE * fp = fopen(file_name, mode);
    if (!fp) {
        perror("Could not open file");
        exit(EXIT_FAILURE);
    }
    return fp;
}

int main(int argc, char ** argv) {
    if (argc > 1) {
        yyin = fopen_checked(argv[1], "r");
    }
    if (argc > 2) {
        yyout = fopen_checked(argv[1], "r");
    }
    push_symbol_table();
    yyparse();
    if (argc > 2) {
        fclose(yyout);
    }
    if (argc > 1) {
        fclose(yyin);
    }
    pop_symbol_table();
    return EXIT_SUCCESS;
}

