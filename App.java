package com.example;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;
import java.util.concurrent.atomic.AtomicReference;

/**
 * Application
 */
class App {
  public static void main(String[] args) {
    testing();
//    command_line();
  }

  /**
   * To command line mode
   */
  static void command_line() {
    Scanner scanner = new Scanner(System.in);

    while (scanner.hasNext()) {
      run(scanner.nextLine());
    }
    scanner.close();
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
//    SimpleExpr
    pm.put("(1 + 2 + (3 + 4)) - 2 * 3", 4.0);
    pm.put("5 / 9 * 8 - 4 * 2 / 7", 3.34);
    pm.put("9 / 7 - (2 - 3 / 2)", 0.79);
    pm.put("(1 * 2) + (3 * 4) - (5 / 6)", 13.17);
    pm.put("4 / 7 * 5 / 9 + 3 / 7 * 5 / 9", 0.56);
    pm.put("1 + 2 3 + 4", 7.00);
//    UnaryExpr
    pm.put("-(2 + 3)", -5.00);
    pm.put("-1 + (-2)", -3.00);
    pm.put("-1 + (-3 + 4)", 0.00);
    pm.put("-5 + 2 + 9", 6.00);
    pm.put("-3 + 3", 0.00);
    pm.put("(-1 + (-2 + 3))", 0.00);
    pm.put("(-7 + (-3)) * (-1) / 7 + 8", 9.43);
    pm.put("(-1) + 2 * (-5 + 5)", -1.00);
//
//    Will throw exceptions:
//
//    NameExpr
    pm.put("a + b * 2", -99.00);
//    CallExpr
    pm.put("x(1, 2, 3)", -99.00);
    pm.put("x(1) + 2", -99.00);
    pm.put("x()()", -99.00);
    pm.put("x(1, 2 + 3, 4 + 5 * 6)", -99.00);
//    AssignExpr
    pm.put("x = 2", -99.00);
    pm.put("(x + y) = 4 + 2 * 9", -99.00);
//    GetExpr
    pm.put("x.y", -99.00);
    pm.put("x.y = 2 + 3 * 4", -99.00);
    pm.put("(x.x + y) + 2 * 3 + (a.b)", -99.00);
//    IndexExpr
    pm.put("x[1]", -99.00);
    pm.put("x()[1 + 2 * 3] = 4", -99.00);
    pm.put("x[0] = x[1]", -99.00);

    AtomicReference<Integer> passed =
        new AtomicReference<>(0);

    pm.forEach((k, v) -> {
      if (run(k) != v) {
        if (v == -99.00) return; // Could not be evaluated

        try {
          throw new Exception("Failed K: " + k + " V: " + v);
        } catch (Exception e) {
          e.printStackTrace();
        }
      } else {
        passed.getAndSet(passed.get() + 1); // PASS
      }
    });

    System.out.printf(
        "\n\tCOUNT: %d | PASS: %d\n", pm.size(), passed.get());
  }
}