#include "lexer.hpp"
#include "parser.hpp"
#include "interpreter.hpp"
#include "typechecker.hpp"
#include "symtable.hpp"

symtable<Value*> valueenv;

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

	// @TODO:
	// The by value passing of the valueenv is a major bottleneck.
	// If we use a large hashmap, a lot of memory will be allocated at each
	// recursive call in the interpreter.
	// Consider an alternative. -bjarke, 6th octobor 2020.
	valueenv.empty(20);

	for (auto& e : expList) {
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