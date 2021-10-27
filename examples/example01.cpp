#include <cstdlib>
#include <iostream>

#include <Xml_writer.h>

int main(int argc, char* argv[])
{
	using namespace aka::xml;
	
	// first template parameter deduced as std::ostream
	Writer writer(std::cout);
	
	writer
		.set_indenting(true)
		.declaration()
		.element("root")
			.attribute("version", 0.1)
			.element("child")
				.attribute("intParam", 2)
				.attribute("doubleParam", 3.14)
				.attribute("stringParam", "value")
				.element("deeperElement").element_end()
				.text("mixed content")
				.element("deeperElement")
					.attribute("id", 2)
				.element_end()
			.element_end()
		.element_end()
		.document_end(); // close any opened elements

	std::cout << '\n';

	return EXIT_SUCCESS;
}
