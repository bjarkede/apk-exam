#include "typechecker.hpp"
#include "typelist.hpp"



std::map<const char*, Type*> makeInitialTypeEnv(Expression* program) {
	std::map<const char*, Type*> tenv;
	std::set<const char*> fv = freevars(program);
	for (auto& element : fv) {
		tenv.insert(std::make_pair(element, IntegerTyp()));
	}
	return tenv;
}

struct Functor
{
	template <class T> Type* operator()()
	{
		std::cout << "Called with the following type: " << typeid(T).name() << std::endl;
		return 
	}
};

Type* typeCheck(Expression* e, std::map<const char*, Type*> tenv) {
	
	// Define a type-list of all the different types.
	typedef TL::make_typelist<IntType, StringType> TestTypes;
	
	Functor fun;
	TL::TypeSwitch<TestTypes> ts;

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
			return ts(lefttype->t, fun);
		} break;
		}
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