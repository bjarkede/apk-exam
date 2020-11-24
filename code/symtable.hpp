#ifndef SYMTABLE_HPP
#define SYMTABLE_HPP

#include "shared.hpp"

#include <stack> // For testing!

// @TODO:
// I don't want to use std::map for symbol tables.
// This is going to be an attempt at a symbol table.
// Interesting links: http://www.cs.cmu.edu/~rwh/theses/okasaki.pdf

// Auxiliary stack for entering/exiting local scopes?

// We use radix-notation for characters strings
// the identifier pt is (112, 116) p = 112, and t = 116 in ascii.
// expressed as radix-128 int (112*128)+116 = 14452
// h(k) = k mod 701 (701 being the table size)

// The typename 'T' is the value we map to.
template <typename T>
struct symtable {
private:
	typedef struct element {
		const char* key = ""; // Set default value, so we can use strcmp
		T value;
	} E;

	E* buffer;
	E* old_buffer;

	std::stack<E> auxStack;

	int m;

	int h(const char* name, int i);

	void reallocate(int num);
protected:
public:
	symtable();
	symtable(symtable<T> const& other);
	~symtable();

	void empty(int num);
	void bind(T value, const char* key);
	T lookup(const char* key);
	void enter();
	void exit();

	T& operator[] (const char* name);

	template <typename U>
	symtable<T>& operator = (symtable<T> const& other) {
		for (int i = 0; i < other.capacity(); i++) {
			buffer[i] = other.buffer[i];
		}
		return *this;
	}
	enum DefineSize {
		AllocateSpace = 701 // We cant use a table size of power of 2 so we use size = 2^p - 1
	};
};

// This function takes a character sequence and turns it into 
// a radix-128 integer. 
template <typename T>
int symtable<T>::h(const char* value, int i) {
	
	unsigned long long index = 0;
	int radix = 128;
	int power = 1;

	for (int j = 0; j < strlen(value); j++) {
		index = ((index + (value[j]) * power) % m) % m;
		power = (power * radix) % m;
	}

	return index + i;
}

template <typename T>
void symtable<T>::bind(T value, const char* key) {
	for (int i = 0; i < m; i++) {
		int j = h(key, i);

		// @TODO:
		// This will break at some point, keep that in mind xd.
		buffer[j].key = key;
		auxStack.push(buffer[j]);
		buffer[j].value = value;
		return;
	}
	FatalError("InterpreterError: Hash table overflow");
}

template <typename T>
symtable<T>::symtable(symtable<T> const& other) {
	//PrintDebug("Copy-constructor called\n");
	buffer = new E[other.m];
	old_buffer = new E[other.m];
	m = other.m;

	if (other.buffer)
		memcpy(buffer, other.buffer, sizeof(E)*other.m);
	if (other.old_buffer)
		memcpy(old_buffer, other.old_buffer, sizeof(E) * other.m);
	
	auxStack = other.auxStack;
}

template <typename T>
T symtable<T>::lookup(const char* key) {
	for (int i = 0; i < m; i++) {
		int j = h(key, i);
		if (j >= m) break;
		if (buffer[j].key != NULL) {
			if(strcmp(buffer[j].key, key) == 0)
				return buffer[j].value;
		}
	}
	FatalError("InterpreterError: Couldn't find variable [%s] in value environment.", key);
}

template <typename T>
void symtable<T>::reallocate(int num) {
	if (old_buffer) delete[] old_buffer;

	E* new_buffer = 0;
	new_buffer = new E[num];
	if (new_buffer == 0) FatalError("InterpreterError: Failed allocating symbol table");
	if (buffer) {
		memcpy(new_buffer, buffer, sizeof(buffer));
	}

	old_buffer = buffer;
	buffer = new_buffer;
	m = num;
}

template <typename T>
void symtable<T>::empty(int num) {
	if (num <= m) {
		if (num <= 0) {
			if(num < 0) FatalError("SymTable: Tried allocating less than 0.");
			//if (buffer) delete[] buffer;
			buffer = 0;
			m = 0;
			return;
		}
		return;
	}

	reallocate(num);
	if (old_buffer) { delete[] old_buffer; old_buffer = 0; }
}

template <typename T>
symtable<T>::symtable() {
	buffer = old_buffer = 0;
	m = 0;
	//PrintDebug("Allocated symbol table.\n");
}

template <typename T>
symtable<T>::~symtable() {
	//PrintDebug("Deallocated symbol table.\n");
	empty(0);
}

// When a scope is entered a 'marker' is pushed onto the auxiliary stack.
template <typename T>
void symtable<T>::enter() {
	E marker = { "xmark",  NULL};
	auxStack.push(marker);
}

// Pop the auxiliary stack untill a marker is reached.
template <typename T>
void symtable<T>::exit() {
	while (strcmp(auxStack.top().key, "xmark") != 0) {
		int j = h(auxStack.top().key, 0);

		buffer[j].value = auxStack.top().value;
		buffer[j].key = auxStack.top().key;

		auxStack.pop();
	}
	auxStack.pop();
}

#endif