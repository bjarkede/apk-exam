#include "typechecker.hpp"

std::map<const char*, Type*> makeInitialTypeEnv(Expression* program) {
	std::map<const char*, Type*> tenv;
	std::set<const char*> fv = freevars(program);
	for (auto& element : fv) {
		tenv.insert(std::make_pair(element, IntegerTyp()));
	}
	return tenv;
}

Type* typeCheck(Expression* e, std::map<const char*, Type*> tenv) {
	switch (e->expType) {
	case E_Integer: {
		return IntegerTyp();
	} break;
	case E_Variable: 
	{
		return tenv[(((Var*)e)->name)];
	} break;
	case E_BinOp: 
	{
		auto lefttype = typeCheck(((BinOp*)e)->left, tenv);
		auto righttype = typeCheck(((BinOp*)e)->right, tenv);

		switch (((BinOp*)e)->opType) {
		case Add: case Sub: case Mul: case Div: case Mod:
		{
			if (lefttype->t == T_Integer && righttype->t == T_Integer) return IntegerTyp();
			FatalError("Type mismatch at '+', found types %d and %d", lefttype->t, righttype->t);
		} break;
		}
	} break;
	case E_IfThenElse:
	{
		auto contype = typeCheck(((IfThenElse*)e)->ifexp, tenv);
		auto thentype = typeCheck(((IfThenElse*)e)->thenexp, tenv);
		auto elsetype = typeCheck(((IfThenElse*)e)->elseexp, tenv);
		switch (contype->t) {
		case T_Bool:
		{
			switch (thentype->t) {
			case T_Integer:
			{
				switch (elsetype->t) {
				case T_Integer:
				{
					return IntegerTyp();
				} break;
				} 
			} break;
			case T_Bool:
			{
				switch (elsetype->t) {
				case T_Bool:
				{
					return BoolTyp();
				}
				}
			} break;
			}
		} break;
		default:
			FatalError("Type mistmatch at 'if then else', found types %d, %d, %d", contype->t, thentype->t, elsetype->t);
		}
	} break;
	case E_LetFun:
	{
		// @TODO:
		// It might be easier to do typechecking if we add 
		// Explicit types to our language... but we dont do that... yet.
		FunType* funtype = (FunType*)malloc(sizeof(funtype));
		AllocBuffer(funtype->paramtypes, sizeof(Type*)*(((LetFun*)e)->x).length);

		// We need to get all the param types
		for(int i = 0; i < (((LetFun*)e)->x).writeIndex; i++)
		{
			((Type**)funtype->paramtypes.buffer)[funtype->paramtypes.writeIndex++] = typeCheck(((Expression**)((LetFun*)e)->x.buffer)[i], tenv);
		}
		funtype->resulttype = typeCheck(((LetFun*)e)->fbody, tenv);
	} break;
	case E_Call:
	{
		// @TODO:
		auto funtype = typeCheck(((Call*)e)->eFun, tenv);
		switch (funtype->t) {
		case T_Fun:
		{
		
		} break;
		default:
			FatalError("Function is not a function.");
		}
	} break;
	case E_LetBinding:
	{
		auto lettype = typeCheck(((Let*)e)->binding, tenv);
		switch (lettype->t) {
		case T_Integer:
		{
			return IntegerTyp();
		} break;
		default:
			if (((Let*)e)->expr == NULL)
				FatalError("Typemismatch in 'let end', found type %d", lettype->t);
			FatalError("Typemismatch in 'let in end'");
		}
	} break; 
	case E_Print: 
	{
		return typeCheck(((Print*)e)->expr, tenv);
	} break;
	case E_Fib:
	{
		return IntegerTyp();
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

Type* BoolTyp() {
	BoolType* e = (BoolType*)malloc(sizeof(BoolType));
	e->t = T_Bool;
	return e;
}