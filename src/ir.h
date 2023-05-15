/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#pragma once

#include "symbol_table.h"

#define CODE_ENTRY_CHUNK 16

typedef enum Opcode {
    OP_MOV,
    OP_MOVS,
    OP_MOVZ,
    OP_TEST,
    OP_CMP,
    OP_SETE,
    OP_SETNE,
    OP_SETL,
    OP_SETLE,
    OP_SETGE,
    OP_SETG,
    OP_SETZ,
    OP_SETNZ,
    OP_JMP,
    OP_JE,
    OP_JNE,
    OP_JL,
    OP_JLE,
    OP_JGE,
    OP_JG,
    OP_JNZ,
    OP_JZ,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_IMUL,
    OP_DIV,
    OP_IDIV,
    OP_MOD,
    OP_AND,
    OP_OR,
    OP_CALL,
    OP_ADDR,
    OP_RET
} Opcode;

typedef enum CodeEntryType {
    CODE_ENTRY_OP,
    CODE_ENTRY_LABEL
} CodeEntryType;

typedef enum AddrType {
    ADDR_TYPE_NONE,
    ADDR_TYPE_PARAM,
    ADDR_TYPE_MEM,
    ADDR_TYPE_REG,
    ADDR_TYPE_IMM
} AddrType;

typedef struct Addr {
    AddrType addr_type;
    long num;
    int size;
} Addr;

typedef struct Op {
    Opcode opcode;
    int size;
    Addr source;
    Addr destination;
} Op;

typedef struct Label {
    int num;
} Label;

typedef struct CodeEntry {
    CodeEntryType code_entry_type;
    union {
        Op op;
        Label label;
    } entry;
} CodeEntry;

typedef enum UnitType {
    UNIT_FUNC,
    UNIT_PROC
} UnitType;

typedef struct Unit {
    UnitType unit_type;
    IdentifierEntry * id;
    CodeEntry * code;
    int num_code_entries;
    int capacity;
    struct Unit * next;
} Unit;

void add_unit(UnitType unit_type, IdentifierEntry * id);
void unit_add_op(Unit * this, Op op);
void unit_add_label(Unit * this, Label label);
void generate_code();

