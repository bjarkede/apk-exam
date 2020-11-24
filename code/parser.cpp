#include "parser.hpp"
#include "ast.hpp"

bool test_next(LexerState* ls, int c) {
	if (ls->t.token == c) {
		ProcessNextToken(ls);
		return true;
	}
	else {
		return false;
	}
}

bool test(LexerState* ls, int c) {
	if (ls->t.token == c) {
		return true;
	}
	else {
		return false;
	}
}

void check_match(LexerState* ls, int what, int who, int where) {
	if (!test_next(ls, what)) {
		if (where == ls->lineNumber) {
			FatalError("SyntaxError: Failed when checking match %d", what);
		}
		else {
			FatalError("SyntaxError: %c expected (to close to %c at line %d)", (char)what, (char)who, where);
		}
	}
}

bool check(LexerState* ls, int c) {
	if (ls->t.token != c)
		return false;
	return true;
}

bool check_next(LexerState* ls, int c) {
	if (check(ls, c)) {
		ProcessNextToken(ls);
		return true;
	}
	return false;
}

Exp* ParseExpression(LexerState* ls) {
	return ParseExpressionTernary(ls);
}

bool IsUnaryOperator(LexerState* ls) {
	return test(ls, '+') || test(ls, '-') || test(ls, '*') || test(ls, '&');
}

Exp* ParseExpressionBase(LexerState* ls) {
	Exp* expr = ParseExpressionOperand(ls);
	int len = 0;
	while (test(ls, '(') || test(ls, '[') || test(ls, '.')) {
		if (check_next(ls, '(')) {
			Buffer args;
			AllocBuffer(args, sizeof(Expression*) * 5);
			if (!test(ls, ')')) {
				((Exp**)args.buffer)[args.writeIndex++] = ParseExpression(ls);
				while (check_next(ls, ',')) {
					((Exp**)args.buffer)[args.writeIndex++] = ParseExpression(ls);
				}
			}
			check_match(ls, ')', '(', ls->t.line);
			expr = CallExp(expr, args);
		}
		else {

		}
	}
	return expr;
}

Exp* ParseExpressionUnary(LexerState* ls) {
	if (IsUnaryOperator(ls)) {
		UnOpType op = GetUnaryOperator(ls->t.token);
		ProcessNextToken(ls);
		return UnaryExp(op, ParseExpressionUnary(ls));
	}
	else {
		return ParseExpressionBase(ls);
	}
}

bool IsMulOperator(LexerState* ls) {
	return test(ls, '*') || test(ls, '/') || test(ls, '%') || test(ls, '&');
}

Exp* ParseExpressionMul(LexerState* ls) {
	Exp* expr = ParseExpressionUnary(ls);
	while (IsMulOperator(ls)) {
		BinaryOpType op = GetBinaryOperator(ls->t.token);
		ProcessNextToken(ls);
		expr = BinaryExp(op, expr, ParseExpressionUnary(ls));
	}

	return expr;
}

bool IsAddOperator(LexerState* ls) {
	return test(ls, '+') || test(ls, '-') || test(ls, '|') || test(ls, '^');
}

Exp* ParseExpressionAdd(LexerState* ls) {
	Exp* expr = ParseExpressionMul(ls);
	while (IsAddOperator(ls)) {
		BinaryOpType op = GetBinaryOperator(ls->t.token);
		ProcessNextToken(ls);
		expr = BinaryExp(op, expr, ParseExpressionMul(ls));
	}
	return expr;
}

bool IsCompOperator(LexerState* ls) {
	return test(ls, '<') || test(ls, '>') || test(ls, TK_EQ) || test(ls, TK_LE);
}

Exp* ParseExpressionCompare(LexerState* ls) {
	Exp* expr = ParseExpressionAdd(ls);
	while (IsCompOperator(ls)) {
		BinaryOpType op = GetBinaryOperator(ls->t.token);
		ProcessNextToken(ls);
		return BinaryExp(op, expr, ParseExpressionAdd(ls));
	}
	return expr;
}

Exp* ParseExpressionAnd(LexerState* ls) {
	Exp* expr = ParseExpressionCompare(ls);
	while (check_next(ls, TK_AND)) {
		expr = BinaryExp(And, expr, ParseExpressionCompare(ls));
	}
	return expr;
}

Exp* ParseExpressionOr(LexerState* ls) {
	Exp* expr = ParseExpressionAnd(ls);
	while (check_next(ls, TK_OR)) {
		expr = BinaryExp(Or, expr, ParseExpressionAnd(ls));
	}
	return expr;
}

Exp* ParseExpressionTernary(LexerState* ls) {
	Exp* expr = ParseExpressionOr(ls);
	
	// @TODO:

	return expr;
}

Exp* ParseParenthesisedExpression(LexerState* ls) {
	check(ls, '(');
	Exp* expr = ParseExpression(ls);
	check(ls, ')');
	return expr;
}

UnOpType GetUnaryOperator(int op) {
	switch (op) {
	case '!': return Not;
	case '-': return Minus;
	case '~': return BNot;
	case '#': return Len;
	default:
		return NoUnOp;
	}
}

BinaryOpType GetBinaryOperator(int op) {
	switch (op) {
	case '+': return Add;
	case '-': return Sub;
	case '*': return Mul;
	case '%': return Mod;
	case '^': return Pow;
	case '/': return Div;
	case TK_EQ: return Equal;
	case '<': return Less;
	default:
		return NoBinOp;
	}
}

Exp* ParseExpressionCompound(LexerState* ls) {
	check_match(ls, '{', '{', ls->t.line);
	check_match(ls, '}', '{', ls->t.line);
	return NULL;
}

Exp* ParseExpressionOperand(LexerState* ls) {
	switch (ls->t.token) {
	case TK_INT: { unsigned long long val = ls->t.semInfo.i; ProcessNextToken(ls); return IntegerExp(val); } break;
	case TK_VAR: { L_STRING* val = ls->t.semInfo.s; ProcessNextToken(ls); return VariableExp(val); } break;
	case '(': {
		check_match(ls, '(', '(', ls->t.line);
		Exp* expr = ParseExpression(ls);
		check_match(ls, ')', ')', ls->t.line);
		return ParenExp(expr);
	} break;
	case TK_IF: {
		check_match(ls, TK_IF, TK_IF, ls->t.line);
		Exp* e1 = ParseExpression(ls);
		check_match(ls, TK_THEN, TK_THEN, ls->t.line);
		Exp* e2 = ParseExpression(ls);
		check_match(ls, TK_ELSE, TK_ELSE, ls->t.line);
		return IfThenElseExp(e1, e2, ParseExpression(ls));
	} break;
	case TK_PRINT: {
		ProcessNextToken(ls);
		check_match(ls, '(', '(', ls->t.line);
		Exp* expr = ParseExpression(ls);
		check_match(ls, ')', ')', ls->t.line);
		return PrintExp(expr);
	} break;
	case TK_LET: {
		// Expr -> let VAR = Expr in Expr end 
		check_match(ls, TK_LET, TK_LET, ls->t.line);
		Exp* e = ParseExpression(ls);
		switch (e->expType) {
		case E_Variable: {
			check_match(ls, '=', '=', ls->t.line);
			Exp* binding = ParseExpression(ls);
			Exp* expr = NULL;
			if (check(ls, TK_IN)) {
				check_match(ls, TK_IN, TK_IN, ls->t.line);
				expr = ParseExpression(ls);
			}
			check_match(ls, TK_END, TK_END, ls->t.line);
			return LetExp(e, binding, expr);
		} break;
		case E_Call: {
			check_match(ls, '=', '=', ls->t.line);
			Exp* fbody = ParseExpression(ls);
			Exp* letbody = NULL;
			if (check(ls, TK_IN)) {
				check_match(ls, TK_IN, TK_IN, ls->t.line);
				letbody = ParseExpression(ls);
			}
			check_match(ls, TK_END, TK_END, ls->t.line);
			return LetFunExp(((Var*)((Call*)e)->eFun)->name, ((Call*)e)->args, fbody, letbody);
		} break;
	
		default:
			FatalError("SyntaxError: Invalid input: %c, Line: %d[%d ].", (char)ls->t.token, ls->t.line, ls->t.col);
		}
	} break;
	default:
		FatalError("SyntaxError: Invalid input: %c, Line: %d[%d ].", (char)ls->t.token, ls->t.line, ls->t.col);
	}
}
