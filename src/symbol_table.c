/*
 * Author: Amittai Aviram - aviram@bc.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "symbol_table.h"

extern SymbolTable * symbol_table;

SymbolTable * new_symbol_table() {
    SymbolTable * new_table = malloc(sizeof(SymbolTable));
    new_table->hash_table = new_hash_table();
    new_table->reg_no = 0;
    new_table->mem_no = 0;
    new_table->param_no = 0;
    new_table->unit_started = 0;
    new_table->next = NULL;
    return new_table;
}

void delete_symbol_table(SymbolTable * this) {
    delete_hash_table(this->hash_table);
    if (this) {
        free(this);
    }
}

void push_symbol_table() {
    SymbolTable * current_table = symbol_table;
    SymbolTable * new_table = new_symbol_table();
    new_table->next = current_table;
    symbol_table = new_table;
}

void pop_symbol_table() {
    SymbolTable * current_table = symbol_table;
    if (current_table) {
        symbol_table = current_table->next;
        delete_symbol_table(current_table);
    }
}

SymbolTable * get_global_symbol_table() {
    SymbolTable * table = symbol_table;
    while (table->next) {
        table = table->next;
    }
    return table;
}

void * symbol_table_get(SymbolTable * this, const char * const key) {
    return hash_table_get(this->hash_table, key);
}

void * symbol_table_get_in_scope(SymbolTable * this, const char * const key) {
    SymbolTable * table = this;
    void * value = NULL;
    while (table->next) {
        value = hash_table_get(table->hash_table, key);
        if (value) {
            break;
        }
        table = table->next;
    }
    return value;
}

void * symbol_table_put(SymbolTable * this, const char * const key, void * value) {
    return hash_table_put(this->hash_table, key, value);
}

void * symbol_table_remove(SymbolTable * this, const char * const key) {
    return hash_table_remove(this->hash_table, key);
}

IdentifierEntry * new_identifier_entry(const char * const lexeme) {
    IdentifierEntry * entry = malloc(sizeof(IdentifierEntry));
    bzero(entry, sizeof(IdentifierEntry));
    strcpy(entry->lexeme, lexeme);
    entry->line_num = -1;
    return entry;
}

NumberEntry * new_number_entry(const char * const lexeme, const int type) {
    NumberEntry * entry = malloc(sizeof(NumberEntry));
    strcpy(entry->lexeme, lexeme);
    entry->type = type;
    entry->value = atol(lexeme);
    return entry;
}

