APP = pl1
FLAGS = -Wall -DYYERROR_VERBOSE -g
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = test
BIN_DIR = bin
GEN_SOURCES = ${BUILD_DIR}/${APP}.tab.c ${BUILD_DIR}/${APP}.lex.c
SOURCES += ${SRC_DIR}/${APP}_handlers.c
SOURCES += ${SRC_DIR}/hash_table.c ${SRC_DIR}/symbol_table.c ${SRC_DIR}/ir.c
SOURCES += ${SRC_DIR}/main.c

${BIN_DIR}/${APP}: ${GEN_SOURCES} ${SOURCES}
	cp ${SRC_DIR}/* ${BUILD_DIR}
	gcc $(FLAGS) ${BUILD_DIR}/*.c -o $@ -ll

${BUILD_DIR}/${APP}.tab.c: ${SRC_DIR}/${APP}.y
	bison -dv $^ -o $@

${BUILD_DIR}/${APP}.lex.c: ${SRC_DIR}/${APP}.l
	lex -o $@ $^

test: ${SOURCES} ${TEST_DIR}/pl1_tests.c
	gcc $(FLAGS) $^ -o ${TEST_DIR}/test -ll

clean:
	rm -rf ${BIN_DIR}/* ${BUILD_DIR}/*

