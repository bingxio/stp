package com.example;

import java.util.ArrayList;
import java.util.Stack;

/**
 * Parser
 */
class Parser {
  private final ArrayList<Token> tokens;
  private int p = 0;

  Parser(ArrayList<Token> tokens) {
    this.tokens = tokens;
  }

  /**
   * Return the operator of literal token
   */
  Op from(String l) {
    return switch (l) {
      case "+" -> Op.ADD;
      case "-" -> Op.SUB;
      case "*" -> Op.MUL;
      case "/" -> Op.DIV;

      default -> throw new IllegalStateException("Unexpected value: " + l);
    };
  }

  /**
   * Return the precedence of operator
   */
  int getPrecedence(Op op) {
    return switch (op) {
      case ADD, SUB -> 1;
      case MUL, DIV -> 2;
    };
  }

  /**
   * Set precedence and the difference is who calculates L or r first
   */
  Expr setPrecedence(Op op, BE b, Expr expr) {
    if (getPrecedence(op) > getPrecedence(b.P)) {
//      If the priority of OP is higher than that of BE append to right
      b.R = new BE(b.R, op, expr);
    } else {
//      Expression of the same level or priority less than, generate new
      return new BE(b, op, expr);
    }
    return b;
  }

  /**
   * Append expression
   */
  void append(Op op, Stack<Expr> l, Stack<Expr> s) {
    if (op == null) return; // Must have an operator to append
    System.out.printf("-> OP: %s L: %s S: %s\n", op, l, s);

    switch (op) {
//      + - * /
      case ADD, SUB, MUL, DIV -> {
//        Append expression
        if (l.empty() && s.size() > 1) {
          Expr y = s.pop();
          Expr x = s.pop();

//          Two GRE expressions
          if (x instanceof GRE && y instanceof GRE) {
            s.push(new BE(x, op, y));
            return;
          }

          assert x instanceof BE;
//          BE and GRE were added
          Expr expr = this.setPrecedence(op, (BE) x, y);
          s.push(expr);

          return;
        }

//        Operand
        Expr er = l.pop();

        if (l.empty()) {
          Expr e = s.pop(); // To stack of expression

//          If the expression is GRE, BE is generated directly
          if (e instanceof GRE) {
            s.push(new BE(e, op, er));
            return;
          }
//          Set precedence
          Expr expr = this.setPrecedence(op, (BE) e, er);

          s.push(expr);
        } else {
//          Two operands of pop generate BE
          s.push(new BE(l.pop(), op, er));
        }
      }

      default -> throw new IllegalArgumentException("Unexpected value: " + op);
    }
  }

  /**
   * Parse expression and return
   */
  Expr parse() {
    Op p = null; // TOP operator

    Stack<Expr> l = new Stack<>(); // Stack of operand
    Stack<Expr> s = new Stack<>(); // Stack of expression

    while (this.p < tokens.size()) {
      Token token = tokens.get(this.p); // Current token

      switch (token.k) {
        case "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" -> {
//          Push a new literal expression
          l.push(new I(Integer.parseInt(token.k)));
//          Append expression
          append(p, l, s);
//          After appending set the top operator to null
          p = null;
        }

//          Operators
        case "+", "-", "*", "/" -> p = from(token.k);

//          Group Expression
        case "(" -> {
//          Skip left paren symbol
          this.p++;
//          Generate GRE expression after recursive parsing
          s.push(new GRE(this.parse()));

          append(p, l, s);
          p = null;
        }

//          Exit the current parsing when look right paren symbol
        case ")" -> {
          return s.pop();
        }
      }
      this.p++;
//      Dissemble peek of expr stack
      if (!s.empty() && p == null) {
        System.out.println(s.peek());
      }
    }
//      Return the final expression at the top of the stack
    return s.pop();
  }
}