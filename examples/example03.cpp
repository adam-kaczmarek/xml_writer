#include <cstdlib>
#include <iostream>

#include <Xml_writer.h>
#include <Xml_writer_basic_extensions.h>
#include "Xml_writer_svg_extension.h"

uint32_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return (r << 16) | (g << 8) | b;
}

int main(int argc, char* argv[])
{
	using namespace aka::xml;
	using namespace aka::xml::svg_literals;

	Writer<std::ostream, Xml_writer_svg_extension> xml(std::cout);

	xml
		.set_indenting(true)
		.declaration(XML_DECLARATION_NO_STANDALONE)
		.svg_start(391, 391, -70.5, -70.5, 391.0, 391.0)
			.background(rgb(255, 128, 128))
			.element("comment") // using basic interface (SVG viewers should ignore unrecognized elements)
				.attribute("version", 1)
				.attribute("pi", 3.14)
			.element_end()
			.empty_element("empty")
			.rect_start(10, 10, 100, 100)
				.stroke(15, 2)
				.fill(rgb(0, 128, 0))
				.transform()
					.rotate(45)
					.translate(30, 20)
					.scale(2, 1.4)
				.end_transform()
			.end_primitive()
			.rect_start(10, 10, 100, 100)
				.stroke(rgb(0, 0, 255), 2)
				.nofill()
				.transform()
					.rotate_around(100, 100, 3.14_rad)
					.scale(0.5f)
				.end_transform()
			.end_primitive();

	for (int i = 0; i < 12; ++i)
	{
		const int radius = 150;
		xml
			.ellipse_start(0, 0, 10, 10)
				.stroke(rgb(0, 0, 255), 2)
				.fill(rgb(128, 128, 255))
				.transform()
					.rotate_around(0, radius, i * 30)
					.translate(200, 200 - radius)
				.end_transform()
			.end_primitive();
	}
	xml.document_end();

	std::cout << '\n';

	return EXIT_SUCCESS;
}
