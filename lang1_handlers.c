#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lang1_handlers.h"
#include "symbol_table.h"
#include "lang1.tab.h"

extern int line_no;
extern int label_no;
extern LabelNo * true_label_no_stack;
extern LabelNo * false_label_no_stack;
extern SymbolTable * symbol_table;

// HELPERS

void report_error(const char * format, ...) {
    char msg[MESSAGE_SIZE];
    sprintf(msg, "Error (line %d): ", line_no - 1);
    va_list args;
    va_start(args, format);
    vsprintf(msg + strlen(msg), format, args);
    va_end(args);
    yyerror(msg);
}

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

int type_id_to_size(int type_id) {
    switch (type_id) {
        case INT1_TYPE:
        case UINT1_TYPE:
            return 1;
        case INT2_TYPE:
        case UINT2_TYPE:
            return 2;
        case INT4_TYPE:
        case UINT4_TYPE:
            return 4;
        case INT8_TYPE:
        case UINT8_TYPE:
            return 8;
        default:
            report_error("Unexpected type id: %d.\n", type_id);
            return -1;
    }
}

void get_new_register(char * buffer, int type_id) {
    sprintf(buffer, "$r%d_%d", type_id_to_size(type_id), symbol_table->reg_no++);
}

void get_new_mem(char * buffer, int type_id) {
    sprintf(buffer, "$m%d_%d", type_id_to_size(type_id), symbol_table->mem_no++);
}

void get_new_param(char * buffer, int type_id) {
    sprintf(buffer, "$p%d_%d", type_id_to_size(type_id), symbol_table->param_no++);
}

void reset_params() {
    symbol_table->param_no = 0;
}

int is_register(Info * expr) {
    return !strncmp(expr->string, "$r", 2);
}

void move_to_register(Info * expr) {
    char new_reg[SYMBOL_SIZE];
    get_new_register(new_reg, expr->type_id);
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

// DECLARATION HANDLERS

void handle_unit_head(Info * id, ParamTypeInfo * params_list, int return_type_id) {
    IdentifierEntry * entry =
        (IdentifierEntry *)symbol_table_get(get_global_symbol_table(), id->string);
    if (!entry) {
        report_error("Unit identifier %s missing from global symbol table.\n", id->string);
    }
    entry->info.unit_info.num_params = params_list->num;
    for (int i = 0; i < params_list->num; ++i) {
        entry->info.unit_info.param_types[i] = params_list->type_ids[i];
    }
    entry->info.unit_info.return_type = return_type_id;
}

void handle_unit_id(Info * lhs, Info * identifier) {
    push_symbol_table();
    symbol_table->unit_started = 1;
    label_no = 0;
    printf("_%s:\n", identifier->string);
    strcpy(lhs->string, identifier->string);
    lhs->type_id = -1;
}

void handle_first_param(ParamTypeInfo * lhs, Info * param) {
    lhs->type_ids[0] = param->type_id;
    lhs->num = 1;
}

void handle_next_param(ParamTypeInfo * lhs, Info * param) {
    lhs->type_ids[lhs->num] = param->type_id;
    ++lhs->num; 
}

void handle_param(Info * lhs, Info * var_decl) {
    Info next_param;
    get_new_param(next_param.string, var_decl->type_id);
    next_param.type_id = var_decl->type_id;
    printf("mov %s, %s\n", next_param.string, var_decl->string);
    strcpy(lhs->string, var_decl->string);
    lhs->type_id = var_decl->type_id;
}

void handle_block_start() {
    if (symbol_table->unit_started) {
        symbol_table->unit_started = 0;
    } else {
        push_symbol_table();
    }
}

void handle_block_rest() {
    pop_symbol_table();
}

void handle_variable_declaration(Info * lhs, Info * identifier, int decl_type) {
    IdentifierEntry * id_entry = (IdentifierEntry *)symbol_table_get(symbol_table, identifier->string);
    if (id_entry->line_num >= 0) {
        report_error("Identifier %s has already been declared at line %d.\n",
                identifier->string, id_entry->line_num);
    }
    id_entry->id_type = VAR;
    id_entry->info.var_info.type_id = decl_type;
    id_entry->line_num = line_no;
    get_new_mem(id_entry->info.var_info.symbol, decl_type);
    strcpy(lhs->string, id_entry->info.var_info.symbol);
    lhs->type_id = id_entry->info.var_info.type_id;
}


// STATEMENT HANDLERS

void handle_initialization(Info * var_decl, Info * value) {
    handle_assignment(NULL, value, var_decl);
}

void handle_assignment(Info * lhs, Info * source, Info * destination) {
    IdentifierEntry * entry = (IdentifierEntry *)symbol_table_get(symbol_table, destination->string);
    if (entry) {
        if (entry->id_type != VAR) {
        }
        strcpy(destination->string, entry->info.var_info.symbol);
    }
    printf("mov %s, %s\n", source->string, destination->string);
    if (lhs) {
        strcpy(lhs->string, destination->string);
        lhs->type_id = source->type_id;
    }
}

void handle_condition(int true) {
    printf("_BB_%d\n", pop_label_no(true));
}

void handle_condition_head(Info * cond_expr) {
    printf("test %s, %s\n", cond_expr->string, cond_expr->string);
    printf("jz _BB_%d\n", push_label_no(0));
}

void handle_else() {
    printf("jmp _BB_%d\n", push_label_no(1));
    printf("_BB_%d\n", pop_label_no(0));
}

void handle_return_statement(Info * expression) {
    printf("mov %s, $ret\n", expression->string);
    printf("ret\n");
}

// EXPRESSION HANDLERS

void handle_number(Info * lhs, Info * num) {
    strcpy(lhs->string, num->string);
    lhs->type_id = INT4_TYPE;
}

void handle_identifier_lexeme(Info * val, char * text) {
    if (!symbol_table_get(symbol_table, text)) {
        symbol_table_put(symbol_table, text, new_identifier_entry(text));
    }
    strcpy(val->string, text);
}

void handle_identifier(Info * lhs, Info * identifier) {
    IdentifierEntry * entry = (IdentifierEntry *)symbol_table_get(symbol_table, identifier->string);
    if (entry->id_type == VAR) {
        strcpy(lhs->string, entry->info.var_info.symbol);
        lhs->type_id = entry->info.var_info.type_id;
    }
}

void handle_arithmetic_expression(Info * lhs, int op, Info * a, Info * b) {
    if (!is_register(b)) {
        char new_reg[SYMBOL_SIZE];
        get_new_register(new_reg, b->type_id);
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

void handle_unary_minus_expression(Info * lhs, Info * expr) {
    Info zero = { "0", expr->type_id };
    handle_arithmetic_expression(lhs, MINUS, &zero, expr);
}

void handle_relational_expression(Info * lhs, int op, Info * a, Info * b) {
    get_new_register(lhs->string, INT1_TYPE);
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

void check_parameter_types(char * identifier, ParamTypeInfo * arg_types) {
    IdentifierEntry * entry =
        (IdentifierEntry *)symbol_table_get(get_global_symbol_table(), identifier);
    if (!entry) {
        report_error("A function or procedure named %s has not been defined.\n", identifier);
    }
    if (entry->info.unit_info.num_params != arg_types->num) {
        report_error("Parameter-argument mistmatch: %s expects %d arguments, but %d were provided.\n",
                identifier, entry->info.unit_info.num_params, arg_types->num);
    }
    for (int i = 0; i < entry->info.unit_info.num_params; ++i) {
        if (entry->info.unit_info.param_types[i] != arg_types->type_ids[i]) {
            report_error("Type mismatch in call to %s, argument %d.\n", identifier, i + 1);
        }
    }
}

void handle_function_call_expression(Info * lhs, Info * identifier, ParamTypeInfo * arg_types) {
    check_parameter_types(identifier->string, arg_types);
    strcpy(lhs->string, "$ret");
    printf("call _%s\n", identifier->string);
}

void handle_first_arg(ParamTypeInfo * lhs, Info * arg) {
    lhs->type_ids[0] = arg->type_id;
    lhs->num = 1;
}

void handle_next_arg(ParamTypeInfo * lhs, Info * arg) {
    lhs->type_ids[lhs->num] = arg->type_id;
    ++lhs->num; 
}

void handle_arg(Info * arg, Info * expression) {
    reset_params();
    get_new_param(arg->string, expression->type_id);
    printf("mov %s, %s\n", expression->string, arg->string);
}

