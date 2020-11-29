#ifndef OPTIMIZATIONS_HPP
#define OPTIMIZATIONS_HPP

#include "shared.hpp"

// Compile-time power function
template <bool IsPower2, int N>
struct powN {
	static constexpr double pow(double x) {
#define N1 (N & (N-1))
		return powN<(N1& (N1 - 1)) == 0, N1>::pow(x) * powN<true, N - N1>::pow(x);
#undef N1
	}
};

template <int N>
struct powN<true, N> {
	static constexpr double pow(double x) {
		return powN<true, N / 2>::pow(x) * powN<true, N / 2>::pow(x);
	}
};

template<>
struct powN<true, 1> {
	static constexpr double pow(double x) {
		return x;
	}
};

template<>
struct powN<true, 0> { 
	static constexpr double pow(double x) { 
		return 1.0; 
	} 
};

template <int N>
static inline double intPow(double x) {
	return powN<(N & N-1) == 0, N>::pow(x);
}

// Fib compile-time:
// Recursive template, used below
template <uint64_t N>
struct fibN {
	static constexpr uint64_t fib() {
// @NOTE:
// the const declaration may in some cases take memory space where a 
// #define directive never takes memory space.
#define N1 (N-1)
#define N2 (N1-1)
		if constexpr (N == 1 || N == 0) {
			return N; // fib(0 or 1)
		}
		return fibN<N1>::fib() + fibN<N2>::fib();
	}
#undef N1
#undef N2
};

template<>
struct fibN<0> {
	static constexpr uint64_t fib() {
		return 0;
	}
};

template<>
struct fibN<1> {
	static constexpr uint64_t fib() {
		return 1;
	}
};

template <uint64_t N>
static inline uint64_t fibseq() {
	return fibN<N>::fib();
}

// @TODO:
// Compile-time lookup-table generation.

double pow_ct(double x, int p);

#endif
