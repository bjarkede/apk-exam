#include "lexer.hpp"

bool InitializeLexerState(LexerState* ls, int firstChar) {
    ls->t = {};
    ls->lookahead = {};
    
    ls->currentChar = firstChar;
    ls->lookahead.token = TK_EOZ;

    ls->lineNumber = ls->lastLine = 1;

    ls->z = (u8*)(ls->IOZ.buffer);
    if (!AllocBuffer(ls->TBuffer, MINBUFFER)) {
        FatalError("Couldn't allocate %d bytes for token buffer.", MINBUFFER);
        return false;
    }

    return true;
}

bool InitializeLexer(char* filePath, LexerState* ls) {
    if (ReadEntireFile(ls->IOZ, filePath)) {
        if (InitializeLexerState(ls, ((char*)ls->IOZ.buffer)[0])) {
            return true;
        }
    }
    else {
        return false;
    }
    return true;    
}

void ProcessNextToken(LexerState* ls) {
    ls->lastLine = ls->lineNumber;
    ls->lastCol = ls->t.col;
    if (ls->lookahead.token != TK_EOZ) {  // If there is a look-a-head token use that one.
        ls->t = ls->lookahead;
        ls->lookahead.token = TK_EOZ;
    }
    else { // Else get the next token.
        ls->t.token = Lex(ls, &ls->t.semInfo);
        reset_buffer(ls);
    }
}

int ProcessLookAHeadToken(LexerState* ls) {
    assert(ls->lookahead.token == TK_EOZ);
    ls->lookahead.token = Lex(ls, &ls->lookahead.semInfo);
    return ls->lookahead.token;
}

int Lex(LexerState* ls, SemanticInfo* semInfo) {
    //ls->TBuffer.buffer = 0; // Maybe add more buffer functions to the struct.
    semInfo->s = 0;
    for (;;) {
        switch (ls->currentChar) {
        case '\n': case '\r': {
            InclineLineNumber(ls);
        } break;
        case ' ': case '\f': case '\t': case '\v': {
            next(ls);
        } break;
        case '=': {
            next(ls);
            if (ls->currentChar == '=') {
                next(ls);
                return TK_EQ;
            }
            return '=';
        } break;
        case '/':
        {
            next(ls);
            if (ls->currentChar != '/') return '/';
            while (ls->currentChar != '\n' || ls->currentChar != '\r' && ls->currentChar == EOZ) {
                next(ls);
            } break;
        } break;
        case '\0': {
            return TK_EOZ;
        } break;
        case '+': {
            if (!isdigit(ls->currentChar)) { save_and_next(ls); return  '+';}
            return ReadNumeralLiteral(ls, semInfo);
        } break;
        case '-': { 
            if (!isdigit(ls->currentChar)) { save_and_next(ls); return  '-'; }
            return ReadNumeralLiteral(ls, semInfo);
        } break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            return ReadNumeralLiteral(ls, semInfo);
        } break;
        default:
            if (isalpha(ls->currentChar) || ls->currentChar == '_') {
                L_STRING* rs;

                do {
                    save_and_next(ls);
                } while (isalnum(ls->currentChar) || ls->currentChar == '_' || isdigit(ls->currentChar));

                SaveToken(ls, '\0');

                rs = CreateVariableString(ls, (char*)ls->TBuffer.buffer, ls->TBuffer.length);
                semInfo->s = rs;

                if (IsReserved(semInfo->s)) {
                    // When checking to see if the string is a reserved keyword
                    // the flag gets set as the token id.
                    return semInfo->s->flags;
                }
                else {
                    return TK_VAR;
                }
            }
            else {
                // Single-character tokens: (, +, -, ... 
                int c = ls->currentChar;
                next(ls);                
                return c;
            }
        }
    }
}

RESERVED SingleCharToToken(int op) {
    switch (op) {
    case '(': return TK_SEPL;
    case ')': return TK_SEPR;
    case '*': return TK_MUL;
    case '/': return TK_DIV;
    default:
        FatalError("LexingError: Couldn't get single-character token %c", (char)op);
    }
}

bool IsReserved(L_STRING* str) {
    for (int i = 0; i < ARRAY_LEN(tokens); i++) {
        if (strcmp(str->contents, tokens[i]) == 0) {
            str->flags = i;
            return true;
        }
    }
    return false;
}

L_STRING* CreateVariableString(LexerState* ls, const char* str, size_t l) {
    L_STRING* s = (L_STRING*)malloc(sizeof(L_STRING));

    // Allocate memory for the string
    
    s->contents = (char*)malloc(l);
    s->length   = l;
    s->flags    = 0;

    // Copy the string from the token buffer to the L_STRING.
    memcpy((void*)s->contents, str, l * sizeof(char));

    return s;
}

// TODO:
int ReadNumeralLiteral(LexerState* ls, SemanticInfo* semInfo) {
    int first = ls->currentChar;
    int value = 0;

    assert(isdigit(first));
    save_and_next(ls);
    for (;;) {
        if (isdigit(ls->currentChar))
            save_and_next(ls);
        else break;
    }

    SaveToken(ls, '\0'); // add null to indicate end of sequence.

    if (!StringToNumber(ls, &value)) FatalError("Failed when converting string to number.");
    
    semInfo->i = value;
    return TK_INT;

    //ls->t.data = (void*)value;
    
}

bool StringToNumber(LexerState* ls, int* value) {

    unsigned int result = 0;
    int negative;

    char* buff = (char*)ls->TBuffer.buffer;

    if (*buff == '-') { buff++; negative = 1; }
    else if (*buff == '+') { buff++; negative = 0;} else { negative = 0; }

    for (;isdigit((char)*buff);buff++) {
        int d = *buff - '0';
        result = result * 10 + d;
    }

    *value = (negative ? 0u - result : result);

    return true;
}

void SaveToken(LexerState* ls, int c) {
    // if we are about to write out of memory
    if (ls->TBuffer.writeIndex + 1 > ls->TBuffer.length) {
        if (ls->TBuffer.old_buffer) delete[] ls->TBuffer.old_buffer;

        void* new_buffer = 0;
        new_buffer = malloc(ls->TBuffer.length * 2);
        if (new_buffer == 0) FatalError("Failed to allocate buffer when saving.");
        if (ls->TBuffer.buffer) {
            memcpy(new_buffer, ls->TBuffer.buffer, ls->TBuffer.length);
        }

        ls->TBuffer.old_buffer = ls->TBuffer.buffer;
        ls->TBuffer.buffer = new_buffer;
    }

    ls->TBuffer.old_buffer = ls->TBuffer.buffer;
    ((char*)ls->TBuffer.buffer)[ls->TBuffer.writeIndex++] = (char)c;
}

void InclineLineNumber(LexerState* ls) {
    int old = ls->currentChar;
    assert(ls->currentChar == '\n' || ls->currentChar == '\r');
    next(ls);
    if ((ls->currentChar == '\n' || ls->currentChar == '\r') && ls->currentChar != old)
        next(ls);
    if (++ls->lineNumber >= INT64_MAX)
        FatalError("Read too many lines in current chunk. Integer overflow.\n");

    ls->t.col = 0;
}

size_t skip_comment(LexerState* ls) {
    size_t count = 0;
    int s = ls->currentChar;
    assert(s == '*');
    save_and_next(ls);
    while (ls->currentChar == '=') {
        save_and_next(ls);
        count++;
    }
    return (ls->currentChar == s) ? count + 2 : (count == 0) ? 1 : 0;
}