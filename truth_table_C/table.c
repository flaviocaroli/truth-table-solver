#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// Forward declaration of TreeNode
struct TreeNode;

// Entry structure definition
typedef struct {
    char *key;
    struct TreeNode *node;  // Use the forward-declared TreeNode pointer
} Entry;

// Dictionary (Hash Table) structure definition
typedef struct {
    Entry **entries; // Array of pointers to entries
    unsigned long size; // Size of the hash table
    char **vars; // Added: Array to store variable names
} Dict;

// Full TreeNode definition
typedef struct TreeNode {
    int (*evaluate)(struct TreeNode*, Dict *assignments);  // Function pointer for evaluating the tree node
} TreeNode;


// Token list definition
typedef struct {
    char **tokens;  // Array of token strings (each token is a dynamically allocated string)
    char **types;
    size_t size;    // Number of tokens stored
    size_t capacity;  // Total list space used
} TokenList;


//utilities for arrays
char** add(char **array, char *new_element);
void free_array(char **array, size_t size);
size_t len_array(char **array);
size_t total_arrlen(char **array);
void free_tree(TreeNode *node);
char** concatenate(char **arr1, char **arr2);

//DICT prototypes

unsigned long hash(const char *str, unsigned long size);
Dict* initialize_dict(unsigned long size);
void insert(Dict *assignments, const char *key, TreeNode *node);
TreeNode* get(Dict *assignments, const char *key);
void free_dict(Dict* assignments);
Dict* assignment(TokenList *token_list, int *index, char **variables);


//ARRAY functions
size_t len_array(char **array){
    if (array == NULL) return 0;
    size_t count = 0;
    while(array[count] != NULL){
        count++;
    }
    return count;
}

size_t total_arrlen(char **array){
    size_t total_length = 0;
    size_t num_strings = len_array(array);

    for(size_t i = 0; i < num_strings; i++){
        total_length += strlen(array[i]);
    }
    return total_length;
}


void free_array(char **array, size_t size){
    for(size_t i = 0; i < size; i++){
        free(array[i]);
    }
    free(array);
}


char** add(char **array, char *new_element){
    size_t length = len_array(array);

    //+2 because it adds memory for the NULL terminator
    char **new_array = realloc(array, (length + 2) * sizeof(char *));

    if (new_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);  // Exit if realloc fails
    }
    // +1 in memory allocation to take into account NULL terminator
    new_array[length] = malloc((strlen(new_element) + 1) * sizeof(char));
    if (new_array[length] == NULL){
        fprintf(stderr, "memory alloc failed \n");
        exit(1);

    }
    strcpy(new_array[length], new_element);
    new_array[length + 1] = NULL;

    return new_array;
}

char** concatenate(char **arr1, char **arr2){
    size_t size1 = len_array(arr1);
    size_t size2 = len_array(arr2);
    char **result = malloc((size1 + size2) * sizeof(char*));
    if (result == NULL){
        fprintf(stderr, "Memory allocation failed");
        exit(1);
    }

    //copy elements of arr1
    for(size_t i = 0; i < size1; i++){
        result[i] = malloc((strlen(arr1[i])+ 1) * sizeof(char));
        if (result[i] == NULL){
            fprintf(stderr, "Memory allocation failed");
            exit(1);
        }
        strcpy(result[i], arr1[i]);
    }
    //copy elements of arr2
    for(size_t j = 0; j < size1; j++){
        result[size1 + j] = malloc((strlen(arr2[j])+ 1) * sizeof(char));
        if (result[size1 + j] == NULL){
            fprintf(stderr, "Memory allocation failed");
            exit(1);
        }
        strcpy(result[size1 + j], arr2[j]);
    }

    return result;
}

/* ASSIGNMENT */

unsigned long hash(const char *str, unsigned long size){
    // 5381 gives a good distribution of hash values for a variety of strings.
    unsigned long hash = 5381;
    int c; 
    //process each character in string
    while ((c = *str++)){
        //hash val = hash * 33 + current
        hash = ((hash << 5 )+ hash) + c;
    }
    return hash % size;
}

Dict* initialize_dict(unsigned long size){
    Dict *dict = malloc(sizeof(Dict));
    dict->size = size;
    // setting the allocated memory to zero
    dict->entries = (Entry**)calloc(size, sizeof(Entry*));
    dict->vars = NULL;
    return dict;
}

void insert(Dict *assignments, const char *key, TreeNode *node){
    unsigned long index = hash(key, assignments->size);

    //create new entry
    Entry *new_entry = malloc(sizeof(Entry));
    if (new_entry == NULL){
        fprintf(stderr, "Memory allocation failed");
        exit(1);
    }
    new_entry->key = malloc(sizeof(strlen(key) + 1 )); //+1 for null terminator
    if (new_entry->key == NULL) {
        // Handle malloc failure
        free(new_entry);
        fprintf(stderr, "Memory allocation failed");
        exit(1); 
    }
    new_entry->node = node;

    if (new_entry->key == NULL || new_entry->node == NULL) {
        // Handle malloc failure
        free(new_entry->key);
        free(new_entry->node);
        free(new_entry);
        fprintf(stderr, "Memory allocation failed");
        exit(1); 
    }

    strcpy(new_entry->key, key);
    
    assignments->entries[index] = new_entry;

    //assume variables were not added before
    assignments->vars = add(assignments->vars, (char*)key);
    
    //assume there won't be collisions, use linked lists otherwise
    
}

TreeNode* get(Dict* assignments, const char *key){
    unsigned long index = hash(key, assignments->size);

    Entry *entry = assignments->entries[index];
    if(entry != NULL && strcmp(entry->key, key) == 0){
        return entry->node;
    }
    
    return NULL;
}

void free_dict(Dict *assignments){
    for(unsigned long i = 0; i < assignments->size; i++){
        if (assignments->entries[i] != NULL){
            free(assignments->entries[i]->key);
            free_tree(assignments->entries[i]->node);
            free(assignments->entries[i]);
        }
    }
    free(assignments->entries);
    for (int i = 0; assignments->vars[i] != NULL; i++){
        free(assignments->vars[i]);
    }
    free(assignments->vars);
    free(assignments);
}

/* ABSTRACT SYNTAX TREE */

// Bool Node
typedef struct {
    TreeNode base;
    int value;
} BoolNode;

int evaluate_boolean(TreeNode *node, Dict *assignments) {
    BoolNode *boolNode = (BoolNode*) node;
    return boolNode->value;   
}

TreeNode* create_bool(int value) {
    BoolNode *node = malloc(sizeof(BoolNode));  
    node->value = value;  
    node->base.evaluate = evaluate_boolean;  
    return (TreeNode*) node;  
}

// Variable Node
typedef struct {
    TreeNode base;
    char *name;  // Flexible array member
} Var;

int evaluate_variable(TreeNode *node, Dict *assignments) {
    Var *varNode = (Var*) node;
    TreeNode *assigned_node = get(assignments, varNode->name);
    if (assigned_node == NULL) {
        fprintf(stderr, "Variable %s not found in assignments\n", varNode->name);
        exit(1);
    }
    return assigned_node->evaluate(assigned_node, assignments);
}

TreeNode* create_var(char *name) {
    Var *node = malloc(sizeof(Var));  // Allocate for Var only
    node->base.evaluate = evaluate_variable;
    node->name = malloc(strlen(name) + 1);  // Allocate separately for the name
    strcpy(node->name, name);  // strcpy method from string.h that copies a string
    return (TreeNode*) node; 
}

// Not Node
typedef struct {
    TreeNode base;
    TreeNode *child;
} Not;

int evaluate_not(TreeNode *node, Dict *assignments) {
    Not *notNode = (Not*) node;  // Cast to Not type
    return !(notNode->child->evaluate(notNode->child, assignments));
}

TreeNode* create_not(TreeNode *child) {
    Not *node = malloc(sizeof(Not));
    node->base.evaluate = evaluate_not;
    node->child = child;
    return (TreeNode*) node;
}

// Or Node
typedef struct {
    TreeNode base;
    TreeNode *left;
    TreeNode *right; 
} Or;

int evaluate_or(TreeNode *node, Dict *assignments) {
    Or *orNode = (Or*) node;  // Cast to Or type
    return orNode->left->evaluate(orNode->left, assignments) ||
           orNode->right->evaluate(orNode->right, assignments);
}

TreeNode* create_or(TreeNode* left, TreeNode* right) {
    Or *node = malloc(sizeof(Or));  
    node->base.evaluate = evaluate_or;  
    node->left = left; 
    node->right = right;
    return (TreeNode*) node;  
}

// And Node
typedef struct {
    TreeNode base;
    TreeNode *left;
    TreeNode *right;
} And;

int evaluate_and(TreeNode *node, Dict *assignments) {
    And *andNode = (And*) node;  // Cast to And type
    return (andNode->left->evaluate(andNode->left, assignments) &&
            andNode->right->evaluate(andNode->right, assignments));
}

TreeNode* create_and(TreeNode *left, TreeNode *right) {
    And *node = malloc(sizeof(And));
    node->base.evaluate = evaluate_and;
    node->left = left;
    node->right = right;
    return (TreeNode*) node;
}

void free_tree(TreeNode *node){
    if (node == NULL){
        return;
    }
    if (((BoolNode*)node)->base.evaluate == evaluate_boolean) {
        free(node);
    } else if (((Var*)node)->base.evaluate == evaluate_variable) {
        free(((Var*)node)->name);
        free(node);
    } else if (((Not*)node)->base.evaluate == evaluate_not) {
        free_tree(((Not*)node)->child);
        free(node);
    } else if (((And*)node)->base.evaluate == evaluate_and ||
               ((Or*)node)->base.evaluate == evaluate_or) {
        free_tree(((And*)node)->left);
        free_tree(((And*)node)->right);
        free(node);
    }

}
/*FUNCTION PROTOTYPES*/

// 1) Tokenizer
TokenList* create_token_list(size_t initial_capacity);
void add_token(TokenList *list, const char *token, const char *type);
void free_token_list(TokenList *list);
TokenList* tokenize(char *input_data);
int is_comment_or_empty(const char *line);
int is_keyword(const char *word);

// 2) Parsing
int startswith(char *string, char *prefix);

// 2.1) Utility functions
int belongs_to(char **array, char *element);
char** variable_declaration(TokenList *token_list, int *index);
TreeNode* parsing(TokenList *token_list, int *index);

// 3) Show
void show(Dict *assignments, char **variables, char **variables_to_show);
void show_ones(Dict *assignments, char **variables, char **variables_to_show);

// 4) Other
TokenList* read_file(const char *input_file);


/* TOKENIZATION */

// Create a token list
TokenList* create_token_list(size_t initial_capacity) {
    TokenList* list = malloc(sizeof(TokenList));
    list->tokens = malloc(initial_capacity * sizeof(char *));
    list->types = malloc(initial_capacity * sizeof(char *));
    list->size = 0;
    list->capacity = initial_capacity;
    return list;
}

// Add a token to the list
void add_token(TokenList *list, const char *token, const char *type) {
    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, list->capacity * sizeof(char *));
        list->types = realloc(list->types, list->capacity * sizeof(char *));
    }
    list->tokens[list->size] = malloc((strlen(token) + 1) * sizeof(char));
    list->types[list->size] = malloc((strlen(type) + 1) * sizeof(char));
    strcpy(list->tokens[list->size], token);
    strcpy(list->types[list->size], type);
    list->size++;
}

// Free the token list
void free_token_list(TokenList *list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->tokens[i]);
        free(list->types[i]);
    }
    free(list->types);
    free(list->tokens);
    free(list);
}

// Special tokens and keywords
const char *special[] = {"(", ")", "=", ";"};
const char *keywords[] = {"var", "show", "show_ones", "and", "or", "not", "True", "False"};

// Check if a word is a keyword
int is_keyword(const char *word) {
    size_t num_keywords = sizeof(keywords) / sizeof(keywords[0]);
    for (size_t i = 0; i < num_keywords; i++) {
        if (strcmp(word, keywords[i]) == 0) {
            return 1;  // Found a keyword
        }
    }
    return 0;  // Not a keyword
}

// Check if the line is a comment or empty
int is_comment_or_empty(const char *line) {
    while (isspace(*line)) {
        line++;
    }
    return (*line == '#' || *line == '\0');
}

// Tokenizer function
TokenList* tokenize(char *input_data) {
    TokenList *token_list = create_token_list(10);  // Create an initial token list

    char *line = strtok(input_data, "\n"); //splits input data according to a separator
    while (line != NULL) {
        if (is_comment_or_empty(line)) {
            line = strtok(NULL, "\n");
            continue;  // Skip comments and empty lines
        }

        int length = strlen(line);
        char word[256]; // Buffer to store construction of words
        memset(word, 0, sizeof(word)); // Initialize word

        for (int i = 0; i < length; ) {
            char current = line[i];

            // special 
            if (strchr("();=", current)) {
                if (strlen(word) > 0) {
                    // Modify to add identifier with correct prefix
                    add_token(token_list, word, is_keyword(word) ? "keyword" : "identifier"); // Token type handling
                    memset(word, 0, sizeof(word)); // Clear the word
                }

                char special_token[2] = {current, '\0'}; // Create a string for the special character
                add_token(token_list, special_token, "special");
                i++;
                continue;
            }

            // whitespace
            if (isspace(current)) {
                if (strlen(word) > 0) {
                    // Modify to add identifier with correct prefix
                    add_token(token_list, word, is_keyword(word) ? "keyword" : "identifier"); // Token type handling
                    memset(word, 0, sizeof(word)); // Clear the word
                }
                i++;
                continue;
            }

            // words
            if (isalnum(current) || current == '_') {
                strncat(word, &current, 1); // Add character to word buffer
                i++;
            } 
            else {
                // Handle unexpected characters
                printf("Error: Invalid character '%c' in input.\n", current);
                free_token_list(token_list);
                return NULL;
            }
        }

        // remaining word at the end of the input
        if (strlen(word) > 0) {
            // Modify to add identifier with correct prefix
            add_token(token_list, word, is_keyword(word) ? "keyword" : "identifier"); // Token type handling
        }

        line = strtok(NULL, "\n");
    }

    return token_list;  // Return the token list
} 

/* VARIABLE DECLARATION */

int startswith(char *string, char *prefix){
    size_t str_len = strlen(string);
    size_t pre_len = strlen(prefix);
    //handles the case prefix is longer than string (most of the times)
    //strncmp to set the number of character sto compare
    return (str_len >= pre_len) && (strncmp(string, prefix, strlen(prefix)) == 0); 
}

int belongs_to(char **array, char *element){
    size_t length = len_array(array);
    for(size_t i = 0; i < length; i++){
        if (strcmp(array[i], element) == 0){
            return 1;
        }
    }
    return 0;
}

char** variable_declaration(TokenList *token_list, int *index){
    
    int num_variables = 0;

    for(int i = 0; i < token_list->size; i++){
        if(startswith(token_list->tokens[i], "identifier: ")){
            num_variables +=1;
        }
    }
    //allocate memory for variables, +1 for null terminator
    char **variables = calloc(num_variables + 1, sizeof(char *));

    //The NULL terminator acts as a sentinel value, marking the end of the array.
    //useful when you don't know the exact number of elements in the array or if
    //you're dynamically allocating memory
    if(variables == NULL){
        fprintf(stderr, "memory allocation failed \n");
        exit(1);
    }
    
    int var_index = 0;
    char **tokens = token_list->tokens;
    char **types = token_list->types;

    if (strcmp(tokens[*index], "var") != 0){
        fprintf(stderr, "The first token should be var");
        free_array(variables, len_array(variables));
        exit(1);
    }
    (*index)++;

    while(*index < token_list->size){

        char *curr_tok= tokens[*index];
        char *curr_type = types[*index];
        if (strcmp(curr_tok, ";") == 0){
            (*index)++;
            break;
        }
        if (strcmp(curr_type, "identifier") == 0){
            char *var = curr_tok;
            
            if (var_index >= 64){
                fprintf(stderr, "Cannot declare more than 64 variables\\n");
                exit(1);
            }
            if (!isalnum(var[0]) && var[0] != '_'){
                fprintf(stderr, "Invalid identifier name %s\n", var);
                exit(1);
            }
            if (belongs_to(variables, var)){
                fprintf(stderr, "variable %s has already been declared\n", var);
                exit(1);
            }
            variables = add(variables, var);
            var_index++;
        }
        else{
            fprintf(stderr, "Expected identifier but got another type of token %s\n", curr_type);
            exit(1);
        }
        (*index)++; // Increment index for the next token
    }
    variables[var_index] = NULL;  // Null terminate the variables array
    return variables;
}

/* PARSING */

// Forward declarations with recursive calls
static TreeNode* parse_operand(TokenList *token_list, int *index);
static TreeNode* parse_and(TokenList *token_list, int *index);
static TreeNode* parse_or(TokenList *token_list, int *index);

// Main parsing function
TreeNode* parsing(TokenList *token_list, int *index) {
    return parse_or(token_list, index);
}

// Parse operand
static TreeNode* parse_operand(TokenList *token_list, int *index) {
    char **tokens = token_list->tokens;
    char **types = token_list->types;

    if ((*index) >= token_list->size) {
        fprintf(stderr, "Unexpected end of tokens while parsing\n");
        exit(1);
    }

    if (strcmp(tokens[*index], "(") == 0) {
        (*index)++;
        TreeNode* node = parse_or(token_list, index);
        if ((*index) >= token_list->size || strcmp(tokens[*index], ")") != 0) {
            fprintf(stderr, "Expected ')' after expression\n");
            exit(1);
        }
        (*index)++;
        return node;
    }
    else if (strcmp(types[*index], "identifier") == 0) {
        return create_var(tokens[(*index)++]);
    }
    else if (strcmp(types[*index], "keyword") == 0) {
        if (strcmp(tokens[*index], "True") == 0) {
            (*index)++;
            return create_bool(true);
        }
        else if (strcmp(tokens[*index], "False") == 0) {
            (*index)++;
            return create_bool(false);
        }
        else if (strcmp(tokens[*index], "not") == 0) {
            (*index)++;
            return create_not(parse_operand(token_list, index));
        }
    }

    fprintf(stderr, "Expected operand, but got %s\n", tokens[*index]);
    exit(1);
}

// Parse AND expressions
static TreeNode* parse_and(TokenList *token_list, int *index) {
    TreeNode *node = parse_operand(token_list, index);
    while ((*index) < token_list->size && 
           strcmp(token_list->types[*index], "keyword") == 0 && 
           strcmp(token_list->tokens[*index], "and") == 0) {
        (*index)++;
        node = create_and(node, parse_operand(token_list, index));
    }
    return node;
}

// Parse OR expressions
static TreeNode* parse_or(TokenList *token_list, int *index) {
    TreeNode *node = parse_and(token_list, index);
    while ((*index) < token_list->size && 
           strcmp(token_list->types[*index], "keyword") == 0 && 
           strcmp(token_list->tokens[*index], "or") == 0) {
        (*index)++;
        node = create_or(node, parse_and(token_list, index));
    }
    return node;
}


Dict* assignment(TokenList *token_list, int *index, char **variables){
    size_t size = token_list->size;
    Dict *assignments = initialize_dict(size);
    if (assignments->vars == NULL) {
        assignments->vars = malloc(sizeof(char *));
            assignments->vars[0] = NULL;
    }
    char **tokens = token_list->tokens;
    char **types = token_list->types;

    while((*index) < size){
        if(strcmp(tokens[*index], ";") == 0){
            (*index)++;
            continue;
        }
        else if(strcmp(types[*index], "identifier") == 0){
            char *var = tokens[*index];
            if(!belongs_to(assignments->vars, var) && !belongs_to(variables, var)){
                assignments->vars = add(assignments->vars, var);
            }
            (*index)++;
            if (((*index)>= size) || strcmp(tokens[*index], "=") != 0){
                fprintf(stderr, "Expected '=', got %s", tokens[*index]);
                exit(1);  
            } 

            (*index)++;
            size_t start = (*index);
            int stack_count = 0;

            while((*index) < size){
                if(strcmp(tokens[*index], ";") == 0 && stack_count == 0){
                        break;
                }
                if(strcmp(tokens[*index], "(") == 0){
                    stack_count++;
                    } 
                else if(strcmp(tokens[*index], ")") == 0){
                    stack_count--;
                    }
                (*index)++;
                }

            size_t end = (*index);
            if((*index) < size && strcmp(tokens[*index], ";") == 0){
                (*index)++;
            }
            // create a subarray of the tokens in the expression
            TokenList *exp_tokens = create_token_list(end - start + 1);
            for(size_t i = start; i < end; i++){
                add_token(exp_tokens, tokens[i], types[i]);
            }
            int exp_index = 0;
            TreeNode *expression = parsing(exp_tokens, &exp_index);
            //start -2 index since whe have expression and '='
            insert(assignments, tokens[start - 2], expression); 
            free_token_list(exp_tokens);
            }
        else if (strcmp(tokens[*index], "show") == 0 || strcmp(tokens[*index], "show_ones") == 0){
            break;
        }
        else{
            fprintf(stderr, "Unexpected token %s in assignments", tokens[*index]);
            exit(1);
        }
    }
    return assignments;
    }
    
/* SHOW TRUTH TABLE */

void show(Dict *assignments, char **variables, char **variables_to_show) {

    char **header = concatenate(variables, variables_to_show);
    //counts size of all strings
    size_t len_head = len_array(header);
    // print '#'
    printf("# ");

    //print the header array element wise
    for (size_t i = 0; i < len_head; i++){
        printf("%s", header[i]);
        //space unless last string
        if (i != len_head - 1){
            printf(" ");
        }
    }
    // newline at the end
    printf("\n");

    size_t num_vars = len_array(variables);
    // Total number of rows in the truth table (all possible combinations of 0 and 1)
    unsigned long int rows = 1UL << num_vars;

    for(unsigned long int i = 0; i < rows; i++ ){
        Dict *truth_table = initialize_dict(num_vars * 2); //num_vars * 2 to avoid collisions    

        //truth table for the row
        for(size_t j = 0; j < num_vars; j++){
            int bit = (i >> (num_vars - 1 -j)) & 1;
            TreeNode *valueNode = create_bool(bit);
            insert(truth_table, variables[j], valueNode); //iterates through all possibilities of 0 and 1 for all vars
        }

        //evaluate assignments
        for(size_t j = 0; assignments->vars[j] != NULL; j++ ){
            char *var = assignments->vars[j];
            TreeNode *expr = get(assignments, var);
            if (expr != NULL){
                int result = expr->evaluate(expr, truth_table);
                TreeNode *result_node = create_bool(result);
                insert(truth_table, var, result_node);
            }
        }

        //Print row
        for(size_t j = 0; j < len_head; j++){
            TreeNode *node = get(truth_table, header[j]);
            printf("%d", node ? node->evaluate(node, truth_table) : 0);
            if(j != len_head -1) {
                printf(" ");
            }
        }
        
        printf("\n");

        free_dict(truth_table);

    }

    free_array(header, len_head);

}

void show_ones(Dict *assignments, char **variables, char **variables_to_show) {

    char **header = concatenate(variables, variables_to_show);
    //counts size of all strings
    size_t len_head = len_array(header);
    // print '#'
    printf("# ");

    //print the header array element wise
    for (size_t i = 0; i < len_head; i++){
        printf("%s", header[i]);
        //space unless last string
        if (i != len_head - 1){
            printf(" ");
        }
    }
    // newline at the end
    printf("\n");

    size_t num_vars = len_array(variables);
    // Total number of rows in the truth table (all possible combinations of 0 and 1)
    unsigned long int rows = 1UL << num_vars;

    for(unsigned long int i = 0; i < rows; i++ ){
        Dict *truth_table = initialize_dict(num_vars * 2); //num_vars * 2 to avoid collisions    

        //truth table for the row
        for(size_t j = 0; j < num_vars; j++){
            int bit = (i >> (num_vars - 1 -j)) & 1;
            TreeNode *valueNode = create_bool(bit);
            insert(truth_table, variables[j], valueNode); //iterates through all possibilities of 0 and 1 for all vars
        }

        //evaluate assignments
        for(size_t j = 0; assignments->vars[j] != NULL; j++ ){
            char *var = assignments->vars[j];
            TreeNode *expr = get(assignments, var);
            if (expr != NULL){
                int result = expr->evaluate(expr, truth_table);
                TreeNode *result_node = create_bool(result);
                insert(truth_table, var, result_node);
            }
        }

        // Check if any of the specified variables are True and print the row if so
        bool print_row = false;
        for(size_t j; variables_to_show[j] != NULL; j++){
            TreeNode *node = get(truth_table, variables_to_show[j]);
            if (node && node->evaluate(node, truth_table)) {
                print_row = true;
                break;
            }
        }

        //Print row
        if (print_row){
            for(size_t j = 0; j < len_head; j++){
                TreeNode *node = get(truth_table, header[j]);
                printf("%d", node ? node->evaluate(node, truth_table) : 0);
                if(j != len_head -1) {
                    printf(" ");
                }
            }
        }
        
        printf("\n");

        free_dict(truth_table);

    }

    free_array(header, len_head);

}

// Read the input file
TokenList* read_file(const char *input_file) {
    FILE* file = fopen(input_file, "r");
    if (file == NULL) {
        perror("error opening file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END); //moves the pointer to the end of the opened file
    long file_size = ftell(file);  // Get the size of the file (in bytes)
    rewind(file);  // Reset the file pointer to the beginning

    char *content = malloc(file_size + 1);
    if (content == NULL) {
        perror("alloc memory error");
        fclose(file);
        return NULL;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';  // Null-terminate the string as a C string
    fclose(file);

    TokenList* token_list = tokenize(content);
    free(content);

    return token_list;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s input_file.txt\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[1];
    TokenList *token_list = read_file(input_file);
    
    if (token_list == NULL) {
        fprintf(stderr, "tokenization failed\n ");
        return EXIT_FAILURE;
    }

    int index = 0;
    //pass the index by reference, allowing variable_declaration to modify it.
    char **variables = variable_declaration(token_list, &index);
    //now i can use the updated index

    if (variables == NULL){
        fprintf(stderr, "declaration failed");
        free_token_list(token_list);
        return EXIT_FAILURE;
    }
    
    Dict *assignments = assignment(token_list, &index, variables);
    
    char **show_vars = malloc(sizeof(char *));
    char **show_ones_vars = malloc(sizeof(char *));
    char **vars_to_show = NULL;

    // Initialize with NULL terminator
    if (show_vars != NULL) {
        show_vars[0] = NULL;
    }
    if (show_ones_vars != NULL) {
        show_ones_vars[0] = NULL;
    }

    size_t len_tok = token_list->size;
    while (index < len_tok){
        if(strcmp(token_list->tokens[index], "show") == 0 && strcmp(token_list->types[index], "keyword") == 0){
            index++;
            vars_to_show = malloc(sizeof(char *));
            if(vars_to_show == NULL) {
                fprintf(stderr, "Memory alloc failed");
                exit(1);
            }
            vars_to_show[0] = NULL;  // Initialize with NULL terminator
            while(index < len_tok && strcmp(token_list->tokens[index], ";") != 0){
                if (strcmp(token_list->types[index], "identifier") == 0){
                    vars_to_show = add(vars_to_show, token_list->tokens[index]);
                }
                index++;
            }
            index++; //skip the semicolon
            show_vars = concatenate(show_vars, vars_to_show);
            free_array(vars_to_show, len_array(vars_to_show));
            vars_to_show = NULL;
        }
        else if(strcmp(token_list->tokens[index], "show_ones") == 0 && strcmp(token_list->types[index], "keyword") == 0){
            index++;
            vars_to_show = malloc(sizeof(char *));
            if(vars_to_show == NULL) {
                fprintf(stderr, "Memory alloc failed");
                exit(1);
            }
            vars_to_show[0] = NULL;  // Initialize with NULL terminator
            while(index < len_tok && strcmp(token_list->tokens[index], ";") != 0){
                if (strcmp(token_list->types[index], "identifier") == 0){
                    vars_to_show = add(vars_to_show, token_list->tokens[index]);
                }
                index++;
            }
            index++;
            show_ones_vars = concatenate(show_ones_vars, vars_to_show);
            free_array(vars_to_show, len_array(vars_to_show));
            vars_to_show = NULL;
        }
        else {
            index++;
        }
    }

    // Display the results
    if(show_vars && show_vars[0] != NULL){
        show(assignments, variables, show_vars);  // Show full truth table
    }
    if(show_ones_vars && show_ones_vars[0] != NULL){
        show_ones(assignments, variables, show_ones_vars);  // Show only when at least one is True
    }

    /* Prints*/

    // Print tokens with their types
    printf("Tokens:\n");
    printf("[");
    for (size_t i = 0; i < token_list->size; i++) {
        printf("%s:%s ", token_list->types[i], token_list->tokens[i]);
    }
    printf("]\n");
    
    printf("Variables:\n");
    printf("[");
    for (size_t i = 0; variables[i] != NULL; i++) {
        printf(" %s ", variables[i]);
    }
    printf("]\n");

    // Print assignments
    printf("Assignments:\n");
    printf("{");
    int first = 1;
    for (unsigned long i = 0; i < assignments->size; i++) {
        if (assignments->entries[i] != NULL) {
            if (!first) {
                printf(", ");
            }
            printf("%s: ", assignments->entries[i]->key);
            TreeNode *node = assignments->entries[i]->node;
            if (node != NULL) {
                printf("%d", node->evaluate(node, assignments));
            } else {
                printf("NULL");
            }
            first = 0;
        }
    }
    printf("}\n");

    // Free allocated memory
    if (show_vars != NULL) {
        free_array(show_vars, len_array(show_vars));
    }

    if (show_ones_vars != NULL) {
        free_array(show_ones_vars, len_array(show_ones_vars));
    }

    if (variables != NULL) {
        free_array(variables, len_array(variables));
    }

    free_token_list(token_list);
    free_dict(assignments);

    return EXIT_SUCCESS; 
}
