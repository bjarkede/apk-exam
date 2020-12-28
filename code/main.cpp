#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "typechecker.hpp"
#include "symtable.hpp"

bool typecheck = false;

symtable<Value*> valueenv;

int main(int argc, char** argv) {

	LexerState ls;

    if (!InitializeLexer(argv[1], &ls)) {
        FatalError("Couldn't process input stream: %s\n", argv[1]);
    }

	for(int i = 2; i < argc; ++i)
	{
		const char* const arg = argv[i];
		if(strstr(arg, "-typecheck") == arg)
		{
			typecheck = true;
		}
	}

	std::vector<Expression*> expList;
    
	ProcessNextToken(&ls);
	while (ls.t.token != TK_EOZ) {
		expList.push_back(ParseExpression(&ls));
	}

	// @TODO:
	// The by value passing of the valueenv is a major bottleneck.
	// If we use a large hashmap, a lot of memory will be allocated at each
	// recursive call in the interpreter.
	// Consider an alternative. -bjarke, 6th octobor 2020.
	valueenv.empty(20);

	for (auto& e : expList) {
		if (typecheck) {
			std::map<const char*, Type*> tenv = makeInitialTypeEnv(e);
			auto t = typeCheck(e, tenv);
			free(t);
		}
		
		auto v = eval(e, &valueenv);
		
		free(e);
		free(v);
	}

    return 1;
}