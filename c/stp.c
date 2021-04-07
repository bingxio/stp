// 
// Copyright (c) 2021 bingxio. All rights reserved.
// 

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * Token structure
 */
struct Token {
  char k; // Literal
};

char *line; // Input expression

/**
 * To command line mode
 */
void command_mode() {
  line = (char *) malloc(128);
  scanf("%[^\n]", line); // Input
  getchar();
}

struct Token *tokens; // Token array
int size = 0;         // Size of tokens

/**
 * Handle the string literal to tokens array
 */
bool lexcial() {
  tokens = (struct Token *) malloc(sizeof(struct Token) * 128); // Tokens
  // Lexcial
  for (int i = 0, j = 0; i < strlen(line); i++) {
    if (line[i] == ' ' || 
        line[i] == '\t') {
          continue;
    }
    // Append
    tokens[j++].k = line[i];
    size++;
  }
  // Empty of line
  if (size == 0) {
    printf("EMPTY\n");
    return false;
  }
  // Dissemble the tokens array
  for (int i = 0; i < size; i++) {
    printf("<'%c'>\t", tokens[i].k);
  }
  putchar('\n'); // A new line
  return true;
}

/**
 * Operator
 */
enum Op {ADD, SUB, MUL, DIV};

/**
 * Return the Op within a character
 */
enum Op from(const char k) {
  switch (k) {
    case '+': return ADD;
    case '-': return SUB;
    case '*': return MUL;
    case '/': return DIV;
  }
}

/**
 * Kind of expressions
 */
enum Expr {LE, BE, GRE, UE, NE, CE, GE, AE, IE};

/**
 * <T>
 */
struct LE {
  char k;
};

/**
 * <L> <P> <R>
 */
struct BE {
  void *L;
  enum Op P;
  void *R;
};

/**
 * <E>
 */
struct GRE {
  void *E;
};

/**
 * <P> <E>
 */
struct UE {
  enum Op P;
  void *E;
};

/**
 * <T>
 */
struct NE {
  char n;
};

/**
 * <E>(<E..>)
 */
struct CE {
  void *C;
  void **A;
};

/**
 * <E>.<T>
 */
struct GE {
  void *E;
  char n;
};

/**
 * <E> = <E>
 */
struct AE {
  void *E;
  void *V;
};

/**
 * <E>[<E>]
 */
struct IE {
  void *E;
  void *D;
};

/**
 * Stack structure
 */
struct Stack {
  void **elements; // Address pointers

  int count;
  int capacity;
};

/**
 * Initialize a stack
 */
void init(struct Stack *sta) {
  sta->elements = NULL;
  sta->count = 0;
  sta->capacity = 0;
}

/**
 * Push the pointer to stack
 */
void push(struct Stack *sta, void *addr) {
  if (sta->count + 1 > sta->capacity) {
    int cap = sta->capacity == 0 
      ? 8 : sta->capacity * 2; // NEW CAP
    sta->capacity = cap;
    sta->elements = 
      (void **) malloc(sizeof(void *) * sta->capacity); // NEW
  }
  sta->elements[sta->count++] = addr; // PUSH
}

/**
 * Pop the back of pointer for elements
 */
void *pop(struct Stack *sta) {
  return sta->elements[sta->count--];
}

/**
 * Return its empty of elements
 */
bool empty(struct Stack *sta) {
  return sta->count == 0;
}

/**
 * Stringer for elements
 */
const char *stringer(struct Stack *sta) {
  if (sta->count == 0)
    return "[]";
}

/**
 * Return the priority of operator
 */
int get_precedence(enum Op op) {
  switch (op) {
    case ADD: case SUB: return 1;
    case MUL: case DIV: return 2;
  }
}

/**
 * Set precedence and the difference is who calculates L or R first
 */
void set_precedence(enum Op *op, struct BE *b, void *expr) {}

/**
 * Append expression
 */
void append(enum Op *op, struct Stack *l, struct Stack *s) {
  if (op == NULL) 
    return;
  printf("-> OP: %s L: %s S: %s\n", op, l, s);
}

int p = 0; // Index

/**
 * Parse the tokens to expression
 */
void parse() {
  enum Op *op; // Operator

  struct Stack l; // L
  struct Stack s; // S

  init(&l);
  init(&s);

  void **ARG; // Arguments

  while (p < size) {
    struct Token tok = tokens[p]; // Now

    switch (tok.k) {
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9': // Literal
        break;
      case 'x': case 'y': case 'z':
      case 'a': case 'b': case 'c': // Literal
        break;
      case '+': case '-': case '*': case '/': // Operator
        break;
      case '(': // Group or Call
        break;
      case ')': // End Group or Call
        break;
      case ',': // Arg
        break;
      case '=': // Set
        break;
      case '.': // Get
        break;
      case '[': // Index
        break;
      case ']': // End Index
        break;

      default:
        printf("Unknown: '%c'\n", tok.k);
        exit(-1);
    }
    p++;
  }
  // To next loop
  size = 0;
}

int main() {
  while (true) {
    command_mode();             // INPUT
    if (!lexcial()) continue;   // LEXCIAL
    parse();                    // PARSE

    free(line);
    free(tokens);               // END
  }
  return 0;
}