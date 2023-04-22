/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#pragma once

#include "hash_table.h"

#define LEXEME_SIZE 256
#define SYMBOL_SIZE 8
#define MAX_NUM_PARAMS 16

typedef enum IdentifierType { NONE, VAR, FUNC, PROC } IdentifierType;

typedef struct SymbolTable {
    struct HashTable * hash_table;
    int reg_no;
    int mem_no;
    int param_no;
    int unit_started;
    struct SymbolTable * next;
} SymbolTable;

typedef struct IdentifierEntry {
    char lexeme[LEXEME_SIZE];
    IdentifierType id_type;
    int line_num;
    union {
        struct {
            int type_id;
            char symbol[SYMBOL_SIZE];
        } var_info;
        struct {
            int param_types[MAX_NUM_PARAMS];
            int ret_type;
        } unit_info;
    } info;
} IdentifierEntry;

typedef struct NumberEntry {
    char lexeme[LEXEME_SIZE];
    int type;
    long value;
} NumberEntry;

SymbolTable * new_symbol_table();
void delete_symbol_table(SymbolTable * this);
void push_symbol_table();
void pop_symbol_table();
SymbolTable * get_global_symbol_table();
void * symbol_table_get(SymbolTable * this, const char * const key);
void * symbol_table_put(SymbolTable * this, const char * const key, void * value);
void * symbol_table_remove(SymbolTable * this, const char * const key);
IdentifierEntry * new_identifier_entry(const char * const lexeme);
NumberEntry * new_number_entry(const char * const lexeme, const int type_id);

