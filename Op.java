package com.example;

/**
 * Operator
 */
enum Op {
  ADD, SUB, MUL, DIV;

  @Override
  public String toString() {
    return switch (this) {
      case ADD -> "+";
      case SUB -> "-";
      case MUL -> "*";
      case DIV -> "/";
    };
  }
}