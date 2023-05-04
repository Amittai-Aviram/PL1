/*
 * Author: Amittai Aviram - aviram@bc.edu
 */
#pragma once

#include "symbol_table.h"

#define CODE_ENTRY_CHUNK 16

typedef enum Opcode {
    MOV,
    MOVS,
    MOVZ,
    TEST,
    CMP,
    SETE,
    SETNE,
    SETL,
    SETLE,
    SETGE,
    SETG,
    SETZ,
    SETNZ,
    JMP,
    JE,
    JNE,
    JL,
    JLE,
    JGE,
    JG,
    JNZ,
    JZ,
    ADD,
    SUB,
    MUL,
    IMUL,
    DIV,
    IDIV,
    MOD,
    AND,
    OR,
    CALL,
    ADDR,
    RET
} Opcode;

typedef enum CodeEntryType {
    OP,
    LABEL
} CodeEntryType;

typedef enum AddrType {
    PARAM,
    MEM,
    REG
} AddrType;

typedef struct Addr {
    AddrType addr_type;
    int num;
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
} Unit;

