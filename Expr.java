package com.example;

/**
 * Expression Node
 */
abstract class Expr {
}

/**
 * LiteralExpr -> number | string | character
 */
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

/**
 * BinaryExpr -> <Expr L> <OP> <Expr R>
 */
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
    return "<B" + " L=" + L + " P=" + P + " R=" + R + '>';
  }
}

/**
 * GroupExpr -> (Expr E)
 */
class GRE extends Expr {
  Expr E;

  GRE(Expr e) {
    this.E = e;
  }

  @Override
  public String toString() {
    return "<GR E=" + E + ">";
  }
}