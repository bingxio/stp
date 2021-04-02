package com.example;

import java.util.ArrayList;
import java.util.Scanner;
import java.util.Stack;

class Token {
  String k;

  Token(String k) {
    this.k = k;
  }

  @Override
  public String toString() {
    return "<Token" + " K='" + k + '\'' + '>';
  }
}

class App {
  public static void main(String[] args) {
    ArrayList<Token> tokens = new ArrayList<>();

    Scanner scanner = new Scanner(System.in);
    System.out.print("INPUT: ");

    char[] line = scanner.nextLine().toCharArray();
    for (char i : line) {
      if (i == ' ') {
        continue;
      }
      tokens.add(new Token(String.valueOf(i)));
    }
    scanner.close();

    Expr expr = new Parser(tokens).parse();
    System.out.println(expr);

    int result = new Exec().evaluate(expr);
    System.out.println(result);
  }
}

enum Op {
  ADD, SUB, MUL, DIV, RP;

  @Override
  public String toString() {
    return switch (this) {
      case ADD -> "+";
      case SUB -> "-";
      case MUL -> "*";
      case DIV -> "/";
      case RP -> ")";
    };
  }
}

abstract class Expr {
}

class I extends Expr {
  int val;

  I(int val) {
    this.val = val;
  }

  @Override
  public String toString() {
    return val + "";
  }
}

class BE extends Expr {
  Expr L;
  Op P;
  Expr R;

  BE(Expr l, Op p, Expr r) {
    this.L = l;
    this.P = p;
    this.R = r;
  }

  @Override
  public String toString() {
    return "<BE" + " L=" + L + " P=" + P + " R=" + R + '>';
  }
}

class GE extends Expr {
  Expr E;

  GE(Expr e) {
    this.E = e;
  }

  @Override
  public String toString() {
    return "<GE E=" + E + ">";
  }
}

class Parser {
  private final ArrayList<Token> tokens;
  private int p = 0;

  Parser(ArrayList<Token> tokens) {
    this.tokens = tokens;
  }

  Op from(String l) {
    return switch (l) {
      case "+" -> Op.ADD;
      case "-" -> Op.SUB;
      case "*" -> Op.MUL;
      case "/" -> Op.DIV;

      default -> throw new IllegalStateException("Unexpected value: " + l);
    };
  }

  int getPrecedence(Op op) {
    return switch (op) {
      case ADD, SUB -> 1;
      case MUL, DIV -> 2;

      default -> throw new IllegalArgumentException("Unexpected value: " + op);
    };
  }

  Expr setPrecedence(Op op, BE b, Expr expr) {
    if (getPrecedence(op) > getPrecedence(b.P)) {
      b.R = new BE(b.R, op, expr);
    } else {
      b.L = new BE(b.L, b.P, b.R);
      b.P = op;
      b.R = expr;
    }
    return b;
  }

  void append(Op op, Stack<Expr> l, Stack<Expr> s) {
    if (op != null) {
      switch (op) {
        case ADD, SUB, MUL, DIV -> {
          if (l.empty() && s.size() > 1) {
            Expr y = s.pop();
            Expr x = s.pop();

            if (x instanceof GE && y instanceof GE) {
              s.push(new BE(x, op, y));
              return;
            }

            assert x instanceof BE;
            Expr expr = this.setPrecedence(op, (BE) x, y);
            s.push(expr);

            return;
          }

          Expr er = l.pop();

          if (l.empty()) {
            Expr e = s.pop();

            if (e instanceof GE) {
              s.push(new BE(er, op, e));
              return;
            }

            Expr expr = this.setPrecedence(op, (BE) e, er);

            s.push(expr);
          } else {
            s.push(new BE(l.pop(), op, er));
          }
        }
        default -> throw new IllegalArgumentException("Unexpected value: " + op);
      }
      System.out.println(s.peek());
    }
  }

  Expr parse() {
    Op top = null;

    Stack<Expr> l = new Stack<>();
    Stack<Expr> s = new Stack<>();

    while (p < tokens.size()) {
      Token token = tokens.get(p);

      switch (token.k) {

        case "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" -> {
          l.push(new I(Integer.parseInt(token.k)));

          append(top, l, s);
          top = null;
        }

        case "+", "-", "*", "/" -> top = from(token.k);

        case "(" -> {
          p++;

          s.push(new GE(this.parse()));

          append(top, l, s);
          top = null;
        }

        case ")" -> {
          return s.pop();
        }
      }
      p++;
    }
    return s.pop();
  }
}