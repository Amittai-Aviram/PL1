#include "symbol_table.h"

#define MESSAGE_SIZE 256

typedef struct Info {
    char string[256];
    int type_id;
} Info;

typedef struct ParamTypeInfo {
    int num;
    int type_ids[MAX_NUM_PARAMS];
} ParamTypeInfo;

typedef struct LabelNo {
    int num;
    struct LabelNo * next;
} LabelNo;

// HELPERS
LabelNo * new_label_no(int num);
int push_label_no(int true);
int pop_label_no(int true);
void get_new_register(char * bufferi, int type_id);
void get_new_mem(char * buffer, int type_id);
void get_new_param(char * buffer, int type_id);
void reset_params();

// DECLARATION HANDLERS
void handle_unit_head(Info * id, ParamTypeInfo * params_list, int return_type_id);
void handle_unit_id(Info * lhs, Info * identifier);
void handle_first_param(ParamTypeInfo * lhs, Info * param);
void handle_next_param(ParamTypeInfo * lhs, Info * param);
void handle_param(Info * lhs, Info * var_decl);
void handle_block_start();
void handle_block_rest();
void handle_variable_declaration(Info * lhs, Info * identifier, int decl_type);

// STATEMENT HANDLERS
void handle_initialization(Info * var_decl, Info * value);
void handle_assignment(Info * lhs, Info * source, Info * destination);
void handle_condition(int true);
void handle_condition_head(Info * cond_expr);
void handle_else();
void handle_while();
void handle_while_loop();
void handle_while_head(Info * cond_expr);
void handle_return_statement(Info * expression);

// EXPRESSION HANDLERS
void handle_number(Info * lhs, Info * num);
void handle_identifier_lexeme(Info * val, char * text);
void handle_identifier(Info * lhs, Info * identifier);
void handle_arithmetic_expression(Info * lhs, int op, Info * a, Info * b);
void handle_unary_minus_expression(Info * lhs, Info * expr);
void handle_relational_expression(Info * lhs, int op, Info * a, Info * b);
void handle_logical_expression(Info * lhs, int op, Info * a, Info * b);
void handle_function_call_expression(Info * lhs, Info * identifier, ParamTypeInfo * arg_types);
void handle_first_arg(ParamTypeInfo * lhs, Info * arg);
void handle_next_arg(ParamTypeInfo * lhs, Info * arg);
void handle_arg(Info * arg, Info * expression);
void handle_dereference_expression(Info * lhs, Info * identifier);

