typedef struct Info {
    char string[256];
    int type_id;
} Info;

typedef struct LabelNo {
    int num;
    struct LabelNo * next;
} LabelNo;

LabelNo * new_label_no(int num);
int push_label_no(int true);
int pop_label_no(int true);
void get_new_register(char * buffer);
void get_new_mem(char * buffer);
void get_new_param(char * buffer);
void reset_params();
void handle_arithmetic_expression(char * buffer, int op, char * a, char * b);
void handle_assignment(char * buffer, char * source, char * destination);
void handle_relational_expression(Info * lhs, int op, char * a, char * b);
void handle_logical_expression(char * buffer, int op, char * a, char * b);
void handle_arg(Info * arg, Info * expression);

