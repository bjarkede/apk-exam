#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "typechecker.hpp"
//#include "symtable.hpp"
#include"hashmap.h"
#include<array>
//symtable<Value*> valueenv;

int main(int argc, char** argv) {

    LexerState* ls = (LexerState*)malloc(sizeof(LexerState));

    if (!InitializeLexer(argv[1], ls)) {
        FatalError("Couldn't process input stream: %s\n", argv[1]);
    }

	std::vector<Expression*> expList;

	ProcessNextToken(ls);
	while (ls->t.token != TK_EOZ) {
		expList.push_back(ParseExpression(ls));
	}

	//PrintDebug("SyntaxMessage: Input accepted by parser.\n\n");
  const size_t size = 701;
  int bufsize = 701 * 8 * 4; //8 pr element, 4 times to make space for links
  std::array<std::byte,bufsize> buf;
  std::pmr::monotonic_buffer_resource mbr(buf.data(),
                                          buf.size(),
                                        std::pmr::null_memory_resource);
  hmap::hashmap<Value*,const char*,size> valueenv(&mbr);
	// @TODO:
	// The by value passing of the valueenv is a major bottleneck.
	// If we use a large hashmap, a lot of memory will be allocated at each
	// recursive call in the interpreter.
	// Consider an alternative. -bjarke, 6th octobor 2020.
	//valueenv.empty(20);

	for (auto& e : expList) {
		std::map<const char*, Type*> tenv = makeInitialTypeEnv(e);
		auto t = typeCheck(e, tenv);
		auto v = eval(e, &valueenv);
		/*if (v != nullptr) {
			switch (v->vType) {
			case V_Integer: {
				// Only print if the expression returns an integer
				PrintDebug("Expression: %s\n", toString(e).c_str());
				PrintDebug("Value: %d\n\n", ((IntVal*)v)->i);
			} break;
			default:
			{}
			}
		}*/
	}

    //PrintDebug("Finished lexing, parsing and interpreting file: %s\n", argv[1]);

    return 1;
}
