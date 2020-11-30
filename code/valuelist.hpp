#ifndef VALUELIST_HPP
#define VALUELIST_HPP

struct NullType {};

template <auto... Values>
struct Valuelist {};

template <auto Value>
struct CTValue
{
	static constexpr auto value = Value;
};

namespace VL {
	template <typename List>
	struct IsEmpty;

	template <auto... Values>
	struct IsEmpty<Valuelist<Values...>> {
		static constexpr bool value = sizeof...(Values) == 0;
	};

	template <typename List>
	struct FrontT;

	template <auto Head, auto... Tail>
	struct FrontT<Valuelist<Head, Tail...>> {
		using Type = CTValue<Head>;
		static constexpr auto value = Head;
	};

	template <typename List>
	using Front = typename FrontT<List>::Type;

	template <typename List>
	struct PopFrontT;

	template <auto Head, auto... Tail>
	struct PopFrontT<Valuelist<Head, Tail...>> {
		using Type = Valuelist<Tail...>;
	};

	template <typename List>
	using PopFront = typename PopFrontT<List>::Type;

	template <typename List, auto New>
	struct PushFrontT;

	template <auto... Values, auto New>
	struct PushFrontT<Valuelist<Values...>, New> {
		using Type = Valuelist<New, Values...>;
	};

	template <typename List, auto New>
	using PushFront = typename PushFrontT<List, New>::Type;

	template <typename List, auto New>
	struct PushBackT;

	template <auto... Values, auto New>
	struct PushBackT<Valuelist<Values...>, New> {
		using Type = Valuelist<Values..., New>;
	};

	template <typename List, auto New>
	using PushBack = typename PushBackT<List, New>::Type;

	// NthElement
	template <typename List, unsigned N>
	struct NthElementT : public NthElementT <PopFront<List>, N-1> {};

	template <typename List>
	struct NthElementT<List, 0> : public FrontT<List> {};

	template <typename List, unsigned N>
	using NthElement = typename NthElementT<List, N>::Type;
};


#endif