package com.example;

/**
 * Token
 */
class Token {
  String k; // Literal

  Token(String k) {
    this.k = k;
  }

  @Override
  public String toString() {
    return "<Token" + " K='" + k + '\'' + '>';
  }
}