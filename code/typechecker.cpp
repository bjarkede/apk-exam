#include "typechecker.hpp"
#include "typelist.hpp"

#include <functional>

std::map<const char*, Type*> makeInitialTypeEnv(Expression* program) {
	std::map<const char*, Type*> tenv;
	std::set<const char*> fv = freevars(program);
	for (auto& element : fv) {
		tenv.insert(std::make_pair(element, IntegerTyp()));
	}
	return tenv;
}

Type* typeCheck(Expression* e, std::map<const char*, Type*> tenv) {
	
	// Define a type-list of all the different types.
	typedef TL::make_typelist<IntType, StringType, BoolType> TestTypes;
	
	switch (e->expType) {
	case E_Integer: {
		return IntegerTyp();
	}
	case E_BinOp: 
	{
		auto lefttype = typeCheck(((BinOp*)e)->left, tenv);
		auto righttype = typeCheck(((BinOp*)e)->right, tenv);

		switch (((BinOp*)e)->opType) {
		case Add: 
		{
		
		} break;
		}
	} break;
	case E_Print: 
	{
		return typeCheck(((Print*)e)->expr, tenv);
	} break;
	default:
		FatalError("Couldn't typecheck on expression type. [%s]\n", toString(e).c_str());
	}
}

Type* IntegerTyp() {
	IntType* e = (IntType*)malloc(sizeof(IntType));

	e->t = T_Integer;

	return e;
}