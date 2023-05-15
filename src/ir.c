/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#include <stdio.h>
#include <stdlib.h>

#include "ir.h"

Unit * program_start;
Unit * current_unit;
extern FILE * yyin;
extern FILE * yyout;

const char * printable_ops[] = {
    "mov",
    "movs",
    "movz",
    "test",
    "cmp",
    "sete",
    "setne",
    "setl",
    "setle",
    "setge",
    "setg",
    "setz",
    "setnz",
    "jmp",
    "je",
    "jne",
    "jl",
    "jle",
    "jge",
    "jg",
    "jnz",
    "jz",
    "add",
    "sub",
    "mul",
    "imul",
    "div",
    "idiv",
    "mod",
    "and",
    "or",
    "call",
    "addr",
    "ret"
};

const char printable_addr_types[] = {
    '\0',
    'p',
    'm',
    'r',
    '$'
};

Unit * new_unit(UnitType unit_type, IdentifierEntry * id) {
    Unit * this = (Unit *)malloc(sizeof(Unit));
    this->unit_type = unit_type;
    this->id = id;
    this->code = NULL;
    this->num_code_entries = 0;
    this->capacity = 0;
    this->next = NULL;
    return this;
}

void add_unit(UnitType unit_type, IdentifierEntry * id) {
    Unit * unit = new_unit(unit_type, id);
    if (!program_start) {
        current_unit = program_start = unit;
    } else {
        current_unit->next = unit;
        current_unit = unit;
    }
}

void unit_manage_capacity(Unit * this) {
    if (!this->capacity) {
        this->code = (CodeEntry *)malloc(CODE_ENTRY_CHUNK * sizeof(CodeEntry));
        this->capacity = CODE_ENTRY_CHUNK;
    } else if (this->num_code_entries == this->capacity) {
        int new_size = this->capacity + CODE_ENTRY_CHUNK;
        this->code = (CodeEntry *)realloc(this->code, new_size);
        this->capacity = new_size;
    }
}

void unit_add_op(Unit * this, Op op) {
    unit_manage_capacity(this);
    this->code[this->num_code_entries].code_entry_type = CODE_ENTRY_OP;
    this->code[this->num_code_entries].entry.op.opcode = op.opcode;
    this->code[this->num_code_entries].entry.op.size = op.size;
    this->code[this->num_code_entries].entry.op.source = op.source;
    this->code[this->num_code_entries].entry.op.destination = op.destination;
    ++this->num_code_entries;
}

void unit_add_label(Unit * this, Label label) {
    unit_manage_capacity(this);
    this->code[this->num_code_entries].code_entry_type = CODE_ENTRY_LABEL;
    this->code[this->num_code_entries].entry.label = label;
}

void generate_code() {
    Unit * unit = program_start;
    fprintf(yyout, "_%s:\n", unit->id->lexeme);
    while (unit) {
        for (int i = 0; i < unit->num_code_entries; ++i) {
            if (unit->code[i].code_entry_type == CODE_ENTRY_OP) {
                Op op = unit->code[i].entry.op;
                fprintf(yyout, "%s", printable_ops[op.opcode]);
                if (op.size > 0) {
                    fprintf(yyout, "%d", op.size);
                }
                if (op.source.addr_type) {
                    fprintf(yyout, " %c%d", printable_addr_types[op.source.addr_type],
                           op.source.size); 
                }
                if (op.destination.addr_type) {
                    fprintf(yyout, ", %c%d", printable_addr_types[op.destination.addr_type],
                           op.destination.size); 
                }
                fputc('\n', yyout);
            } else {
                Label label = unit->code[i].entry.label;
                fprintf(yyout, "_LABEL_%d:\n", label.num);
            }
        }
        unit = unit->next;
    }
}
