#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "ast.hpp"
#include "local.hpp"
//#include "symtable.hpp"
#include"hashmap.h"
// @Nocheckin
#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <set>
#include <algorithm>

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
	hmap::hashmap<Value*,const char*> fdeclenv;
} Closure;

// Values
Value* MakeEmptyVal();
Value* MakeIntegerVal(L_INTEGER i);
Value* MakeStringVal(L_STRING s);
Value* MakeFloatVal(double f);
Value* MakeClosureVal(const char* f,
					  Buffer x,
	                  Expression* fbody,
					  hmap::hashmap<Value *,const char*> fdeclenv);

// Interpreting
Value* eval(Expression* Expr, hmap::hashmap<Value*,const char*>* env);
Value* lookup(const char* name, hmap::hashmap<Value*,const char*>* env);
std::set<const char*> freevars(Expression* e);
bool closedin(std::vector<const char*> vs, Expression* e);

// Utility
std::string toString(Expression* Expr);


#endif
