#include "interpreter.hpp"

Value* lookup(const char* name, symtable<Value*>* env) {
	Value* result = env->lookup(name);
	if (result->vType == V_NoType) FatalError("InterpreterError: Couldn't find variable [%s] in value environment.", name);
	return result;
}

bool closedin(std::vector<const char*> vs, Expression* e) {
	switch (e->expType) {
	case E_Integer: 
	{
		return true;
	} break;
	case E_Variable:
	{
		bool found = false;
		for (auto& element : vs) {
			if (strcmp(element, ((Var*)e)->name) == 0) { found = true; break; }
		}
		return (found ? true : false);
	} break;
	case E_LetBinding:
	{
		auto vs1 = vs;
		vs1.push_back(((Variable*)(((Let*)e)->variable))->name);
		bool closed = closedin(vs, (((Let*)e)->binding));
		return (closed && closedin(vs1, e) ? true : false);
	} break;
	case E_BinOp:
	{
		return (closedin(vs, ((BinOp*)e)->left) && closedin(vs, ((BinOp*)e)->right) ? true : false);
	} break;
	}
}

std::set<const char*> freevars(Expression* e) {
	switch (e->expType) {
	case E_Integer: 
	{
		printf("Hit an integer!\n");
		return std::set<const char*>();
	} break;
	case E_Variable: 
	{
		std::set<const char*> result;
		result.insert(((Variable*)e)->name);
		return result;
	} break;
	case E_LetBinding:
	{
		printf("Hit let-binding!\n");
		std::set<const char*> erhs;
		std::set<const char*> result;
		std::vector<const char*> bounded;

		auto body = freevars(((Let*)e)->expr); // Get the free variables in the body.
		auto data = freevars(((Let*)e)->binding);
		body.erase(((Variable*)((Let*)e)->variable)->name);
		bounded.push_back(((Variable*)((Let*)e)->variable)->name);
		for (auto const& var : data) {
			if (!closedin(bounded, ((Let*)e)->binding) && strcmp(((Variable*)((Let*)e)->variable)->name, var) != 0) {
				erhs.insert(var);
			}
		}
		std::set_union(erhs.begin(), erhs.end(), body.begin(), body.end(), std::inserter(result, result.begin()));
		return result;
	} break;
	case E_BinOp: 
	{
		printf("Hit an BinOp!\n");
		auto fe1 = freevars(((BinOp*)e)->left);
		auto fe2 = freevars(((BinOp*)e)->right);
		std::set<const char*> result;
		std::set_union(fe1.begin(), fe1.end(), fe2.begin(), fe2.end(), std::inserter(result, result.begin()));
		return result;
	} break;
	case E_Print: 
	{
		return freevars(((Print*)e)->expr);
	} break;
	}
};

Value* eval(Expression* e, symtable<Value*>* env) {
	switch (e->expType) {
	case E_Integer:
	{
		return MakeIntegerVal((((Integer*)e)->value));
	} break;
	case E_Variable:
	{
		return lookup(((Var*)e)->name, env);
	} break;
	case E_LetBinding: {
		Value* xval = eval(((Let*)e)->binding, env);
		if (((Let*)e)->expr != NULL) {
			symtable<Value*> letbodyenv = *env;
			letbodyenv.bind(xval, ((Var*)((Let*)e)->variable)->name);
			return eval(((Let*)e)->expr, &letbodyenv); // Evaluate the expression in-scope.
		}
		else {
			env->bind(xval, ((Var*)((Let*)e)->variable)->name);
			return NULL;
			break;
		}
	} break;
	case E_LetFun: {
		if (((LetFun*)e)->letbody != NULL) {
			symtable<Value*> bodyenv = *env;
			bodyenv.bind(MakeClosureVal((((LetFun*)e)->f), (((LetFun*)e)->x), (((LetFun*)e)->fbody), *env), (((LetFun*)e)->f));
			return eval(((LetFun*)e)->letbody, &bodyenv);
		}
		else {
			// The function will be called later
			env->bind(MakeClosureVal((((LetFun*)e)->f), (((LetFun*)e)->x), (((LetFun*)e)->fbody), *env), (((LetFun*)e)->f));
			break;
		}
	} break;
	case E_Print: {
		Value* v = eval(((Print*)e)->expr, env);
		switch (v->vType) {
		case V_Integer: {
			printf("%d\n", (((IntVal*)v)->i));
		} break;
		default:
			FatalError("InterpreterError: Couldn't print expression.");
		}
		return NULL;
	} break;
	case E_Call: {
		auto fClosure = eval(((Call*)e)->eFun, env);
		if (fClosure->vType == V_Closure) {
			symtable<Value*> fbodyenv = ((Closure*)fClosure)->fdeclenv;
			Buffer varNames = ((Closure*)fClosure)->x;
			// Evaluate each expression in the function call arguments and
			// bind them to the function body.
			for (int i = 0; i < ((Call*)e)->args.writeIndex; i++) {
				fbodyenv.bind(eval(((Expression**)((Call*)e)->args.buffer)[i], env), ((Var*)((Exp**)varNames.buffer)[i])->name);
			}
			fbodyenv.bind(fClosure, ((Var*)((Call*)e)->eFun)->name);
			return eval(((Closure*)fClosure)->fbody, &fbodyenv); // Now we can evaluate the function.
		}
	} break;
	case E_IfThenElse: {
		auto data = eval(((If*)e)->ifexp, env);
		if (data->vType == V_Integer) {
			return (((IntVal*)data)->i != 0 ? eval(((If*)e)->thenexp, env) : eval(((If*)e)->elseexp, env));
		}
		else {
			FatalError("InterpreterError: Didn't get a boolean when interpreting [if]-expression.");
		}
	} break;
	case E_BinOp:
	{
		Value* lvalue = eval(((BinOp*)e)->left, env);
		Value* rvalue = eval(((BinOp*)e)->right, env);

		switch (((BinOp*)e)->opType) {
		case Add:
		{
			switch (lvalue->vType) {
			case V_Integer:
			{
				switch (rvalue->vType) {
				case V_Integer: { return MakeIntegerVal(((IntVal*)lvalue)->i + ((IntVal*)rvalue)->i); }
				case V_Float: { return MakeFloatVal(((IntVal*)lvalue)->i + ((FloatVal*)rvalue)->f); }
				}
			} break;
			case V_Float: {
				switch (rvalue->vType) {
				case V_Integer: { return MakeIntegerVal(((FloatVal*)lvalue)->f + ((IntVal*)rvalue)->i); }
				case V_Float: { return MakeFloatVal(((FloatVal*)lvalue)->f + ((FloatVal*)rvalue)->f); }
				}
			}
			}
		} break;
		case Sub:
		{
			return MakeIntegerVal(((IntVal*)lvalue)->i - ((IntVal*)rvalue)->i);
		} break;
		case Mul:
		{
			return MakeIntegerVal(((IntVal*)lvalue)->i * ((IntVal*)rvalue)->i);
		} break;
		case Div:
		{
			return MakeIntegerVal(((IntVal*)lvalue)->i / ((IntVal*)rvalue)->i);
		}
		case Equal:
		{
			if (lvalue->vType == V_Integer && rvalue->vType == V_Integer) {
				return MakeIntegerVal(((IntVal*)lvalue)->i == ((IntVal*)rvalue)->i ? 1 : 0);
			}
			else {
				FatalError("InterpreterError: Unknown primitive or wrong type in [if]-expression.");
			}
		} break;
		case Less:
		{
			if (lvalue->vType == V_Integer && rvalue->vType == V_Integer) {
				return MakeIntegerVal(((IntVal*)lvalue)->i < ((IntVal*)rvalue)->i ? 1 : 0);
			}
			else {
				FatalError("InterpreterError: Unknown primitive or wrong type in [if]-expression.");
			}
		} break;
		}
	} break;
	case E_Paren: {
		return eval(((Paren*)e)->expr, env);
	} break;
	case E_UnOp: {
		Value* v = eval(((UnOp*)e)->expr, env);
		if (v->vType == V_Integer) {
			if ((((UnOp*)e)->opType == Minus)) {
				(((IntVal*)v)->i) = (((IntVal*)v)->i) * -1;
			}
		}
		return v;
	} break;
	}
}

std::string toString(Expression* e) {

	static const char* operands[] = { "+", "-", "*", "%", "^", "/", "", "==", "<" };

	std::ostringstream buffer;

	switch (e->expType) {
	case E_Integer:
	{
		buffer << ((Integer*)e)->value;
	} break;
	case E_Variable: {
		buffer << ((Var*)e)->name;
	} break;
	case E_BinOp: {
		buffer << toString(((BinOp*)e)->left) << operands[((BinOp*)e)->opType] << toString(((BinOp*)e)->right);
	} break;
	case E_Paren: {
		buffer << "(" << toString(((Paren*)e)->expr) << ")";
	} break;
	case E_LetBinding: {
		if (((Let*)e)->expr != NULL) {
			buffer << "let " << toString(((Let*)e)->variable) << " = " << toString(((Let*)e)->binding) << " in " << toString(((Let*)e)->expr) << " end";
		}
		else {
			buffer << "let " << toString(((Let*)e)->variable) << " = " << toString(((Let*)e)->binding) <<  " end";
		}
	} break;
	case E_LetFun: {
		Exp** x = (Exp**)((Call*)e)->args.buffer;

		buffer << "let " << (((LetFun*)e)->f) << "(";;
		if (((Call*)e)->args.writeIndex > 1) { // If there's more than one variable
			for (int i = 0; i < ((Call*)e)->args.writeIndex - 1; i++) {
				buffer << toString(x[i]) << ",";
			}
			buffer << toString(x[((Call*)e)->args.writeIndex-1]);
		}
		else {
			buffer << toString(x[0]);
		}
		if (((LetFun*)e)->letbody != NULL) {
			buffer << ") = " << toString(((LetFun*)e)->fbody) << " in " << toString(((LetFun*)e)->letbody) << " end";
		}
		else {
			buffer << ") = " << toString(((LetFun*)e)->fbody) << " end";
		}
	} break;
	case E_Call: {
		Exp** x = (Exp**)((Call*)e)->args.buffer;

		buffer << toString(((Call*)e)->eFun) << "(";
		if (((Call*)e)->args.writeIndex > 1) {
			for (int i = 0; i < ((Call*)e)->args.writeIndex - 1; i++) {
				buffer << toString(x[i]) << ",";
			}
			buffer << toString(x[((Call*)e)->args.writeIndex - 1]);
		}
		else {
			buffer << toString(x[0]);
		}
		buffer << ")";
	} break;
	case E_IfThenElse: {
		buffer << "if " << toString(((If*)e)->ifexp) << " then " << toString(((If*)e)->thenexp) << " else " << toString(((If*)e)->elseexp);
	} break;
	case E_Print: {
		buffer << "print(" << toString(((Print*)e)->expr) << ")";
	} break;
	case E_UnOp: {
		switch ((((UnOp*)e)->opType)) {
		case Minus: {
			buffer << "-" << toString(((UnOp*)e)->expr);
		} break;
		default:
			break;
		}
	}
	default:
		buffer << "";
	}

	return buffer.str();
}

Value* MakeEmptyVal() {
	Value* v = (Value*)malloc(sizeof(Value));
	v->vType = V_NoType;
	return v;
}

Value* MakeIntegerVal(L_INTEGER i) {
	IntVal* v = (IntVal*)malloc(sizeof(IntVal));
	v->vType = V_Integer;
	v->i = i;
	return v;
}

Value* MakeStringVal(L_STRING s) {
	StringVal* v = (StringVal*)malloc(sizeof(StringVal));
	v->vType = V_String;
	v->s.contents = s.contents;
	v->s.flags = s.flags;
	v->s.length = s.length;
	v->s.u = s.u;
	return v;
}

Value* MakeFloatVal(double f) {
	FloatVal* v = (FloatVal*)malloc(sizeof(FloatVal));
	v->vType = V_Float;
	v->f = f;
	return v;
}

Value* MakeClosureVal(const char* f,
	Buffer x,
	Expression* fbody,
	symtable<Value*> fdeclenv) {
	Closure* v = new Closure;
	v->vType = V_Closure;
	v->f = f;
	v->x = x;
	v->fbody = fbody;
	v->fdeclenv = fdeclenv;
	return v;
}