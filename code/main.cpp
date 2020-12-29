#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "typechecker.hpp"
#include "symtable.hpp"
#include "./hash/hashmap.h"

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

	const size_t buffer_size = 701 * 8 * 4;
	std::array<std::byte, buffer_size> buffer;
	

	std::pmr::monotonic_buffer_resource mbr(buffer.data(), buffer.size(),
	std::pmr::null_memory_resource());

	hmap::hashmap<Value*, const char*> env(&mbr);
	
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
		
		//auto v = eval(e, &valueenv);
		auto v2 = eval(e, &env);
		
		free(e);
		free(v2);
	}

    return 1;
}