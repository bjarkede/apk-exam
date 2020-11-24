#ifndef LOCAL_HPP
#define LOCAL_HPP

#include "shared.hpp"

#define L_INTEGER int64_t
#define L_BOOL bool
#define L_NUMBER double

typedef struct L_STRING {
	u8 flags;
	u8 length;
	union {
		size_t longLength;
		struct L_STRING* nextHash; // ???
	} u;
	const char* contents;
} L_STRING;

typedef union L_VALUE {
	typedef union Value {
		L_INTEGER i;
		L_BOOL b;
		L_NUMBER d;
	} Value;
} L_VALUE;

enum RESERVED {
	TK_LET, TK_IF, TK_THEN, TK_ELSE, TK_IN, TK_END,
	TK_TRUE, TK_FALSE, TK_PRINT, TK_VAR, TK_INT, TK_FLOAT,
	TK_EQ, TK_ASSN, TK_AND, TK_OR, TK_LE, TK_MIN, TK_MAX,
	TK_SEPR, TK_SEPL, TK_COM, TK_FUNC, TK_STRING,
	TK_EOZ,

	TK_ADD, TK_SUB, TK_MUL, TK_DIV,

	TK_COUNT
};

static const char* const tokens[] = {
	"let", "if", "then", "else", "in", "end", "true", "false", "print"
};

#define NUM_RESERVED ((int)(TK_COUNT));

typedef union {
	L_INTEGER i;
	L_STRING* s;
} SemanticInfo;

typedef struct Token {
	int token;
	int line, col;
	SemanticInfo semInfo;
} Token;

typedef struct LexerState {
	u8 currentChar; // Current charint
	int lineNumber;  // Current linenumber (gets incremented when /n/r)
	int lastLine;    // Line of the last consumed token.
	int lastCol; // Column of last consumed token.

	Token t;
	Token lookahead;

	Buffer IOZ; // Character sequence stream.
	u8* z; // Pointer to the current character in the input stream.
	Buffer TBuffer; // Buffer for tokens.
} LexerState;


#endif