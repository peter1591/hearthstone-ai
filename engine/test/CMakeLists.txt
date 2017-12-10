SET(OUTPUT_DIR "" CACHE STRING "output directory")

project (hello-cmake)

cmake_minimum_required(VERSION 3.7)

add_compile_options(
	/std:c++latest
	/bigobj
)

include_directories(
	../../contrib/lib_json
	../../include
)

configure_file(
	../../include/Cards/cards.json
	./${OUTPUT_DIR}/cards.json
	COPYONLY
)

add_executable(hello-cmake 
	../../contrib/lib_json/json_reader.cpp
	../../contrib/lib_json/json_writer.cpp
	../../contrib/lib_json/json_value.cpp
	CardDispatcher.cpp
	main.cpp
	test2.cpp
	test3.cpp
	test4.cpp
)