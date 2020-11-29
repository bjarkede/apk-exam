#include "optimizations.hpp"

// If 'n' is power of 10 we can optimize this call at compile time
double pow_ct(double x, int p) {
	switch (p) {
	case 1:
	{
		return intPow<1>(x);
	} break;
	case 2:
	{
		return intPow<2>(x);
	} break;
	case 3:
	{
		return intPow<3>(x);
	} break;
	case 4:
	{
		return intPow<4>(x);
	} break;
	case 5:
	{
		return intPow<5>(x);
	} break;
	case 6: 
	{
		return intPow<6>(x);
	} break;
	case 7:
	{
		return intPow<7>(x);
	} break;
	case 8:
	{
		return intPow<8>(x);
	} break;
	case 9:
	{
		return intPow<9>(x);
	} break;
	case 10:
	{
		return intPow<10>(x);
	} break;
	default:
		FatalError("Compile-time power constant out of range. [%d]", p);
	}
}

uint64_t factorial(int n) {
	// Table of factorials
}