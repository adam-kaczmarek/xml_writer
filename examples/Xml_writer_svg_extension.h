#pragma once

#include <vector>
#include <string>
#include <string_view>

#include <Xml_writer.h>
#include <Xml_writer_basic_extensions.h>

#include <cmath>

#include <fmt/format.h>

namespace aka::xml
{
	namespace svg_literals
	{
		constexpr float operator""_rad(long double num)
		{
			return (float)(num / 3.14159265 * 180);
		}

		constexpr float operator""_rad(unsigned long long num)
		{
			return operator""_rad((long double)num);
		}

	}

	enum class Svg_transform_strategy : uint8_t
	{
		always_exact,	// all operations are written as is
		always_matrix,	// all operations are combined into single matrix
		smart			// single operations remain as is, more are converted to matrix
	};

	template<typename Writer>
	class Xml_writer_svg_extension
	{
		Svg_transform_strategy strategy = Svg_transform_strategy::smart;
		int width;
		int height;
		double view_x;
		double view_y;
		double view_width;
		double view_height;
	public:
		using required_extensions_t = aka::variadic::tpack<Basic_extension>;

		class Transform_writer
		{
			Writer& writer;
			std::vector<std::string> transforms;
			Svg_transform_strategy strategy;
		public:
			Transform_writer(Writer& writer, Svg_transform_strategy strategy) :
				writer(writer),
				strategy(strategy)
			{

			}

			Transform_writer& translate(int x, int y)
			{
				transforms.push_back(fmt::format("translate({} {})", x, y));
				return *this;
			}

			Transform_writer& rotate(float angle)
			{
				transforms.push_back(fmt::format("rotate({})", angle));
				return *this;
			}

			Transform_writer& rotate_around(int x, int y, float angle)
			{
				return
					translate(-x, -y).rotate(angle).translate(x, y);
			}

			Transform_writer& scale(float x)
			{
				transforms.push_back(fmt::format("scale({})", x));
				return *this;
			}

			Transform_writer& scale(float sx, float sy)
			{
				transforms.push_back(fmt::format("scale({} {})", sx, sx));
				return *this;
			}

			Writer& end_transform()
			{
				if (transforms.size() == 0)
					return writer;

				std::string transform_string = transforms.back();
				for (int i = transforms.size() - 2; i >= 0; --i)
				{
					transform_string += " ";
					transform_string += transforms[i];
				}

				return writer.attribute("transform", transform_string);
			}
		};

		void declare_namespaces()
		{
			self()
				.attribute("xmlns", "http://www.w3.org/2000/svg")
				.attribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
		}

		Writer& svg_start(int width, int height, double view_x, double view_y, double view_width, double view_height)
		{
			this->width = width;
			this->height = height;
			this->view_x = view_x;
			this->view_y = view_y;
			this->view_width = view_width;
			this->view_height = view_height;
			return self()
				.element("svg")
					.attribute("width", width)
					.attribute("height", height)
					.attribute("viewBox", fmt::format("({} {} {} {})", view_x, view_y, view_width, view_height));
		}

		template<typename T>
		Writer& rect_start(T x, T y, T width, T height)
		{
			return self()
				.element("rect")
					.attribute("x", x)
					.attribute("y", y)
					.attribute("width", width)
					.attribute("height", height);
		}

		template<typename T>
		Writer& ellipse_start(T cx, T cy, T rx, T ry)
		{
			return self()
				.element("ellipse")
					.attribute("cx", cx)
					.attribute("cy", cy)
					.attribute("rx", rx)
					.attribute("ry", ry);
		}

		Writer& stroke(uint32_t color, int width)
		{
			return self()
				.color_attribute("stroke", color)
				.attribute("stroke-width", width);
		}

		Writer& fill(uint32_t color)
		{
			return self().color_attribute("fill", color);
		}

		Writer& nofill()
		{
			return self().attribute("fill", "none");
		}

		Writer& end_primitive()
		{
			return self().element_end();
		}

		Writer& background(uint32_t color)
		{
			return self()
				.rect_start(view_x, view_y, view_width - view_x, view_height - view_y)
					.fill(color)
				.end_primitive();
		}

		Transform_writer transform()
		{
			return Transform_writer{ self(), strategy };
		}

		Writer& color_attribute(std::string_view attribute_name, uint32_t color)
		{
			return self().attribute(attribute_name, fmt::format("#{:0>6X}", color));
		}
	private:
		Writer& self() { return static_cast<Writer&>(*this); }
	};
}
