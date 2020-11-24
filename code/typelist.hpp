#ifndef TYPELIST_HPP
#define TYPELIST_HPP

struct NullType {};

template <typename... Types>
struct Typelist {};

namespace TL {

	template <typename...>
	struct MakeTypelist;

	template <typename... Types>
	using make_typelist = typename MakeTypelist<Types...>::Type;

	template <typename Front, typename... Tail>
	struct MakeTypelist<Front, Tail...>
	{
		using Type = Typelist<Front, make_typelist<Tail...>>;
	};

	template <>
	struct MakeTypelist<>
	{
		using Type = NullType;
	};

	// Front
	template <typename List>
	struct FrontT;

	template <typename Front, typename... Tail>
	struct FrontT<Typelist<Front, Tail...>>
	{
		using Type = Front;
	};

	template <typename List>
	using Front = typename FrontT<List>::Type;

	template <typename List>
	struct PopFrontT;

	template <typename Front, typename... Tail>
	struct PopFrontT<Typelist<Front, Tail...>>
	{
		using Type = Typelist<Tail...>;
	};

	template <typename List>
	using PopFront = typename PopFrontT<List>::Type;

	// NthElement of the Typelist
	template <typename List, unsigned N>
	struct NthElementT : public NthElementT<PopFront<List>, N-1> {};

	template <typename List>
	struct NthElementT<List, 0> : public FrontT<List> {};

	template <typename List, unsigned N>
	using NthElement = typename NthElementT<List, N>::Type;

	// Calculate length of typelist
	template <typename List> struct Length;
	template <> struct Length<NullType>
	{
		static constexpr size_t value = 0;
	};

	template <typename T, typename U>
	struct Length<Typelist<T, U>>
	{
		static constexpr size_t value = 1 + Length<U>::value;
	};

	// Type-switching.
	template <typename List, int N = 0, bool Stop=(N == Length<List>::value)>
	struct TypeSwitch;

	template <typename List, int N, bool Stop>
	struct TypeSwitch
	{
		template <typename Functor>
		decltype(Functor().template operator() < typename NthElementT<List, N>::Type > ()) operator()(int i, Functor f)
		{
			if (i == N) {
				return f.template operator()<typename NthElementT<List, N>::Type> ();
			}
			else {
				TypeSwitch<List, N + 1> next_element;
				return next_element.operator()(i, f);
			}
		}
	};

	// Base-case
	template <typename List, int N>
	struct TypeSwitch<List, N, true>
	{
		template <typename Functor>
	    decltype(Functor().template operator() < NullType > ()) operator()(int, Functor)
		{
			FatalError("Index is out of range.");
		}
	};
};



#endif