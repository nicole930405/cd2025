#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    char* word;
    char* token_type;
    struct node *next;
} Node;

void new_node(Node** head, const char* word, const char* token_type){

    Node* point = *head;

    // add node at last
    if(*head == NULL){
        //new node
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode -> word = strdup(word);
        newNode -> token_type = strdup(token_type);
        newNode -> next = NULL;
        *head = newNode;
    }else{
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode -> word =strdup(word);
        newNode -> token_type = strdup(token_type);
        newNode -> next = NULL;
        while(point -> next != NULL){
            //not the last
            point = point -> next;
        }
        point -> next = newNode;
    }
}

void print(Node* head) {
    Node* point = head;
    while(point != NULL){
        //not the end
        printf("%s:%s\n", point -> word, point -> token_type);
        point = point -> next;
    }
}

void free_data(Node* head){
    Node* point = head;
    while(point != NULL){
        point = head;
        head = head -> next;
        free(point);
    }
}

int is_alpha(char c){ //是字母
    return(c >= 'a' && c <= 'z') || (c >= 'A' && c <='Z');
}

int is_digit(char c){ //是數字
    return c >= '0' && c <='9';
}

int is_alnum(char c){
    return is_alpha(c) || is_digit(c); //是字母或數字
}

int is_space(char c){
    return c == ' ' || c == '\n' || c == '\t';
}//要忽略的

const char* check_keyword(const char* str) {
    if (strcmp(str, "int") == 0) return "TYPE_TOKEN";
    if (strcmp(str, "main") == 0) return "MAIN_TOKEN";
    if (strcmp(str, "if") == 0) return "IF_TOKEN";
    if (strcmp(str, "else") == 0) return "ELSE_TOKEN";
    if (strcmp(str, "while") == 0) return "WHILE_TOKEN";
    return "ID_TOKEN";
}

int main() {
    Node* head = NULL;
    char get_word;

    //read itself
    FILE* file = fopen("test.c", "r");

    int in_char;
    int c;

    while ((in_char = fgetc(file)) != EOF) {
        if (is_space(in_char)) //空格
            continue;
        // ID or keyword
        if (is_alpha(in_char) || in_char == '_') { //是字母或底線
            char buffer[128];
            int idx = 0;
            buffer[idx++] = in_char;

            while ((c = fgetc(file)), is_alnum(c) || c == '_') {
                buffer[idx++] = c; //是字母或底線或空白
            }
            buffer[idx] = '\0';
            ungetc(c, file);

            const char* token = check_keyword(buffer);
            new_node(&head, buffer, token);

        }

        // INT literal
        else if (is_digit(in_char)) {
            char buffer[64];
            int idx = 0;
            buffer[idx++] = in_char;
            while ((c = fgetc(file)), is_digit(c)) {
                buffer[idx++] = c;
            }
            buffer[idx] = '\0';
            ungetc(c, file);
            new_node(&head, buffer, "LITERAL_TOKEN");
        }

        // Two-char operators
        else {
            c = fgetc(file);
            char op[3] = {0};
            op[0] = in_char;
            op[1] = c; 
            op[2] = '\0';

            if (in_char == '=' && c == '=') {
                new_node(&head, "==", "EQUAL_TOKEN");
            } else if (in_char == '<' && c == '=') {
                new_node(&head,"<=", "LESSEQUAL_TOKEN");
            } else if (in_char == '>' && c == '=') {
                printf(">=: GREATEREQUAL_TOKEN\n");
                new_node(&head, ">=", "GREATEREQUAL_TOKEN");
            } else {
                ungetc(c, file);
                op[1] = '\0';
                if (in_char == '=') new_node(&head, "=", "ASSIGN_TOKEN");
                else if (in_char == '<') new_node(&head, "<", "LESS_TOKEN");
                else if (in_char == '>') new_node(&head, ">", "GREATER_TOKEN");
                else if (in_char == '(') new_node(&head, "(", "LEFTPAREN_TOKEN");
                else if (in_char == ')') new_node(&head, ")", "RIGHTPAREN_TOKEN");
                else if (in_char == '{') new_node(&head, "{", "LEFTBRACE_TOKEN");
                else if (in_char == '}') new_node(&head, "}", "RIGHTBRACE_TOKEN");
                else if (in_char == ';') new_node(&head, ";", "SEMICOLON_TOKEN");
                else if (in_char == '+') new_node(&head, "+", "PLUS_TOKEN");
                else if (in_char == '-') new_node(&head, "-", "MINUS_TOKEN");
            }
        }
    }

    print(head);

    free_data(head);

    fclose(file);

    return 0;
}
