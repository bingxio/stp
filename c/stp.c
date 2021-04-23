// 
// Copyright (c) 2021 bingxio, All rights reserved.
// 



#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
 * Token structure
 */
struct Token {
  char k; // Literal
};

char *line; // Input expression

/*
 * To command line mode
 */
void command_mode() {
  line = (char *) malloc(128);
  scanf("%[^\n]", line); // Input
  getchar();
}

struct Token *tokens; // Token array
int size = 0;         // Size of tokens

/*
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

/*
 * Stack structure
 */
struct Stack {
  struct E **elements; // Elements

  int count;
  int capacity;
};

/*
 * Initialize a stack
 */
void init(struct Stack *sta) {
  sta->elements = NULL;
  sta->count = 0;
  sta->capacity = 0;
}

/*
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

/*
 * Push the expr to minal stack
 */
void push_mini(struct Stack *sta, struct E *expr) {
  if (sta->elements == NULL) { /* First alloc memory */
    sta->capacity = 2;
    sta->elements = (struct E **)malloc(sizeof(struct E *) * 2); // NEW
  }
  if (sta->count + 1 > sta->capacity) {
    fprintf(stderr, "ERROR: the l stack has only two sizes");
    exit(-1);
  }
  sta->elements[sta->count] = expr; // PUSH
  sta->count++;
}

/*
 * Return the top of elements
 */
struct E *top(struct Stack *sta) {
  return sta->elements[sta->count];
}

/*
 * Pop a element
 */
struct E *pop(struct Stack *sta) {
  return sta->elements[--sta->count];
}

#define PUSH(S, E)   push(S, E)
#define PUSH_M(S, E) push_mini(S, E)
#define TOP(S)       top(S)
#define POP(S)       pop(S)

/*
 * Return its empty of elements
 */
bool empty(struct Stack *sta) {
  return sta->count == 0;
}

/*
 * Operator
 */
enum Op {ADD, SUB, MUL, DIV, NIl};

/*
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

/*
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

/*
 * Kind of expressions
 */
enum Kind {LE, BE, GRE, UE, NE, CE, GE, AE, IE, ARG};

/*
 * Expression node
 */
struct E {
  enum Kind k;
};

/*
 * <T>
 */
struct LE {
  struct E E;
  char k;
};

/*
 * <L> <P> <R>
 */
struct BE {
  struct E E, *L, *R;
  enum Op P;
};

/*
 * <E>
 */
struct GRE {
  struct E E, *G;
};

/*
 * <P> <E>
 */
struct UE {
  struct E E, *R;
  enum Op P;
};

/*
 * <T>
 */
struct NE {
  struct E E;
  char n;
};

/*
 * <E>(<E..>)
 */
struct CE {
  struct E E, *CALLE, *ARG;
};

/*
 * E..
 */
struct ARG {
  struct E E;
  struct Stack *ARGS; /* Argument stack */
};

/*
 * <E>.<T>
 */
struct GE {
  struct E E, *L;
  char n;
};

/*
 * <E> = <E>
 */
struct AE {
  struct E E, *L, *R;
};

/*
 * <E>[<E>]
 */
struct IE {
  struct E E, *L, *R;
};

/*
 * Dissemble expression
 */
const char *dissemble_expr(struct E *expr) {
  if (expr == NULL) return "NULL"; /* ERROR */

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
    case GRE: { // Group
      struct GRE *gre = (struct GRE *)expr;
      sprintf(str, "<GRE E=%s>", dissemble_expr(gre->G));
    } break;
    case UE: { // Unary
      struct UE *ue = (struct UE *)expr;
      sprintf(str, "<UE P=%s E=%s>", to(ue->P), dissemble_expr(ue->R));
    } break;
    case AE: { // Assign
      struct AE *ae = (struct AE *)expr;
      sprintf(str, "<AE L=%s R=%s>", dissemble_expr(ae->L), dissemble_expr(ae->R));
    } break;
    case GE: { // Get
      struct GE *ge = (struct GE *)expr;
      sprintf(str, "<GE E=%s N='%c'>", dissemble_expr(ge->L), ge->n);
    } break;
    case IE: { // Index
      struct IE *ie = (struct IE *)expr;
      sprintf(str, "<IE L=%s R=%s>", dissemble_expr(ie->L), dissemble_expr(ie->R));
    } break;
    case CE: { // Call
      struct CE *ce = (struct CE *)expr;
      sprintf(str, "<CE E=%s A=%s>", 
        dissemble_expr(ce->CALLE), ce->ARG == NULL ? "NONE" : dissemble_expr(ce->ARG));
    } break;
    case ARG: { // Arguments
      struct ARG *arg = (struct ARG *)expr;

      if (arg->ARGS->count == 0) {
        sprintf(str, "EMPTY");
      } else {
        sprintf(str, "(");
        //
        for (int i = 0; i < arg->ARGS->count; i++) {
          strcat(str, dissemble_expr(*arg->ARGS->elements++));

          if (i + 1 != arg->ARGS->count) {
            strcat(str, ", ");  /* String literal to append */
          }
        }
        strcat(str, ")");
      }
    } break;
    default:
      fprintf(stderr, 
        "ERROR: unknown expr kind of %d\n", expr->k);
  }
  // 
  return str;
}

/*
 * Stringer for elements
 */
void stringer(struct Stack *sta) {
  if (sta->count == 0) {
    printf("EMPTY");
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

/*
 * Return the priority of operator
 */
int get_precedence(enum Op op) {
  switch (op) {
    case ADD: case SUB: return 1;
    case MUL: case DIV: return 2;
  }
}

/*
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

/*
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

  if (empty(l)) {
    // Splicing in S stack
    if (s->count > 1) {
      struct E *y = POP(s);
      struct E *x = POP(s);

      // BE
      if (x->k == BE) {
        PUSH(s, set_precedence(op, (struct BE *)x, y));
        return;
      }

      // New BE
      struct BE *expr = (struct BE *)malloc(sizeof(struct BE));
      struct E E = {.k = BE};
    
      expr->E = E;

      expr->L = x;
      expr->P = *op;
      expr->R = y;

      PUSH(s, expr);
      return;
    }
    // Unary
    if (s->count == 1) {
      struct UE *expr = (struct UE *)malloc(sizeof(struct UE));
      struct E E = {.k = UE};

      expr->E = E;
      expr->P = *op;
      expr->R = POP(s);

      PUSH(s, expr);
      return;
    }
  }

  struct E *le = POP(l); // L

  if (empty(l)) {
    // Unary
    if (empty(s)) {
      struct UE *expr = (struct UE *)malloc(sizeof(struct UE));
      struct E E = {.k = UE};

      expr->E = E;
      expr->P = *op;
      expr->R = le;

      PUSH(s, expr);
      return;
    }

    struct E *se = POP(s); // S

    if (se->k == BE) {
      PUSH(s, set_precedence(op, (struct BE *)se, le)); // Append
    } else if (
      // Group
      // Unary
      // Call
      // Get
      se->k == GRE || se->k == UE || se->k == CE || se->k == GE
    ) {
      struct BE *expr = (struct BE *)malloc(sizeof(struct BE));
      struct E E = {.k = BE}; 
      
      expr->E = E;
      
      expr->L = se;
      expr->P = *op;
      expr->R = le;

      PUSH(s, expr);  // New BE
    }
    // 
  } else {
    struct BE *expr = (struct BE *)malloc(sizeof(struct BE));
    struct E E = {.k = BE}; 
    
    expr->E = E;

    expr->L = POP(l);
    expr->P = *op;
    expr->R = le;

    PUSH(s, expr);  // New BE
  }
}

/*
 * Return the top of expression
 */
struct E *get_expr(struct Stack *l, struct Stack *s) {
  if (empty(l) && empty(s)) {
    return NULL;
  } else {
    return empty(l) ? POP(s) : POP(l); // POP
  }
}

int p = 0; // Index

/*
 * Return the literal in previous token
 */
bool previous(char c) { return tokens[p - 1].k == c; }

/*
 * Parse the tokens to expression
 */
struct E *parse() {
  enum Op op = NIl; // Operator

  struct Stack l; // L
  struct Stack s; // S

  init(&l);
  init(&s);

  struct Stack *args = 
    (struct Stack *)malloc(sizeof(struct Stack)); // Arguments for CE expr
  init(args);

  while (p < size) {
    struct Token tok = tokens[p]; // Now

    switch (tok.k) {
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9': { // Literal
        struct LE *expr = (struct LE *)malloc(sizeof(struct LE));
        struct E E = {.k = LE};
        
        expr->E = E;
        expr->k = tok.k;

        PUSH_M(&l, expr);   /* To L */

        append(&op, &l, &s);
        op = NIl;

      } break;
      case 'x': case 'y': case 'z':
      case 'a': case 'b': case 'c': { // Literal
        struct NE *expr = (struct NE *)malloc(sizeof(struct NE));
        struct E E = {.k = NE};

        expr->E = E;
        expr->n = tok.k;

        PUSH_M(&l, expr);   /* To L */

        append(&op, &l, &s);
        op = NIl;

      } break;
      case '+': case '-': case '*': case '/': // Operator
        op = from(tok.k);
        break;
      case '(': { // Group or Call
        bool callE = 
          previous('a') || previous('b') || previous('c') ||
          previous('x') || previous('y') || previous('z'); /* Is call expr */

        p++; /* Skip left paren symbol */

        // Call
        if (callE) {
          struct CE *expr = (struct CE *)malloc(sizeof(struct CE));
          struct E E = {.k = CE};

          expr->E = E;
          expr->CALLE = get_expr(&l, &s);
          expr->ARG = parse();

          PUSH(&s, expr);   /* Call Expr */
        } else {
          // Group
          struct GRE *expr = (struct GRE *)malloc(sizeof(struct GRE));
          struct E E = {.k = GRE};

          expr->E = E;
          expr->G = parse();  /* Group expression */

          PUSH(&s, expr);     /* To S */

          append(&op, &l, &s);
          op = NIl;
        }
      } break;
      case ')': // End Group or Call
        if (!empty(args)) {
          PUSH(args, get_expr(&l, &s)); // Back argument

          struct ARG *arg = (struct ARG *)malloc(sizeof(struct ARG));
          struct E E = {.k = ARG};

          arg->E = E;
          arg->ARGS = args; /* Args */

          return arg;
        } else {
          return get_expr(&l, &s);
        }
        break;
      case ',': // Arg
        PUSH(args, get_expr(&l, &s));
        break;
      case '=': { // Assign
        struct E *expr = get_expr(&l, &s);
        p++; /* Skip assignment symbol */

        struct AE *ae = (struct AE *)malloc(sizeof(struct AE));
        struct E E = {.k = AE};

        ae->E = E;
        ae->L = expr;
        ae->R = parse();

        PUSH(&s, ae);
      } break;
      case '.': { // Get
        struct E *expr = get_expr(&l, &s);
        p++; /* Skip dot symbol */

        struct GE *ge = (struct GE *)malloc(sizeof(struct GE));
        struct E E = {.k = GE};

        ge->E = E;
        ge->L = expr;
        ge->n = tokens[p].k;

        PUSH(&s, ge);
      } break;
      case '[': { // Index
        struct E *expr = get_expr(&l, &s);
        p++; /* Skip assignment symbol */

        struct IE *ie = (struct IE *)malloc(sizeof(struct IE));
        struct E E = {.k = IE};

        ie->E = E;
        ie->L = expr;
        ie->R = parse();

        PUSH(&s, ie);
      } break;
      case ']': // End Index
        return get_expr(&l, &s);

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

bool eval_E = false; /* Is evaluated has error */

/*
 * Test evaluate
 */
double eval(struct E *expr) {
  double result; /* Value */

  switch (expr->k) {
    case BE: {
      struct BE *be = (struct BE *)expr;

      if (be->P == ADD)
        result = eval(be->L) + eval(be->R);
      if (be->P == SUB)
        result = eval(be->L) - eval(be->R);
      if (be->P == MUL)
        result = eval(be->L) * eval(be->R);
      if (be->P == DIV)
        result = eval(be->L) / eval(be->R);
    } break;
    case GRE:
      result = eval(((struct GRE *)expr)->G);
      break;
    case LE:
      result = (double)((struct LE *)expr)->k - 48;
      break;
    case UE: {
      struct UE *ue = (struct UE *)expr;

      if (ue->P != SUB) {
        fprintf(stderr, "ERROR: unexpected operator '%s'", to(ue->P));
        exit(-1);
      } else {
        return -eval(ue->R);  /* Negative */
      }
    } break;
    default:
      fprintf(stderr, "WARNING: can not evaluate\n");
      eval_E = true;   /* NIL CODE */
  }
  return result;
}

int main() {
  while (true) {
    command_mode();             // INPUT
    if (!lexcial()) continue;   // LEXCIAL

    struct E *expr = parse();
    printf("%s\n", dissemble_expr(expr));   /* DIS */
    
    double r = eval(expr);
    if (!eval_E) printf("%lf\n", r);     /* EVAL */

    free(line);
    free(tokens);               // END

    eval_E = false;
  }
  return 0;
}
