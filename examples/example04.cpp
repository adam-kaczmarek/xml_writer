#include <cstdlib>
#include <iostream>

#include <Xml_writer.h>

template<typename Writer> // final class is passed as a template parameter to allow using full functionality
class ExtensionA
{
public:
	Writer& func_a()
	{
		return self()
			.element("ExtensionA")
				.attribute("dummy", 'a')
			.element_end();
	}

private:
	Writer& self() { return static_cast<Writer&>(*this); }
};

template<typename Writer>
class ExtensionB
{
public:
	Writer& func_b()
	{
		return self()
			.func_a() // (*) might cause an error, because ExtensionA might not be attached. ExtensionC fixes that
			.element("ExtensionB")
				.attribute("attr", 'b')
			.element_end();
	}
private:
	Writer& self() { return static_cast<Writer&>(*this); }
};

template<typename Writer>
class ExtensionC
{
public:
	// declare required extensions, which will be automatically attached whenever this extensions is attached
	using required_extensions_t = aka::variadic::tpack<ExtensionA>;
	// more extensions might be added as in:
	// using required_extensions_t = aka::variadic::tpack<ExtensionA, ExtensionB>;

	Writer& func_c()
	{
		return self()
			.func_a() // (**) assured to work
			.element("ExtensionC")
				.attribute("attr", "custom")
			.element_end();

	}
private:
	Writer& self() { return static_cast<Writer&>(*this); }
};

void test_a()
{
	std::cout << "test_a: \n";

	using namespace aka::xml;
	Writer<std::ostream, ExtensionA> xml(std::cout);
	
	xml
		.declaration()
		.set_indenting(true)
		.element("NonExtension")
			.func_a()
		.element_end()
		.document_end();

	std::cout << '\n';
}

void test_b1()
{
	std::cout << "\ntest_b1: \n";

	using namespace aka::xml;
	Writer<std::ostream, ExtensionB> xml(std::cout);
	
	xml
		.declaration()
		.set_indenting(true)
		.element("NonExtension")
			//.func_b() // compile error, because ExtensionA is not at attached, but func_b calls func_a
		.element_end()
		.document_end();

	std::cout << '\n';
}

void test_b2()
{
	std::cout << "\ntest_b2: \n";

	using namespace aka::xml;
	Writer<std::ostream, ExtensionA, ExtensionB> xml(std::cout);
	
	xml
		.declaration()
		.set_indenting(true)
		.element("NonExtension")
			.func_b() // func_b doesn't cause compile error, because ExtensionA is explicitly attached
			.func_a()
		.element_end()
		.document_end();

	std::cout << '\n';
}

void test_c()
{
	std::cout << "\ntest_c: \n";

	using namespace aka::xml;
	Writer<std::ostream, ExtensionC> xml(std::cout);

	xml
		.declaration()
		.set_indenting(true)
		.element("NonExtension")
			.func_c() // ExtensionA is attached implicitly as a dependency of ExtensionC, so no compile error
			.func_a() // calling functions from ExtensionA works as well
		.element_end()
		.document_end();
	
	std::cout << '\n';
}

int main(int argc, char* argv[])
{
	test_a();
	test_b1();
	test_b2();
	test_c();

	return EXIT_SUCCESS;
}
