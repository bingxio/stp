package com.example;

public class Exec {
  int evaluate(Expr expr) {
    int result;

    if (expr instanceof BE) {
      result = this.l((BE) expr);
    } else if (expr instanceof GE) {
      result = this.g((GE) expr);
    } else if (expr instanceof I) {
      result = this.i((I) expr);
    } else {
      return -1;
    }

    return result;
  }

  private int i(I expr) {
    return expr.val;
  }

  private int l(BE expr) {
    int x = this.evaluate(expr.L);
    int y = this.evaluate(expr.R);

    return switch (expr.P) {
      case ADD -> x + y;
      case SUB -> x - y;
      case MUL -> x * y;
      case DIV -> x / y;
      default -> throw new IllegalArgumentException("Unexpected value: " + expr.P);
    };
  }

  private int g(GE expr) {
    return this.evaluate(expr.E);
  }
}