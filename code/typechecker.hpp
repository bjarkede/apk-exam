#ifndef TYPECHECKER_HPP
#define TYPECHECKER_HPP

#include "local.hpp"
#include "ast.hpp"
#include "interpreter.hpp"
#include <map>
#include <iostream>

enum TypType
{
	T_Integer,
	T_String,
	T_Variable,
	T_Bool,
	T_Count
};

typedef struct Type {
	TypType t;
} Type;

typedef struct IntType : public Type {
} IntType;

typedef struct FloatType : public Type {
} FloatType;

typedef struct BoolType : public Type {
} BoolType;

typedef struct StringType : public Type {
} StringType;

std::map<const char*, Type*> makeInitialTypeEnv(Expression* e);
Type* typeCheck(Expression* e, std::map<const char*, Type*> tenv);

// Constructors
Type* IntegerTyp();

#endif