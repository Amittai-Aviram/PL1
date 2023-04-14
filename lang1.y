%{
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash_table.h"
#include "lang1_handlers.h"
#include "lang1.tab.h"
#include "symbol_table.h"

int yylex();
int yyerror(const char * msg);

extern FILE * yyin;
extern SymbolTable * symbol_table;
int label_no;
LabelNo * true_label_no_stack;
LabelNo * false_label_no_stack;

%}

%union {
    Info info;
    char string[256];
    int type_id;
}

%{
%}

%token <type_id> BOOLEAN_TYPE INT1_TYPE UINT1_TYPE INT2_TYPE UINT2_TYPE
%token <type_id> INT4_TYPE UINT4_TYPE INT8_TYPE UINT8_TYPE ADDRESS_TYPE
%token <info> IDENTIFIER NUMBER MINUS
%token PROCEDURE FUNCTION RUN IF ELSE WHILE FOR FROM TO BY RETURN
%token PERIOD COLON LPAREN RPAREN LBRACE RBRACE LARROW RARROW COMMA DEREFERENCE
%token PLUS TIMES DIV MOD EQ NE LT LE GE GT
%type <type_id> type_expression
%type <info> expression arithmetic_expression relational_expression logical_expression
%type <info> function_call_expression dereference_expression procedure_id function_id 
%type <info> variable_declaration param arg

%left OR
%left AND
%left EQ NE
%left LT LE GT GE
%left PLUS MINUS
%left TIMES DIV MOD
%right DEREFERENCE UMINUS
%%

program : unit_definitions { puts("Done."); }

unit_definitions : unit_definitions unit_definition
                 | unit_definition
                 ;

unit_definition : procedure_definition { printf("ret\n\n"); }
       | function_definition { putchar('\n'); }
       ;

procedure_definition : procedure_head block
                     ;

function_definition : function_head block
                    ;

procedure_head : procedure_id params_list
               ;

function_head : function_id params_list RARROW type_expression
              ;

procedure_id : PROCEDURE IDENTIFIER {
    push_symbol_table();
    symbol_table->unit = 1;
    label_no = 0;
    printf("_%s:\n", $2.string);
}
             ;

function_id : FUNCTION IDENTIFIER {
    push_symbol_table();
    symbol_table->unit = 1;
    label_no = 0;
    printf("_%s:\n", $2.string);
}
            ;

params_list : LPAREN params RPAREN
            ;

block : block_start block_rest
      ;

block_start : LBRACE {
    if (symbol_table->unit) {
        symbol_table->unit = 0;
    } else {
        push_symbol_table();
    }
}
            ;

block_rest : statements RBRACE { pop_symbol_table(); }
           ;



params : params COMMA param
       | param
       |
       ;

param : variable_declaration {
    char next_param[LEXEME_SIZE];
    get_new_param(next_param);
    handle_assignment($$.string, next_param, $1.string);
}
      ;


type_expression : INT1_TYPE { $$ = $1; }
                | UINT1_TYPE { $$ = $1; }
                | INT2_TYPE { $$ = $1; }
                | UINT2_TYPE { $$ = $1; }
                | INT4_TYPE { $$ = $1; }
                | UINT4_TYPE { $$ = $1; }
                | INT8_TYPE { $$ = $1; }
                | UINT8_TYPE { $$ = $1; }
                | ADDRESS_TYPE LPAREN type_expression RPAREN { $$ = $1; }
                ;

statements : statements statement
           | statement
           ;

statement : initialized_variable_declaration_statement
          | variable_declaration_statement
          | assignment_statement
          | procedure_call_statement
          | conditional_statement
          | iteration_statement
          | return_statement
          | block
          ;

initialized_variable_declaration_statement : variable_declaration LARROW expression PERIOD
                                               { printf("mov %s, %s\n", $3.string, $1.string); }
                                           ;

variable_declaration_statement : variable_declaration PERIOD
                     ;
                               ;
assignment_statement : IDENTIFIER LARROW expression PERIOD {
    printf("mov %s, ", $3.string);
    printf("%s\n", ((IdentifierEntry *)symbol_table_get(symbol_table, $1.string))->symbol);
}
                     | expression RARROW IDENTIFIER PERIOD {
    printf("mov %s, ", $1.string);
    printf("%s\n", ((IdentifierEntry *)symbol_table_get(symbol_table, $3.string))->symbol);
}
                     ;

procedure_call_statement : RUN IDENTIFIER LPAREN args RPAREN PERIOD
                         ;

conditional_statement : condition_head block ELSE block { printf("_BB_%d\n", pop_label_no(1)); }
                      | condition_head block { printf("_BB_%d\n", pop_label_no(0)); }
                      ;

condition_head : IF LPAREN expression RPAREN {
    printf("test %s, %s\n", $3.string, $3.string);
    printf("jz _BB_%d\n", push_label_no(0));
}
               ;

iteration_statement : WHILE LPAREN expression RPAREN LBRACE expression RBRACE
                    | FOR LPAREN variable_declaration FROM expression TO expression BY expression 
                       LBRACE statements RBRACE
                    ;

return_statement : RETURN expression PERIOD { printf("mov %s, $ret\n", $2.string); printf("ret\n"); }

variable_declaration : IDENTIFIER COLON type_expression {
    IdentifierEntry * id_entry = (IdentifierEntry *)symbol_table_get(symbol_table, $1.string);
    id_entry->type = $3;
    get_new_mem(id_entry->symbol);
    strcpy($$.string, id_entry->symbol);
}
                     ;

expression : arithmetic_expression
           | relational_expression
           | logical_expression
           | function_call_expression
           | dereference_expression
           | MINUS expression %prec UMINUS {handle_arithmetic_expression($$.string, MINUS, "0", $2.string); }
           | NUMBER {
    strcpy($$.string, $1.string);
    $$.type_id = INT4_TYPE;
}
           | IDENTIFIER {
    IdentifierEntry * entry = (IdentifierEntry *)symbol_table_get(symbol_table, $1.string);
    strcpy($$.string, entry->symbol);
    $$.type_id = entry->type;
}
           | LPAREN expression RPAREN { strcpy($$.string, $2.string); }
           ;

arithmetic_expression : expression PLUS expression { handle_arithmetic_expression($$.string, PLUS, $1.string, $3.string); }
        | expression MINUS expression { handle_arithmetic_expression($$.string, MINUS, $1.string, $3.string); }
        | expression TIMES expression { handle_arithmetic_expression($$.string, TIMES, $1.string, $3.string); }
        | expression DIV expression { handle_arithmetic_expression($$.string, DIV, $1.string, $3.string); }
        | expression MOD expression { handle_arithmetic_expression($$.string, MOD, $1.string, $3.string); }
        ;

relational_expression : expression EQ expression { handle_relational_expression(&$$, EQ, $1.string, $3.string); }
        | expression NE expression { handle_relational_expression(&$$, NE, $1.string, $3.string); }
        | expression LT expression { handle_relational_expression(&$$, LT, $1.string, $3.string); }
        | expression LE expression { handle_relational_expression(&$$, LE, $1.string, $3.string); }
        | expression GE expression { handle_relational_expression(&$$, GE, $1.string, $3.string); }
        | expression GT expression { handle_relational_expression(&$$, GT, $1.string, $3.string); }
        ;

logical_expression : expression AND expression { handle_logical_expression($$.string, AND, $1.string, $3.string); }
        | expression OR expression { handle_logical_expression($$.string, OR, $1.string, $3.string); }
        ;

function_call_expression : IDENTIFIER LPAREN args RPAREN {
    strcpy($$.string, "$ret");
    printf("call _%s\n", $1.string);
}
                         ;

dereference_expression : DEREFERENCE IDENTIFIER {
    get_new_register($$.string);
    printf("%s = address %s\n", $$.string, $2.string);
}
                       | DEREFERENCE LPAREN expression RPAREN
                           { get_new_register($$.string); printf("%s = address %s\n", $$.string, $3.string); }
                       ;


args : args COMMA expression
     | arg
     ;

arg : expression {
    reset_params();
    get_new_param($$.string);
    printf("mov %s, %s\n", $1.string, $$.string);
}
     ;

%%

int yyerror(const char * msg) {
    fprintf(stderr, "%s\n", msg);
    return EXIT_FAILURE;
}

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
    push_symbol_table();
    yyparse();
    if (argc > 1) {
        fclose(yyin);
    }
    pop_symbol_table();
    return EXIT_SUCCESS;
}

