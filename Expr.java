package com.example;

import java.util.ArrayList;

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

/**
 * UnaryExpr -> <OP> <Expr>
 */
class UE extends Expr {
  Op op;
  Expr expr;

  UE(Op op, Expr expr) {
    this.op = op;
    this.expr = expr;
  }

  @Override
  public String toString() {
    return "<UE" +
        " P=" + op +
        " E=" + expr +
        '>';
  }
}

/**
 * NameExpr -> <T>
 */
class NE extends Expr {
  Token name;

  NE(Token name) {
    this.name = name;
  }

  @Override
  public String toString() {
    return "<NE N=" + name.k + ">";
  }
}

/**
 * CallExpr -> <Expr>(<Expr>..)
 */
class CE extends Expr {
  Expr callee;
  Expr arguments;

  CE() {
  }

  @Override
  public String toString() {
    return "<CE" +
        " C=" + callee +
        " A=" + arguments +
        '>';
  }
}

/**
 * Arguments
 */
class ARG extends Expr {
  ArrayList<Expr> exprArrayList;

  ARG(ArrayList<Expr> exprArrayList) {
    this.exprArrayList = exprArrayList;
  }

  @Override
  public String toString() {
    return "<ARG" +
        " E=" + exprArrayList +
        '>';
  }
}

/**
 * GetExpr -> <Expr>.<T>
 */
class GE extends Expr {
  Expr expr;
  Token name;

  GE(Expr expr, Token name) {
    this.expr = expr;
    this.name = name;
  }

  @Override
  public String toString() {
    return "<GE" +
        " E=" + expr +
        " N=" + name.k +
        '>';
  }
}

/**
 * AssignExpr -> <Expr> = <Expr>
 */
class AE extends Expr {
  Expr expr;
  Expr value;

  AE(Expr expr, Expr value) {
    this.expr = expr;
    this.value = value;
  }

  @Override
  public String toString() {
    return "<AE" +
        " E=" + expr +
        " V=" + value +
        '>';
  }
}

/**
 * IndexExpr -> <Expr>[Expr]
 */
class IE extends Expr {
  Expr expr;
  Expr dex;

  IE(Expr expr, Expr dex) {
    this.expr = expr;
    this.dex = dex;
  }

  @Override
  public String toString() {
    return "<IE" +
        " E=" + expr +
        " D=" + dex +
        '>';
  }
}