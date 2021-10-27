#include <cstdlib>
#include <iostream>

#include <Xml_writer.h>
#include <Xml_writer_basic_extensions.h>

int main(int argc, char* argv[])
{
	using namespace aka::xml;

	Writer<std::ostream, Basic_extension> xml(std::cout);
	
	xml
		.set_indenting(true)
		.declaration()
		.element("root")
			.attribute("version", 0.1)
			.element("child")
				.attribute("intParam", 2)
				.attribute("doubleParam", 3.14)
				.attribute("stringParam", "value")
				.empty_element("deeperElement") // usage of extension method. chaining still works
				.text("mixed content")
				.attribute_element("deeperElement", "id", 2) // another method from extension
			.element_end()
		.element_end()
		.document_end();

	std::cout << '\n';

	return EXIT_SUCCESS;
}
