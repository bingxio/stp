package com.example;

/**
 * Execute Expression
 */
class Exec {
  /**
   * Return the value after execution
   */
  double evaluate(Expr expr) {
    double result;

    if (expr instanceof BE) {
      result = this.l((BE) expr);
    } else if (expr instanceof GRE) {
      result = this.g((GRE) expr);
    } else if (expr instanceof I) {
      result = this.l((I) expr);
    } else {
      return -1;
    }

    return Double.parseDouble(
        String.format("%.2f", result));
  }

  /**
   * LiteralExpr
   */
  private double l(I expr) {
    return expr.val;
  }

  /**
   * BinaryExpr
   */
  private double l(BE expr) {
    double x = this.evaluate(expr.L); // L
    double y = this.evaluate(expr.R); // R

    System.out.printf("x: %.2f \t y: %.2f \t p: %s\n", x, y, expr.P);

    return switch (expr.P) { // P
      case ADD -> x + y;
      case SUB -> x - y;
      case MUL -> x * y;
      case DIV -> x / y;

      default -> throw new IllegalArgumentException("Unexpected value: " + expr.P);
    };
  }

  /**
   * GroupExpr
   */
  private double g(GRE expr) {
    return this.evaluate(expr.E);
  }
}