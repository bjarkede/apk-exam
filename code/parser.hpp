#ifndef PARSER_HPP
#define PARSER_HPP

#include "ast.hpp"
#include "local.hpp"
#include "lexer.hpp"

#define MAXSIZE 100

#include <stack>
#include <list>
#include <map>

// Test Parsing
Exp* ParseExpression(LexerState* ls);
Exp* ParseExpressionTernary(LexerState* ls);
Exp* ParseParenthesisedExpression(LexerState* ls);
Exp* ParseExpressionOr(LexerState* ls);
Exp* ParseExpressionOperand(LexerState* ls);
Exp* ParseExpressionAnd(LexerState* ls);
Exp* ParseExpressionCompare(LexerState* ls);
Exp* ParseExpressionAdd(LexerState* ls);
Exp* ParseExpressionMul(LexerState* ls);
Exp* ParseExpressionUnary(LexerState* ls);
Exp* ParseExpressionBase(LexerState* ls);

Exp* ParseExpressionCompound(LexerState* ls);

// Helpers in general.
void check_match(LexerState* ls, int what, int who, int where);
bool test_next(LexerState* ls, int c);
bool test(LexerState* ls, int c);
bool check(LexerState* ls, int c);
bool check_next(LexerState* ls, int c);

BinaryOpType GetBinaryOperator(int op);
UnOpType GetUnaryOperator(int op);
bool IsCompOperator(LexerState* ls);
bool IsAddOperator(LexerState* ls);
bool IsMulOperator(LexerState* ls);
bool IsUnaryOperator(LexerState* ls);

#endif