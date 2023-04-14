/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#pragma once

#include "hash_table.h"

#define LEXEME_SIZE 256

typedef struct SymbolTable {
    struct HashTable * hash_table;
    int reg_no;
    int mem_no;
    int param_no;
    int unit;
    struct SymbolTable * next;
} SymbolTable;

typedef struct IdentifierEntry {
    char lexeme[LEXEME_SIZE];
    char symbol[LEXEME_SIZE];
    int type;
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
void * symbol_table_get(SymbolTable * this, const char * const key);
void * symbol_table_put(SymbolTable * this, const char * const key, void * value);
IdentifierEntry * new_identifier_entry(const char * const lexeme, const int type_id);
NumberEntry * new_number_entry(const char * const lexeme, const int type_id);
void symbol_table_print(SymbolTable * this);
