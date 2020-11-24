#ifndef AST_HPP
#define AST_HPP

#include "local.hpp"

enum BinaryOpType {
	// Arithmetic
	Add, Sub, Mul, Mod, Pow,
	Div, IDiv,
	// Comparison
	Equal, Less,
	// Logical
	And, Or,
	// Others
	MIN, MAX,

	NoBinOp,
	OP_COUNT
};

enum UnOpType {
	Not,
	Minus,
	BNot,
	Len,
	NoUnOp
};

enum ExpressionType
{
	E_Integer,
	E_String,
	E_Variable,
	E_LetBinding,
	E_LetFun,
	E_IfThenElse,
	E_BinOp,
	E_Call,
	E_UnOp,
	E_Paren,
	E_Print,
	E_NoType,
	E_Count
};

typedef struct Expression {
	ExpressionType expType;
} Exp;

typedef struct Integer : public Expression {
	unsigned long long value;
} Integer;

typedef struct Variable : public Expression {
	const char* name;
} Var;

typedef struct BinOp : public Expression {
	BinaryOpType opType;
	Expression* left;
	Expression* right;
} BinOp;

typedef struct UnOp : public Expression {
	UnOpType opType;
	Expression* expr;
} UnOp;

typedef struct Paren : public Expression {
	Expression* expr;
} Paren;

typedef struct Let : public Expression {
	Expression* variable;
	Expression* binding;
	Buffer bindings;
	Expression* expr;
} Let;

typedef struct LetFun : public Expression {
	const char* f;
	Buffer x;
	Expression* fbody;
	Expression* letbody;
} LetFun;

typedef struct Call : public Expression {
	Expression* eFun;
	Buffer args;
} Call;

typedef struct IfThenElse : public Expression {
	Expression* ifexp;
	Expression* thenexp;
	Expression* elseexp;
} If;

typedef struct Print : public Expression {
	Expression* expr;
} Print;

// Empty
Expression* EmptyExp();

// Literals
Expression* IntegerExp(L_INTEGER i);
Expression* VariableExp(L_STRING* val);
Expression* ParenExp(Expression* expr);

// Binary Expressions
Expression* BinaryExp(BinaryOpType op, Expression* left, Expression* right);

// Unary
Expression* UnaryExp(UnOpType op, Expression* expr);

// If
Expression* IfThenElseExp(Expression* ifexp, Expression* thenexp, Expression* elseexp);

// Let-bindings
Expression* LetExp(Expression* var, Expression* binding, Expression* expr);
Expression* LetFunExp(const char* f, Buffer x, Expression* fbody, Expression* letbody);
Expression* CallExp(Expression* eFun, Buffer args);

// Helpers
Expression* PrintExp(Expression* expr);

#endif