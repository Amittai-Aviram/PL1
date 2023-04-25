APP = pl1
FLAGS = -Wall -DYYERROR_VERBOSE -g
SOURCES = ${APP}.tab.c ${APP}.lex.c hash_table.c symbol_table.c ${APP}_handlers.c

${APP}: ${SOURCES} main.c
	gcc $(FLAGS) $^ -o $@ -ll

${APP}.tab.c: ${APP}.y
	bison -dv $^

${APP}.lex.c: ${APP}.l
	lex -o $@ $^

test: ${SOURCES} pl1_tests.c
	gcc $(FLAGS) $^ -o $@ -ll

clean:
	rm -rf ${APP} ${APP}.tab.* ${APP}.lex.* *.output *.dSYM

