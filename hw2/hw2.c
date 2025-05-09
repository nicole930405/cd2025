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

typedef enum {
    START,
    ID,
    NUMBER,
    OPERATOR,
    DONE
} State;

const char* check_keyword(const char* str) {
    if (strcmp(str, "int") == 0) return "TYPE_TOKEN";
    if (strcmp(str, "main") == 0) return "MAIN_TOKEN";
    if (strcmp(str, "if") == 0) return "IF_TOKEN";
    if (strcmp(str, "else") == 0) return "ELSE_TOKEN";
    if (strcmp(str, "while") == 0) return "WHILE_TOKEN";
    return "ID_TOKEN";
}

Node* run_scanner(const char* filename){
    Node* head = NULL;

    FILE* file = fopen(filename, "r");

    if (!file) {
        printf("File open error!\n");
        exit(1);
    }

    State state = START;
    int c;
    char buffer[128];
    int idx = 0;

    while ((c = fgetc(file)) != EOF) {
        switch (state) {
            case START:
                if (is_space(c)) {
                    continue;
                } else if (is_alpha(c) || c == '_') {
                    state = ID;
                    buffer[idx++] = c;
                } else if (is_digit(c)) {
                    state = NUMBER;
                    buffer[idx++] = c;
                } else {
                    state = OPERATOR;
                    buffer[idx++] = c;
                }
                break;

            case ID:
                if (is_alnum(c) || c == '_') {
                    buffer[idx++] = c;
                } else {
                    buffer[idx] = '\0';
                    const char* token = check_keyword(buffer);
                    new_node(&head, buffer, token);
                    idx = 0;
                    state = START;
                    ungetc(c, file); // 回退一個字元
                }
                break;

            case NUMBER:
                if (is_digit(c)) {
                    buffer[idx++] = c;
                } else {
                    buffer[idx] = '\0';
                    new_node(&head, buffer, "LITERAL_TOKEN");
                    idx = 0;
                    state = START;
                    ungetc(c, file);
                }
                break;

            case OPERATOR:
                buffer[idx] = '\0';

                if ((buffer[0] == '=' && c == '=') ||
                    (buffer[0] == '<' && c == '=') ||
                    (buffer[0] == '>' && c == '=')) {
                    buffer[1] = c;
                    buffer[2] = '\0';

                    if (strcmp(buffer, "==") == 0) new_node(&head, "==", "EQUAL_TOKEN");
                    else if (strcmp(buffer, "<=") == 0) new_node(&head, "<=", "LESSEQUAL_TOKEN");
                    else if (strcmp(buffer, ">=") == 0) new_node(&head, ">=", "GREATEREQUAL_TOKEN");
                } else {
                    ungetc(c, file);
                    if (buffer[0] == '=') new_node(&head, "=", "ASSIGN_TOKEN");
                    else if (buffer[0] == '<') new_node(&head, "<", "LESS_TOKEN");
                    else if (buffer[0] == '>') new_node(&head, ">", "GREATER_TOKEN");
                    else if (buffer[0] == '(') new_node(&head, "(", "LEFTPAREN_TOKEN");
                    else if (buffer[0] == ')') new_node(&head, ")", "RIGHTPAREN_TOKEN");
                    else if (buffer[0] == '{') new_node(&head, "{", "LEFTBRACE_TOKEN");
                    else if (buffer[0] == '}') new_node(&head, "}", "RIGHTBRACE_TOKEN");
                    else if (buffer[0] == ';') new_node(&head, ";", "SEMICOLON_TOKEN");
                    else if (buffer[0] == '+') new_node(&head, "+", "PLUS_TOKEN");
                    else if (buffer[0] == '-') new_node(&head, "-", "MINUS_TOKEN");
                }

                idx = 0;
                state = START;
                break;

            default:
                break;
        }
    }

    // 如果檔案結尾時還有字元沒處理（例如在 ID 或 NUMBER 狀態）
    if (state == ID || state == NUMBER) {
        buffer[idx] = '\0';
        if (state == ID) {
            const char* token = check_keyword(buffer);
            new_node(&head, buffer, token);
        } else {
            new_node(&head, buffer, "LITERAL_TOKEN");
        }
    }

    fclose(file);
    return head;
}


Node* current_token = NULL;

void syntax_error(){
    printf("Parse Error\n");
    exit(1);
}

void advance() {
    if(current_token != NULL){
        current_token = current_token -> next;
    }
}

void print(int indent) {
    for(int i = 0; i < indent; i++){
        printf("    ");
    }
}


void parse_S(int indent);
void parse_S_(int indent);
void parse_E(int indent);

void print_line(int indent, const char* fmt, const char* content) {
    printf("%*s", indent, "");
    printf(fmt, content);
}

void parse_S(int indent){
    printf("%*sS -> E S'\n", indent, "");
    parse_E(indent + 4);
    parse_S_(indent + 4);
}

void parse_S_(int indent){
    if (current_token != NULL && strcmp(current_token->token_type, "PLUS_TOKEN") == 0) {
        printf("%*sS' -> + S\n", indent, "");
        print_line(indent + 4, "%s\n", "+");
        advance();
        parse_S(indent + 4);
    } else if (current_token == NULL || strcmp(current_token->token_type, "RIGHTPAREN_TOKEN") == 0) {
        printf("%*sS' -> EPSILON\n", indent, "");
    } else {
        syntax_error();
    }
}

void parse_E(int indent){
    if (current_token == NULL) syntax_error();

    if (strcmp(current_token->token_type, "LITERAL_TOKEN") == 0) {
        printf("%*s%s\n", indent, "", current_token->word);
        advance();
    } else if (strcmp(current_token->token_type, "LEFTPAREN_TOKEN") == 0) {
        printf("%*sE -> ( S )\n", indent, "");
        advance();
        parse_S(indent + 4);
        if (current_token == NULL || strcmp(current_token->token_type, "RIGHTPAREN_TOKEN") != 0)
            syntax_error();
        advance();
    } else {
        syntax_error();
    }
}

void parse(Node* token_list) {
    current_token = token_list;
    parse_S(0);
    if (current_token != NULL) syntax_error();
}

int main() {
    
    Node* tokens = run_scanner("test.c");
    parse(tokens);
    free_data(tokens);


    return 0;
}
