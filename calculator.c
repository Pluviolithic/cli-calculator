#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>
#include <math.h>

typedef struct node {
    char *data;
    int type;
    struct node *next;
} node;

typedef struct stack {
    node *data;
    struct stack *next;
} stack;

typedef struct tuple {
    int token_count;
    node *first_node;
} tuple;

void free_linked_list(node *n) {
    while (n != NULL) {
        node *tmp = n;
        free(n->data);
        free(tmp);
        n = n->next;
    }
}

node *copy_node(node *n, bool free_data) {
    node *tmp = malloc(sizeof(node));
    if (tmp == NULL) {
        exit(1);
    }
    tmp->data = malloc(strlen(n->data) + 1);
    if (tmp->data == NULL) {
        exit(1);
    }
    strcpy(tmp->data, n->data);
    tmp->type = n->type;
    tmp->next = NULL;
    if (free_data) {
        free(n->data);
        free(n);
    }
    return tmp;
}

regex_t operator;
regex_t number;
regex_t left_bracket;
regex_t right_bracket;
stack *top;

void initialize_stack(void) {
    top = NULL;
}

void push(node *n) {
    stack *tmp;
    tmp = malloc(sizeof(stack));
    if (tmp == NULL) {
        exit(1);
    }
    tmp->data = copy_node(n, false);
    tmp->next = top;
    top = tmp;
}

node *pop(void) {
    stack *tmp;
    tmp = top;
    node *data = tmp->data;
    top = top->next;
    free(tmp);
    return data;
}

node *read_top(void) {
    if (top != NULL) {
        return top->data;
    }
    return NULL;
}

bool stack_is_empty(void) {
    return top == NULL;
}

double calculate_function(char *function_name, double n) {
    double result = -1;
    if (strcmp(function_name, "sin") == 0) {
        result = sin(n);
    }
    else if (strcmp(function_name, "cos") == 0) {
        result = cos(n);
    }
    else if (strcmp(function_name, "tan") == 0) {
        result = tan(n);
    }
    else if (strcmp(function_name, "sinh") == 0) {
        result = sinh(n);
    }
    else if (strcmp(function_name, "cosh") == 0) {
        result = cosh(n);
    }
    else if (strcmp(function_name, "tanh") == 0) {
        result = tanh(n);
    }
    else if (strcmp(function_name, "ln") == 0) {
        result = log(n);
    }
    return result;
}

double get_const(char *const_name) {
    if (strcmp(const_name, "pi") == 0) {
        return M_PI;
    }
    else if (strcmp(const_name, "e") == 0) {
        return M_E;
    }
    return -1;
}

bool is_function(char *name) {
    if (strcmp(name, "sin") == 0 || strcmp(name, "cos") == 0 || strcmp(name, "tan") == 0 || strcmp(name, "sinh") == 0 || strcmp(name, "cosh") == 0 || strcmp(name, "tanh") == 0 || strcmp(name, "ln") == 0) {
        return true;
    }
    return false;
}

bool is_left_associative(char operator) {
    if (operator == '^' || operator == 'u') {
        return false;
    }
    return true;
}


int rank_operator(char operator) {
    if ((int)operator == 42 || (int)operator == 47) {
        return 1;
    }
    else if ((int)operator == 94 || (int)operator == 117) {
        return 2;
    }
    return 0;
}

char *substr(const char *src, int n, int m) {
    int len = m - n;
    char *dest = (char*)malloc(sizeof(char)*(len + 1));
    if (dest == NULL) {
        exit(1);
    }
    for (int i = n; i < m && (*(src + i) != '\0'); i++) {
        *dest = *(src + i);
        dest++;
    }
    *dest = '\0';
    return dest - len; 
}

void initialize_regex(void) {
    regcomp(&operator, "[\\^\\*\\/\\+\\-]", 0);
    regcomp(&number, "[0-9.]", 0);
    regcomp(&left_bracket, "[[({]", 0);
    regcomp(&right_bracket, "[])}]", 0);
}

int get_type(char c) { 
    int is_operator = regexec(&operator, &c, 0, NULL, 0);
    int is_number = regexec(&number, &c, 0, NULL, 0);
    int is_left_bracket = regexec(&left_bracket, &c, 0, NULL, 0);
    int is_right_bracket = regexec(&right_bracket, &c, 0, NULL, 0);
    if (is_operator == 0) {
        return 0;
    }
    else if (is_number == 0) {
        return 1;
    }
    else if (is_left_bracket == 0) {
        return 2;
    }
    else if (is_right_bracket == 0) {
        return 3;
    }
    else {
        return 4;
    }
}

tuple *lex_input(const char *expression) {
    int current_type = -1;
    int current_type_start_index = 0;
    int size = sizeof(node);
    int token_count = 0;
    bool set_unary_minus = true;
    node *first_node = NULL;
    node *current_node;
    for (int i = 0, n = strlen(expression); i < n + 1; i++) {
        int type = get_type(expression[i]);
        if (type != current_type || type == 0 || i == n) {
            if (current_type_start_index != i) {
                int type_to_assign = current_type;
                char *new_data = substr(expression, current_type_start_index, i);
                node *tmp = malloc(size);
                if (tmp == NULL) {
                    exit(1);
                }
                if (is_function(new_data)) {
                    type_to_assign = 5;
                }
                else if (get_const(new_data) != -1) {
                    type_to_assign = 6;
                }
                else if (*new_data == '-' && set_unary_minus) {
                    type_to_assign = 7;
                }
                if (current_type_start_index == 0) {
                    first_node = tmp;
                }
                else {
                    current_node->next = tmp;
                }
                if (type_to_assign == 1 || type_to_assign == 6) {
                    set_unary_minus = false;
                }
                else if (type_to_assign == 0 || type_to_assign == 5) {
                    set_unary_minus = true;
                }
                if (type_to_assign == 7) {
                    *new_data = 'u';
                }
                tmp->data = malloc(i - current_type_start_index);
                if (tmp->data == NULL) {
                    exit(1);
                }
                strcpy(tmp->data, new_data);
                free(new_data);
                tmp->type = type_to_assign;
                tmp->next = NULL;
                current_node = tmp;
                token_count++;
            }
            current_type_start_index = i;
            current_type = type;
        }
    }
    tuple *return_tuple = malloc(sizeof(tuple));
    if (return_tuple == NULL) {
        exit(1);
    }
    return_tuple->token_count = token_count;
    return_tuple->first_node = first_node;
    return return_tuple;
}

tuple *convert_to_rpn(int stack_size, node *token) {
    initialize_stack();
    int size = sizeof(node);
    int new_stack_size = 0;
    bool initialized = false;
    node *first_node = NULL;
    node *current_node;
    while (token != NULL) { 
        if (token->type != 4) {
            if (token->type == 1 || token->type == 6) {
                node *tmp = copy_node(token, false);
                if (!initialized) {
                    first_node = tmp;
                    initialized = true;
                }
                else {
                    current_node->next = tmp;
                }
                current_node = tmp;
                new_stack_size++; 
            }
            else if (token->type == 5) {
                push(token);
            }
            else if (token->type == 0 || token->type == 7) { 
                int rank = rank_operator(*token->data);
                while (read_top() != NULL && read_top()->type != 2 && (rank_operator(*read_top()->data) > rank || (rank_operator(*read_top()->data) == rank && is_left_associative(*read_top()->data)))) { 
                    node *tmp = copy_node(pop(), true); 
                    if (!initialized) {
                        first_node = tmp;
                        initialized = true;
                    }
                    else {
                        current_node->next = tmp;
                    }
                    current_node = tmp;
                    new_stack_size++;
                }
                push(token);
            }
            else if (token->type == 2) {
                push(token);
            }
            else if (token->type == 3) {
                while (read_top()->type != 2) {
                    node *tmp = copy_node(pop(), true);
                    if (!initialized) {
                        first_node = tmp;
                        initialized = true;
                    }
                    else {
                        current_node->next = tmp;
                    }
                    current_node = tmp;
                    new_stack_size++;
                }
                if (read_top()->type == 2) {
                    pop();
                }
            }
        }
        token = token->next; 
    } 
    while (!stack_is_empty()) {
        node *tmp = copy_node(pop(), true);
        if (!initialized) {
            first_node = tmp;
            initialized = true;
        }
        else {
            current_node->next = tmp;
        } 
        current_node = tmp;
        new_stack_size++;
    }
    tuple *return_tuple = malloc(sizeof(tuple));
    if (return_tuple == NULL) {
        exit(1);
    }
    return_tuple->token_count = new_stack_size;
    return_tuple->first_node = first_node;
    return return_tuple;
}

double evaluate_rpn_expression(int array_size, node *n) {
    double array[array_size];
    int i = -1;
    while (n != NULL) { 
        if (n->type == 1) {
            i++;
            array[i] = strtod(n->data, NULL);
        }
        else if (n->type == 5) {
            array[i] = calculate_function(n->data, array[i]);
        }
        else if (n->type == 6) {
            array[i] = get_const(n->data);
        }
        else if (n->type == 7) {
            array[i] = -array[i];
        }
        else {
            switch(*n->data) {
                case '^':
                    array[i] = pow(array[i - 1], array[1]);
                    break;
                case '*':
                    array[i] = array[i - 1]*array[i];
                    break;
                case '/':
                    array[i] = array[i - 1]/array[i];
                    break;
                case '+':
                    array[i] = array[i - 1] + array[i];
                    break;
                case '-':
                    array[i] = array[i - 1] - array[i]; 
                    break;
            }
            if (i > 1) {
                array[i - 1] = array[i - 2];
            }
        }
        n = n->next;
    }
    return array[i];
}

int main(void) {
    initialize_regex(); 
    char expression[50];
    printf("Input a mathematical expression to be evaluated. Enter 'quit' to exit at any time.\n");
    while (true) {
        fgets(expression, sizeof(expression), stdin);
        expression[strcspn(expression, "\n")] = 0;
        if (strcmp(expression, "quit") == 0) {
            break;
        }
        tuple *return_tuple = lex_input(expression);
        int token_count = return_tuple->token_count;

        node *return_node = return_tuple->first_node;
        tuple *rpn_tuple = convert_to_rpn(token_count, return_node);

        char result[8];
        sprintf(result, "%f", evaluate_rpn_expression(rpn_tuple->token_count, rpn_tuple->first_node));  
        int word_end = strlen(result);
        for (int i = word_end - 1; i > -1; i--) {
            if (result[i] == '0') {
                word_end = i;
            }
            else {
                break;
            }
        }
        if (result[word_end - 1] == '.') {
            word_end--;
        }
        char *formatted_result = substr(result, 0, word_end);
        printf("%s\n", formatted_result);
        free_linked_list(return_node);
        free_linked_list(rpn_tuple->first_node);
        free(formatted_result);
        free(return_tuple);
        free(rpn_tuple);
    }
    regfree(&operator);
    regfree(&number);
    regfree(&left_bracket);
    regfree(&right_bracket);
}
