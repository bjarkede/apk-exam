#include "ast.hpp"

Expression* EmptyExp() {
	Expression* e = (Expression*)malloc(sizeof(Expression));
	e->expType = E_NoType;
	return e;
}

Expression* VariableExp(L_STRING* val) {
	Var* e = (Var*)malloc(sizeof(Var));
	e->expType = E_Variable;
	e->name = val->contents;

	return e;
}

Expression* LetExp(Expression* var, Expression* binding, Expression* expr) {
	Let* e = (Let*)malloc(sizeof(Let));

	e->expType = E_LetBinding;
	e->variable = var;
	e->expr = expr;
	e->binding = binding;

	return e;
}

Expression* LetFunExp(const char* f, Buffer x, Expression* fbody, Expression* letbody) {
	LetFun* e = (LetFun*)malloc(sizeof(LetFun));
	e->expType = E_LetFun;
	e->f = f;
	e->x = x;
	e->fbody = fbody;
	e->letbody = letbody;
	return e;
}

Expression* CallExp(Expression* eFun, Buffer args) {
	Call* e = (Call*)malloc(sizeof(Call));
	e->expType = E_Call;
	e->eFun = eFun;
	e->args = args;
	return e;
}

Expression* IntegerExp(L_INTEGER i) {
	Integer* e = (Integer*)malloc(sizeof(Integer));

	e->expType = E_Integer;
	e->value = i;

	return e;
}

Expression* ParenExp(Expression* expr) {
	Paren* e = (Paren*)malloc(sizeof(Paren));
	e->expType = E_Paren;
	e->expr = expr;

	return e;
}

Expression* BinaryExp(BinaryOpType op, Expression* left, Expression* right) {
	BinOp* e = (BinOp*)malloc(sizeof(BinOp));

	e->expType = E_BinOp;
	e->opType = op;
	e->left = left;
	e->right = right;

	return e;
}

Expression* IfThenElseExp(Expression* ifexp, Expression* thenexp, Expression* elseexp) {
	If* e = (If*)malloc(sizeof(If));
	e->expType = E_IfThenElse;
	e->ifexp = ifexp;
	e->thenexp = thenexp;
	e->elseexp = elseexp;
	return e;
}

Expression* UnaryExp(UnOpType op, Expression* expr) {
	UnOp* e = (UnOp*)malloc(sizeof(UnOp));

	e->expType = E_UnOp;
	e->opType = op;
	e->expr = expr;

	return e;
}

Expression* PrintExp(Expression* expr) {
	Print* e = (Print*)malloc(sizeof(Print));
	e->expType = E_Print;
	e->expr = expr;
	return e;
}