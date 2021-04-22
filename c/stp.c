// 
// Copyright (c) 2021 bingxio, All rights reserved.
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
    if (line[i] == ' ' 
     || line[i] == '\t') {
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
  // 
  return true;
}

/**
 * Operator
 */
enum Op {ADD, SUB, MUL, DIV, NIl};

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
 * Return the operator 's literal
 */
const char *to(enum Op op) {
  switch (op) {
    case ADD: return "+";
    case SUB: return "-";
    case MUL: return "*";
    case DIV: return "/";

    case NIl: return "NIL";
  }
}

/**
 * Kind of expressions
 */
enum Kind {LE, BE, GRE, UE, NE, CE, GE, AE, IE};

/**
 * Expression node
 */
struct E {
  enum Kind k;
};

/**
 * <T>
 */
struct LE {
  struct E E;
  char k;
};

/**
 * <L> <P> <R>
 */
struct BE {
  struct E E, *L, *R;
  enum Op P;
};

/**
 * <E>
 */
struct GRE {
  struct E E, *G;
};

/**
 * <P> <E>
 */
struct UE {
  struct E E, *R;
  enum Op P;
};

/**
 * <T>
 */
struct NE {
  struct E E;
  char n;
};

/**
 * <E>(<E..>)
 */
struct CE {
  struct E E, *CALLE, *ARG;
};

/**
 * <E>.<T>
 */
struct GE {
  struct E E, *L;
  char n;
};

/**
 * <E> = <E>
 */
struct AE {
  struct E E, *L, *R;
};

/**
 * <E>[<E>]
 */
struct IE {
  struct E E, *L, *R;
};

/**
 * Stack structure
 */
struct Stack {
  struct E **elements; // Elements

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
 * Push the expr into stack
 */
void push(struct Stack *sta, struct E *expr) {
  if (sta->count + 1 > sta->capacity) {
    int cap = sta->capacity == 0 
      ? 8 : sta->capacity * 2; // NEW CAP
    sta->capacity = cap;
    sta->elements = 
      (struct E **) malloc(sizeof(struct E *) * sta->capacity); // NEW
  }
  sta->elements[sta->count] = expr; // PUSH
  sta->count++;
}

/**
 * Return the top of elements
 */
struct E *top(struct Stack *sta) {
  return sta->elements[sta->count];
}

/**
 * Pop a element
 */
struct E *pop(struct Stack *sta) {
  return sta->elements[--sta->count];
}

#define PUSH(S, E)  push(S, E)
#define TOP(S)      top(S)
#define POP(S)      pop(S)

/**
 * Return its empty of elements
 */
bool empty(struct Stack *sta) {
  return sta->count == 0;
}

/**
 * Dissemble expression
 */
const char *dissemble_expr(struct E *expr) {
  char *str = (char *)malloc(sizeof(char) * 128); /* Each expression */

  switch (expr->k) {
    case LE:  // Literal
      sprintf(str, "<LE '%c'>", ((struct LE *)expr)->k);
      break;
    case NE:  // Name
      sprintf(str, "<NE '%c'>", ((struct NE *)expr)->n);
      break;
    case BE: { // Binary
      struct BE *be = (struct BE *)expr;
      sprintf(str, "<BE L=%s P=%s R=%s>", 
        dissemble_expr(be->L), to(be->P), dissemble_expr(be->R));
    } break;
    default:
      fprintf(stderr, 
        "ERROR: unknown expr kind of %d\n", expr->k);
  }
  // 
  return str;
}

/**
 * Stringer for elements
 */
void stringer(struct Stack *sta) {
  if (sta->count == 0) {
    printf("[]");
    return;
  }
  printf("[");
  // DISSEMBLE
  for (int i = 0; i < sta->count; i++) {
    printf("%s", dissemble_expr(sta->elements[i]));
    if (i + 1 != sta->count) {
      printf(", ");
    }
  }
  printf("]");
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
struct E *set_precedence(enum Op *op, struct BE *b, struct E *expr) {
  struct BE *new = (struct BE *)malloc(sizeof(struct BE));
  struct E E = {.k = BE};   /* New BE Expr */

  new->L = b->R;
  new->P = *op;
  new->R = expr;

  new->E = E;

  // Append to the right
  if (get_precedence(*op) > get_precedence(b->P)) {
    b->R = new;
    return b;
  }
  new->L = b;
  return new;
}

/**
 * Append expression
 */
void append(enum Op *op, struct Stack *l, struct Stack *s) {
  if (*op == NIl) {
    return; // Skip
  } else {
    printf("%20s ", to(*op));

    printf(" L: "); stringer(l);  // STRINGER
    printf(" S: "); stringer(s);  // STRINGER
    printf("\n");
  }

  // if (empty(l)) {}
  struct E *er = POP(l);

  if (empty(l)) {
    struct E *sr = POP(s);

    if (sr->k == BE) {
      PUSH(s, set_precedence(op, (struct BE *)sr, er)); // Append
    }
    // 
  } else {
    struct BE *expr = (struct BE *)malloc(sizeof(struct BE));
    struct E E = {.k = BE};

    expr->L = POP(l);
    expr->P = *op;
    expr->R = er;

    expr->E = E;

    PUSH(s, expr);  // New BE
  }
}

/**
 * Return the top of expression
 */
struct E *get_expr(struct Stack *l, struct Stack *s) {
  if (empty(l) && empty(s)) {
    fprintf(stderr, "ERROR: L and S are empty!!\n");
    exit(-1);
  }
  return empty(l) ? POP(s) : POP(l); // POP
}

int p = 0; // Index

/**
 * Parse the tokens to expression
 */
struct E *parse() {
  enum Op op = NIl; // Operator

  struct Stack l; // L
  struct Stack s; // S

  init(&l);
  init(&s);

  while (p < size) {
    struct Token tok = tokens[p]; // Now

    switch (tok.k) {
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9': { // Literal
        struct LE *expr = (struct LE *)malloc(sizeof(struct LE));
        struct E E = {.k = LE};
        
        expr->E = E;
        expr->k = tok.k;

        PUSH(&l, expr);

        append(&op, &l, &s);
        op = NIl;

      } break;
      case 'x': case 'y': case 'z':
      case 'a': case 'b': case 'c': { // Literal
        struct NE *expr = (struct NE *)malloc(sizeof(struct NE));
        struct E E = {.k = NE};

        expr->E = E;
        expr->n = tok.k;

        PUSH(&l, expr);

        append(&op, &l, &s);
        op = NIl;

      } break;
      case '+': case '-': case '*': case '/': // Operator
        op = from(tok.k);
        break;
      case '(': // Group or Call
        break;
      case ')': // End Group or Call
        break;
      case ',': // Arg
        break;
      case '=': // Assign
        break;
      case '.': // Get
        break;
      case '[': // Index
        break;
      case ']': // End Index
        break;

      default:
        fprintf(stderr,
          "ERROR: unknown character '%c'\n", tok.k);
        exit(-1);
    }
    p++;
  }
  // To next loop
  size = 0;
  p = 0;

  return get_expr(&l, &s); // Return expr
}

int main() {
  while (true) {
    command_mode();             // INPUT
    if (!lexcial()) continue;   // LEXCIAL

    struct E *expr = parse();
    printf("%s\n", dissemble_expr(expr));

    free(line);
    free(tokens);               // END
  }
  return 0;
}
