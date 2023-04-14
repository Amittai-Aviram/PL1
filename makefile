APP = lang1
FLAGS = -Wall -DYYERROR_VERBOSE -g

${APP}: ${APP}.tab.c ${APP}.lex.c hash_table.c symbol_table.c lang1_handlers.c
	gcc $(FLAGS) $^ -o $@ -ll

${APP}.tab.c: ${APP}.y
	bison -dv $^

${APP}.lex.c: ${APP}.l
	lex -o $@ $^

clean:
	rm -rf ${APP} ${APP}.tab.* ${APP}.lex.* *.output *.dSYM

