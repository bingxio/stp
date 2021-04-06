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

      default -> {
        throw new IllegalStateException("Unexpected value: " + l);
      }
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
    if (op == null)
      return; // Must have an operator to append
    System.out.printf("-> OP: %s L: %s S: %s\n", op, l, s);

//    Append expression
    if (l.empty()) {

      if (s.size() > 1) {
        Expr y = s.pop();
        Expr x = s.pop();

        if (x instanceof BE) {
//          Precedence
          s.push(this.setPrecedence(op, (BE) x, y));
          return;
        }
//        BE Expr
        s.push(new BE(x, op, y));
        return;
      }

//      Unary Expr
      if (s.size() == 1) {
        s.push(new UE(op, s.pop()));
        return;
      }
    }

//    Operand
    Expr er = l.pop();

    if (l.empty()) {
//      Unary Expr
      if (s.empty()) {
        s.push(new UE(op, er));
        return;
      }

      Expr e = s.pop(); // To stack of expression

//      Binary expression
      if (e instanceof BE) {
//        Set precedence
        s.push(this.setPrecedence(op, (BE) e, er));
      }
//      Group expr
//      Unary
//      Call
//      Get
      else if (
        e instanceof GRE
          || e instanceof UE
          || e instanceof CE
          || e instanceof GE
      ) {
        s.push(new BE(e, op, er)); // To BE Expr
      }
    } else {
//      Two operands of pop generate BE
      s.push(new BE(l.pop(), op, er));
    }
  }

  //  Return the previous of token literal is equal?
  boolean previous(String k) {
    if (
      p - 1 == -1
    ) {
      return false;
    } else {
      return this.tokens.
        get(p - 1).k.equals(k); // Previous
    }
  }

  //  Return expression of L or S stack
  Expr getExpr(Stack<Expr> l, Stack<Expr> s) {
    if (l.empty() &&
      s.empty()
    ) {
      return null;
    } else {
      return l.empty() ?
        s.pop() : l.pop(); // POP it
    }
  }

  /**
   * Parse expression and return
   */
  Expr parse() {
    Op p = null; // TOP operator

    Stack<Expr> l = new Stack<>(); // Stack of operand
    Stack<Expr> s = new Stack<>(); // Stack of expression

    ArrayList<Expr> args = new ArrayList<>(); // Arguments for CE Expr

    while (this.p < tokens.size()) {
      Token token = tokens.get(this.p); // Current token

      switch (token.k) {
//        Literal Expr
        case "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" -> {
//          Push a new literal expression
          l.push(new I(Integer.parseInt(token.k)));
//          Append expression
          append(p, l, s);
//          After appending set the top operator to null
          p = null;
        }

//        Operators
        case "+", "-", "*", "/" -> p = from(token.k);

//        Group expression
        case "(" -> {
//          CallExpr
          boolean callE =
            previous("a")
              || previous("b")
              || previous("c")
              || previous("x")
              || previous("y")
              || previous("z")
              || previous(")");

          this.p++; // Skip left paren symbol

          if (callE) {
            CE ce = new CE(); // Call Expression

            ce.callee = getExpr(l, s);
            ce.arguments = this.parse();

            s.push(ce);
          } else {
//          Generate GRE expression after recursive parsing
            s.push(new GRE(this.parse()));

            append(p, l, s);
            p = null;
          }
        }

//        Exit the current parsing when look right paren symbol
        case ")" -> {
          // Current parsing CE expression.
          if (!args.isEmpty()) {
            ArrayList<Expr> list = new ArrayList<>(args); // Retold
            list.add(getExpr(l, s)); // back argument

            return new ARG(list);
          } else {
            return getExpr(l, s);
          }
        }

//        Arguments split
        case "," -> args.add(getExpr(l, s));

//        Name expression
        case "a", "b", "c", "x", "y", "z" -> {
          l.push(new NE(token));
//          Append expression
          append(p, l, s);
          p = null;
        }

//        Assign expression
        case "=" -> {
          Expr expr = getExpr(l, s);
          this.p++; // Skip assignment symbol

          s.push(new AE(expr, this.parse()));
        }

//        Get expression
        case "." -> {
          Expr expr = getExpr(l, s);
          this.p++; // Skip dot symbol

          s.push(new GE(expr, tokens.get(this.p)));
        }

//        Index expression
        case "[" -> {
          Expr expr = getExpr(l, s);
          this.p++; // Skip left bracket symbol

          s.push(new IE(expr, this.parse()));
        }

//        Exit the current parsing when look right bracket symbol
        case "]" -> {
          return getExpr(l, s);
        }
      }
      this.p++; // For loop

//      Dissemble message
      if (!s.empty() && p == null) {
        System.out.println(s.peek());
      }
    }
//      Return the final expression at the top of the stack
    return getExpr(l, s);
  }
}