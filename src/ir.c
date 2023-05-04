/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#include <stdlib.h>

#include "ir.h"

Unit * program;

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
    this->code[this->num_code_entries].code_entry_type = OP;
    this->code[this->num_code_entries].entry.op.opcode = op.opcode;
    this->code[this->num_code_entries].entry.op.size = op.size;
    this->code[this->num_code_entries].entry.op.source = op.source;
    this->code[this->num_code_entries].entry.op.destination = op.destination;
    ++this->num_code_entries;
}

void unit_add_label(Unit * this, Label label) {
    unit_manage_capacity(this);
    this->code[this->num_code_entries].code_entry_type = LABEL;
    this->code[this->num_code_entries].entry.label = label;
}


