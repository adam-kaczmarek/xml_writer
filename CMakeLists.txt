cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

project(aka_xml_writer
		VERSION 0.1
		LANGUAGES CXX)

add_library(aka_xml_writer INTERFACE)
target_include_directories(aka_xml_writer INTERFACE "include")

find_package(fmt)
target_link_libraries(aka_xml_writer INTERFACE fmt::fmt)

option(BUILD_EXAMPLES "comment" OFF)
if (BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()
