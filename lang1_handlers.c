#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang1_handlers.h"
#include "lang1.tab.h"
#include "symbol_table.h"

extern int label_no;
extern LabelNo * true_label_no_stack;
extern LabelNo * false_label_no_stack;
extern SymbolTable * symbol_table;

LabelNo * new_label_no(int num) {
    LabelNo * new_no = malloc(sizeof(LabelNo));
    new_no->num = num;
    return new_no;
}

int push_label_no(int true) {
    LabelNo * new_no = new_label_no(++label_no);
    LabelNo * stack = true ? true_label_no_stack : false_label_no_stack;
    new_no->next = stack;
    if (true) {
        true_label_no_stack = new_no;
    } else {
        false_label_no_stack = new_no;
    }
    return new_no->num;
}

int pop_label_no(int true) {
    LabelNo * current_label_no = true ? true_label_no_stack  : false_label_no_stack;
    int answer = current_label_no->num;
    if (true) {
        true_label_no_stack = true_label_no_stack->next;
    } else {
        false_label_no_stack = false_label_no_stack->next;
    }
    current_label_no->next = NULL;
    free(current_label_no);
    return answer;
}

void get_new_register(char * buffer) {
    sprintf(buffer, "$r%d", symbol_table->reg_no++);
}

void get_new_mem(char * buffer) {
    sprintf(buffer, "$m%d", symbol_table->mem_no++);
}

void get_new_param(char * buffer) {
    sprintf(buffer, "$p%d", symbol_table->param_no++);
}

void reset_params() {
    symbol_table->param_no = 0;
}

void handle_arithmetic_expression(char * buffer, int op, char * a, char * b) {
    if (strncmp(b, "$r", 2)) {
        char new_reg[8];
        get_new_register(new_reg);
        printf("mov %s, %s\n", b, new_reg);
        strcpy(b, new_reg);
    }
    switch(op) {
        case PLUS:
            printf("add ");
            break;
        case MINUS:
            printf("sub ");
            break;
        case TIMES:
            printf("imul ");
            break;
        case DIV:
            printf("idiv ");
            break;
        case MOD:
            printf("mod ");
            break;
    }
    printf("%s, %s\n", a, b);
    strcpy(buffer, b);
}

void handle_relational_expression(Info * lhs, int op, char * a, char * b) {
    get_new_register(lhs->string);
    lhs->type_id = BOOLEAN_TYPE;
    printf("cmp %s, %s\n", b, a);
    printf("set");
    switch(op) {
        case EQ:
            printf("e");
            break;
        case NE:
            printf("ne");
            break;
        case LT:
            printf("l");
            break;
        case LE:
            printf("le");
            break;
        case GE:
            printf("ge");
            break;
        case GT:
            printf("g");
            break;
    }
    printf(" %s\n", lhs->string);
}

void handle_logical_expression(char * buffer, int op, char * a, char * b) {
}

void handle_assignment(char * buffer, char * source, char * destination) {
    printf("mov %s, %s\n", source, destination);
    strcpy(buffer, destination);
}

