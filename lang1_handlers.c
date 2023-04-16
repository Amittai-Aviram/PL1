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

int is_register(Info * expr) {
    return !strncmp(expr->string, "$r", 2);
}

void move_to_register(Info * expr) {
    char new_reg[8];
    get_new_register(new_reg);
    printf("mov %s, %s\n", expr->string, new_reg); 
    strcpy(expr->string, new_reg);
}

int promote_integer_type(int type_a, int type_b) {
    return (type_a >= type_b) ? type_a : type_b;
}

void convert_to_boolean(Info * expr) {
    if (!is_register(expr)) {
        move_to_register(expr);
    }
    printf("test %s, %s\n", expr->string, expr->string);
    printf("setnz %s\n", expr->string);
    expr->type_id = BOOLEAN_TYPE;
}

void handle_arithmetic_expression(Info * lhs, int op, Info * a, Info * b) {
    if (!is_register(b)) {
        char new_reg[8];
        get_new_register(new_reg);
        printf("mov %s, %s\n", b->string, new_reg);
        strcpy(b->string, new_reg);
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
    printf("%s, %s\n", a->string, b->string);
    strcpy(lhs->string, b->string);
    lhs->type_id = promote_integer_type(a->type_id, b->type_id);
}

void handle_relational_expression(Info * lhs, int op, Info * a, Info * b) {
    get_new_register(lhs->string);
    lhs->type_id = BOOLEAN_TYPE;
    printf("cmp %s, %s\n", b->string, a->string);
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

void handle_logical_expression(Info * lhs, int op, Info * a, Info * b) {
    if (!is_register(b)) {
        move_to_register(b);
    }
    if (a->type_id != BOOLEAN_TYPE) {
        convert_to_boolean(a);
    }
    if (b->type_id != BOOLEAN_TYPE) {
        convert_to_boolean(b);
    }
    switch (op) {
        case AND:
            printf("and %s, %s\n", a->string, b->string);
            break;
        case OR:
            printf("or %s, %s\n", a->string, b->string);
            break;
    }
}

void handle_assignment(Info * lhs, Info * source, Info * destination) {
    printf("mov %s, %s\n", source->string, destination->string);
    if (lhs) {
        strcpy(lhs->string, destination->string);
        lhs->type_id = source->type_id;
    }
}

void handle_arg(Info * arg, Info * expression) {
    reset_params();
    get_new_param(arg->string);
    printf("mov %s, %s\n", expression->string, arg->string);
}

void handle_unit_id(Info * identifier) {
    push_symbol_table();
    symbol_table->unit = 1;
    label_no = 0;
    printf("_%s:\n", identifier->string);
}

void handle_param(Info * lhs, Info * var_decl) {
    Info next_param;
    get_new_param(next_param.string);
    next_param.type_id = var_decl->type_id;
    handle_assignment(lhs, &next_param, var_decl);
}

void handle_variable_declaration(Info * lhs, Info * identifier, int decl_type) {
    IdentifierEntry * id_entry = (IdentifierEntry *)symbol_table_get(symbol_table, identifier->string);
    id_entry->type = decl_type;
    get_new_mem(id_entry->symbol);
    strcpy(lhs->string, id_entry->symbol);
    lhs->type_id = id_entry->type;
}

void handle_initialization(Info * var_decl, Info * value) {
    handle_assignment(NULL, value, var_decl);
}

void handle_condition(int true) {
    printf("_BB_%d\n", pop_label_no(true));
}

void handle_condition_head(Info * cond_expr) {
    printf("test %s, %s\n", cond_expr->string, cond_expr->string);
    printf("jz _BB_%d\n", push_label_no(0));
}

void handle_number(Info * lhs, Info * num) {
    strcpy(lhs->string, num->string);
    lhs->type_id = INT4_TYPE;
}

void handle_identifier(Info * lhs, Info * identifier) {
    IdentifierEntry * entry = (IdentifierEntry *)symbol_table_get(symbol_table, identifier->string);
    strcpy(lhs->string, entry->symbol);
    lhs->type_id = entry->type;
}
