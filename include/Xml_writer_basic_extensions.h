#pragma once

#include <string_view>

namespace aka::xml
{
	template<typename Writer>
	class Basic_extension
	{
	public:
		Writer& empty_element(const std::string_view name)
		{
			return self()
				.element(name)
				.element_end();
		}

		template<typename T>
		Writer& text_element(const std::string_view name, T&& t)
		{
			return self()
				.element(name)
					.text(std::forward<T>(t))
				.element_end();
		}

		template<typename T>
		Writer& attribute_element(const std::string_view element_name, const std::string_view attribute_name, T&& value)
		{
			return self()
				.element(element_name)
					.attribute(attribute_name, value)
				.element_end();
		}

		template<typename T>
		Writer& declare_namespace(const std::string_view nmsp, const std::string_view prefix)
		{
			std::string attribute_name = "xmlns:";
			attribute_name += prefix;
			return self().attribute(attribute_name, nmsp);
		}

		template<typename T>
		Writer& declare_namespace(const std::string_view nmsp)
		{
			return self().attribute("xmlns", nmsp);
		}

	protected:
		Writer& self() { return static_cast<Writer&>(*this); }
	};
}
