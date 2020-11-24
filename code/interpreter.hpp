#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "ast.hpp"
#include "local.hpp"
#include "symtable.hpp"

// @Nocheckin
#include <string>
#include <sstream>
#include <iostream>
#include <map>

enum ValueType {
	V_Integer,
	V_Float,
	V_Variable,
	V_Bool,
	V_String,
	V_Closure,
	V_NoType,
	V_Count
};

typedef struct Value {
	ValueType vType;
} Value;

typedef struct IntVal : public Value {
	L_INTEGER i;
} IntVal;

typedef struct FloatVal : public Value {
	double f;
};

typedef struct BoolVal : public Value {
	L_BOOL b;
} BoolVal;

typedef struct StringVal : public Value {
	L_STRING s;
} StringVal;

typedef struct Closure : public Value {
	const char* f;
	Buffer x;
	Expression* fbody;
	symtable<Value*> fdeclenv;
} Closure;

// Values
Value* MakeEmptyVal();
Value* MakeIntegerVal(L_INTEGER i);
Value* MakeStringVal(L_STRING s);
Value* MakeFloatVal(double f);
Value* MakeClosureVal(const char* f,
					  Buffer x,
	                  Expression* fbody,
					  symtable<Value*> fdeclenv);

// Interpreting
Value* eval(Expression* Expr, symtable<Value*>* env);
Value* lookup(const char* name, symtable<Value*>* env);

// Utility
std::string toString(Expression* Expr);


#endif