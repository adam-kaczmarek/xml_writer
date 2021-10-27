#pragma once

#include "Variadic_tools.h"

namespace aka::xml::internals
{
	using namespace aka::variadic;

	struct Writer_stack_element
	{
		std::string name;
		int subelements_no = 0;
		Writer_stack_element() = default;
		Writer_stack_element(const Writer_stack_element&) = default;
		Writer_stack_element(Writer_stack_element&&) = default;
		~Writer_stack_element() = default;

		template<typename S>
		Writer_stack_element(S&& s) :
			name(std::forward<S>(s))
		{

		}
	};

	template<typename C>
	struct has_namespaces
	{
	private:
		template<typename T>
		static constexpr auto check(T*) -> typename std::is_same<decltype(std::declval<T>().declare_namespaces()), void>::type;

		template<typename>
		static constexpr std::false_type check(...);

		typedef decltype(check<C>(0)) type;
	public:
		static constexpr bool value = type::value;
	};

	template<typename T>
	struct to_void
	{
		using type = void;
	};

	template<typename T, typename dummy = void>
	struct has_extensions : std::false_type {};

	template<typename T>
	struct has_extensions<T, typename to_void<typename T::required_extensions_t>::type> : std::true_type { };

	template<typename T>
	constexpr bool has_extensions_v = has_extensions<T>::value;

	template<typename T, typename Acc, typename... Exts>
	struct extender_helper
	{
		using type = Acc;
	};

	template<typename W, typename Pack, typename T, bool b>
	struct expand_pack
	{
		using type = Pack;
	};

	template<typename W, typename T, typename... Ts>
	struct expand_pack<W, pack<Ts...>, T, true>
	{
		using type = pack<Ts..., concrete_t<typename T::required_extensions_t, W>>;
	};

	template<typename W, typename Pack, typename T, bool b>
	using expand_pack_t = typename expand_pack<W, Pack, T, b>::type;

	template<typename T, typename Ext, typename... Ts, typename... Exts>
	struct extender_helper<T, pack<Ts...>, Ext, Exts...>
	{
		using type = typename extender_helper<T, expand_pack_t<T, pack<Ts...>, Ext, has_extensions_v<Ext>>>::type;
	};

	template<typename T, typename... Exts>
	struct  extender : extender_helper<T, pack<>, Exts...> { };

	template<typename T, typename... Exts>
	using extender_t = typename extender<T, Exts...>::type;

	template<typename P, template<typename> typename... Extensions>
	class Writer_base : public inherit_t<unique_flatten_t<pack<Extensions<P>..., extender_t<P, Extensions<P>...>>>>
	{

	};
}
