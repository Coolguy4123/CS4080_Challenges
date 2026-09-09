package com.craftinginterpreters.lox;

class RpnPrinter implements Expr.Visitor<String> {

    String print(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public String visitBinaryExpr(Expr.Binary expr) { // Example: 2 + 3
        return expr.left.accept(this) + " " + expr.right.accept(this) + " " + expr.operator.lexeme;
    }

    @Override
    public String visitGroupingExpr(Expr.Grouping expr) { // Example: (2 + 3)
        return expr.expression.accept(this);
    }

    @Override
    public String visitLiteralExpr(Expr.Literal expr) { // Example: 10
        return expr.value.toString();
    }

    @Override
    public String visitUnaryExpr(Expr.Unary expr) { // Example: -1
        String operator = expr.operator.lexeme;

        // Cannot use '-' in unary, must be '~'
        if (expr.operator.type == TokenType.MINUS) {
            operator = "~";
        }

        return expr.right.accept(this) + " " + operator;
    }

    // 1 + 2
    public static void main(String[] args) {
        Expr expression = new Expr.Binary(
            new Expr.Literal(1),
            new Token(TokenType.PLUS, "+", null, 1),
            new Expr.Literal(2)
        );
        System.out.println(new RpnPrinter().print(expression));
    }
}