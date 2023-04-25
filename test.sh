make clean && make

run_test() {
    ./pl1
    $1
}

test_function_definition() {
    INPUT="function func(a : int2) { }"
    EXPECTED="?"
    run_test $INPUT $EXPECTED
}

test_function_definition

