#include <assert.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "symbol_table.h"

FILE * fopen_checked(const char * const file_name, const char * const mode);
int yyparse();

extern FILE * yyin;
extern FILE * yyout;

const char * in_file = "test_input.pl1";
const char * out_file = "test_output.pl1";

void stat_checked(const char * const file_name, struct stat * file_status) {
    int ret = stat(file_name, file_status);
    if (ret != 0) {
        perror("Could not obtain file information");
        exit(EXIT_FAILURE);
    }
}

int run_test(const char * const input, const char * const expected) {
    FILE * out = fopen_checked(in_file, "w");
    fprintf(out, "%s", input);
    fclose(out);
    yyin = fopen_checked(in_file, "r");
    yyout = fopen_checked(out_file, "w");
    push_symbol_table();
    int ret_val = yyparse();
    assert(!ret_val);
    pop_symbol_table();
    fclose(yyin);
    fclose(yyout);
    out = fopen_checked(out_file, "r");
    struct stat file_info;
    stat_checked(out_file, &file_info);
    char * buffer = malloc(file_info.st_size + 1);
    size_t length = fread(buffer, sizeof(char), file_info.st_size, out);
    fclose(out);
    buffer[length] = '\0';
    ret_val = !strcmp(buffer, expected);
    if (!ret_val) {
        puts("Input:");
        puts(input);
        puts("\nExpected:");
        puts(expected);
        puts("\nActual:");
        puts(buffer);
        puts("");
    }
    remove(in_file);
    remove(out_file);
    return ret_val;
}

void test_function_definition() {
    const char * input =
        "function func(x : int4) -> int4 {\n"
        "   return x.\n"
        "}\n";
    const char * expected =
        "_func:\n"
        "mov4 $p4_0, $m4_0\n"
        "mov4 $m4_0, $ret4\n"
        "ret8\n"
        "ret\n"
        "\n"
        "Done.\n";
    assert(run_test(input, expected));
}

void test_bad_function_definition() {
    const char * input =
        "function func(x : int4) {\n"
        "   return x.\n"
        "}\n";
    const char * expected =
        "_func:\n"
        "mov4 $p4_0, $m4_0\n"
        "mov4 $m4_0, $ret4\n"
        "ret8\n"
        "ret\n"
        "\n"
        "Done.\n";
    assert(run_test(input, expected));
}

int main() {
    test_function_definition();
    test_bad_function_definition();
    puts("All tests have completed successfully.");
    return EXIT_SUCCESS;
}

