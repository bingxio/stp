package com.example;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicReference;

/**
 * Application
 */
class App {
  public static void main(String[] args) {
    testing();
  }

  /**
   * Run a source and return the result
   */
  static double run(String source) {
    System.out.println(source);

    ArrayList<Token> tokens = new ArrayList<>();

    char[] line = source.toCharArray();
    for (char i : line) {
      if (i == ' ') {
        continue;
      }
      tokens.add(new Token(String.valueOf(i)));
    }

//    Parse expression
    Expr expr = new Parser(tokens).parse();
//    Evaluate value
    double result = new Exec().evaluate(expr);
    System.out.println(result);

    return result;
  }

  /**
   * To testing some expression
   */
  static void testing() {
    Map<String, Double> pm = new HashMap<>();

    pm.put("(1 + 2 + (3 + 4)) - 2 * 3", 4.0);
    pm.put("5 / 9 * 8 - 4 * 2 / 7", 3.34);
    pm.put("9 / 7 - (2 - 3 / 2)", 0.79);
    pm.put("(1 * 2) + (3 * 4) - (5 / 6)", 13.17);
    pm.put("4 / 7 * 5 / 9 + 3 / 7 * 5 / 9", 0.56);
    pm.put("1 + 2 3 + 4", 7.00);

    AtomicReference<Integer> passed = new AtomicReference<>(0);

    pm.forEach((k, v) -> {
      if (run(k) != v) {
        try {
          throw new Exception("Failed K: " + k + " V: " + v);
        } catch (Exception e) {
          e.printStackTrace();
        }
      } else {
        passed.getAndSet(passed.get() + 1);
      }
    });

    System.out.printf("\n\tCOUNT: %d | PASS: %d\n", pm.size(), passed.get());
  }
}