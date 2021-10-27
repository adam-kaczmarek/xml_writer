#pragma once

#include <string>
#include <string_view>
#include <stack>

#include "Xml_writer_internals.h"

#define XML_ASSERT(condition) // if (!(condition)) { BREAKPOINT; }

namespace aka::xml
{
	constexpr uint8_t XML_DECLARATION_STANDALONE = 0x01;
	constexpr uint8_t XML_DECLARATION_NO_STANDALONE = 0x02;

	template<typename Output, template<typename> typename... Extensions>
	class Writer : public internals::Writer_base<Writer<Output, Extensions...>, Extensions...>
	{
		enum class State
		{
			Empty,
			Post_declaration,
			Element_head,
			Element
		};

		Output& stream;
		std::stack<internals::Writer_stack_element> stack;

		State state = State::Empty;

		bool indenting = false;
		int indent_level = 0;
		bool in_root = true;
		std::string indent_string = "";
	public:
		Writer(Output& stream);
		Writer(const Writer&) = delete;
		Writer(Writer&&) = delete;
		~Writer();

		Writer& operator=(const Writer&) = delete;
		Writer& operator=(Writer&&) = delete;

		Writer& element(const std::string_view& name);
		Writer& element_end();
		Writer& element_end(const std::string_view& name); // with check

		template<typename T>
		Writer& attribute(const std::string_view& name, const T& value);

		template<typename T>
		Writer& text(const T& value);

		Writer& declaration(uint8_t options = 0);

		Writer& set_indenting(bool onoff);
		Writer& set_indent_level(int level);

		void document_end();

	private:
		void close_element_head();
		void close_empty_element_head();

		void write_indent();

		template<typename Extension>
		void helper_declare_namespaces(Extension& extension)
		{
			if constexpr (internals::has_namespaces<Extension>::value)
			{
				extension.declare_namespaces();
			}
		}
	};

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>::Writer(Output& stream) :
		stream(stream)
	{

	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>::~Writer()
	{
		document_end();
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::element(const std::string_view& str)
	{
		if (stack.size() > 0)
			++stack.top().subelements_no;
		if (state == State::Element_head)
			close_element_head();

		if (indenting)
		{
			stream << '\n';
			write_indent();
			++indent_level;
		}

		stream << '<' << str;

		state = State::Element_head;

		stack.push(internals::Writer_stack_element{str});

		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::element_end()
	{
		XML_ASSERT(stack.size() > 0);
		--indent_level;
		switch (state)
		{
		case State::Element_head:
			close_empty_element_head();
			state = State::Element;
			break;

		case State::Element:
			if (indenting && stack.top().subelements_no > 0)
			{
				stream << "\n";
				write_indent();
			}
			stream << "</" << stack.top().name << ">";
			break;
		}
		stack.pop();
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::element_end(const std::string_view& name) // with check!
	{
		XML_ASSERT(stack.top().name == name);
		return element_end();
	}

	template<typename Output, template<typename> typename... Extensions>
	template<typename T>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::attribute(const std::string_view& name, const T& value)
	{
		XML_ASSERT(state == State::Element_head);
		stream << " " << name << "=\"" << value << "\"";
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	template<typename T>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::text(const T& value)
	{
		switch (state)
		{
		case State::Element_head:
			close_element_head();
			state = State::Element;
			// no break
		case State::Element:
			if (indenting)
			{
				stream << '\n';
				write_indent();
			}
			stream << value;
			break;
		}
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::declaration(uint8_t options)
	{
		XML_ASSERT(state == State::Empty);
		stream << "<?xml version=\"1.0\" encoding=\"utf-8\"";
		if (options & XML_DECLARATION_STANDALONE)
			stream << " standalone=\"true\"";
		else if (options & XML_DECLARATION_NO_STANDALONE)
			stream << " standalone=\"no\"";
		stream << "?>";
		state = State::Post_declaration;
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::set_indenting(bool onoff)
	{
		indenting = onoff;
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline Writer<Output, Extensions...>& Writer<Output, Extensions...>::set_indent_level(int level)
	{
		indent_level = level;
		indenting = true;
		return *this;
	}

	template<typename Output, template<typename> typename... Extensions>
	inline void Writer<Output, Extensions...>::document_end()
	{
		while (stack.size() > 0)
			element_end();
	}

	template<typename Output, template<typename> typename... Extensions>
	inline void Writer<Output, Extensions...>::close_element_head()
	{
		if (in_root)
		{
			// automatically declare namespaces required by extensions before closing root head
			(helper_declare_namespaces(static_cast<Extensions<Writer<Output, Extensions...>>&>(*this)), ...);
			in_root = false;
		}
		stream << '>';
	}

	template<typename Output, template<typename> typename... Extensions>
	inline void Writer<Output, Extensions...>::close_empty_element_head()
	{
		stream << "/>";
	}

	template<typename Output, template<typename> typename... Extensions>
	inline void Writer<Output, Extensions...>::write_indent()
	{
		for (int i = 0; i < indent_level; ++i)
			stream << "  ";
	}
}
