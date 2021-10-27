#pragma once

#include <type_traits>

namespace aka::variadic
{

	template<typename... Ts>
	struct pack { };

	template<template<typename> typename... Ts>
	struct tpack { };

	template<typename T = pack<>, typename...>
	struct concat
	{
		using type = T;
	};

	template<typename... T1, typename... T2, typename... Ts>
	struct concat<pack<T1...>, pack<T2...>, Ts...>
		: concat<pack<T1..., T2...>, Ts...>
	{

	};

	template<typename... Ts>
	using concat_t = typename concat<Ts...>::type;

	template<typename T, typename Pack>
	struct contains
	{
		constexpr static bool value = false;
	};

	template<typename T, typename K, typename... Ks>
	struct contains<T, pack<K, Ks...>>
	{
		constexpr static bool value = std::is_same_v<T, K> ? true : contains<T, pack<Ks...>>::value;
	};

	template<typename T, typename Pack>
	constexpr bool contains_v = contains<T, Pack>::value;

	template<typename Acc, typename T>
	struct distinct_helper
	{
		using type = Acc;
	};

	template<typename... Ks, typename T, typename... Ts>
	struct distinct_helper<pack<Ks...>, pack<T, Ts...>>
	{
		using type = std::conditional_t
		<
			contains_v<T, pack<Ks...>>,
			typename distinct_helper<pack<Ks...>, pack<Ts...>>::type,
			typename distinct_helper<pack<Ks..., T>, pack<Ts...>>::type
		>;
	};

	template<typename T>
	struct distinct : distinct_helper<pack<>, T> { };

	template<typename T>
	using distinct_t = typename distinct<T>::type;

	template<typename Acc, typename Pack>
	struct flatten_helper
	{
		using type = Acc;
	};

	template<typename... Ks, typename... Ts, typename... Tails>
	struct flatten_helper<pack<Ks...>, pack<pack<Ts...>, Tails...>>
	{
		using type = typename flatten_helper<pack<Ks...>, pack<Ts..., Tails...>>::type;
	};

	template<typename... Ks, typename T, typename... Ts>
	struct flatten_helper<pack<Ks...>, pack<T, Ts...>>
	{
		using type = typename flatten_helper<pack<Ks..., T>, pack<Ts...>>::type;
	};

	template<typename Pack>
	using flatten_t = typename flatten_helper<pack<>, Pack>::type;

	template<typename Pack>
	using unique_flatten_t = distinct_t<flatten_t<Pack>>;

	template<typename Pack>
	class inherit_t;

	template<typename... Ts>
	class inherit_t<pack<Ts...>> : public Ts...
	{
		
	};

	template<typename Tpack, typename K>
	struct concrete
	{
		using type = Tpack;
	};

	template<template<typename> typename... Ts, typename K>
	struct concrete<tpack<Ts...>, K>
	{
		using type = pack<Ts<K>...>;
	};

	template<typename Tpack, typename K>
	using concrete_t = typename concrete<Tpack, K>::type;
}
