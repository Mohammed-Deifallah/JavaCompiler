
# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (JavaCompiler)

add_compile_options(-std=c++14) # CMake 2.8.12 or newer

include_directories(./)

# Recurse into the "Hello" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
add_subdirectory (LexicalAnalyzer)
add_subdirectory (LexicalAnalyzerGenerator)
add_subdirectory (ParserGenerator)
add_subdirectory (Compiler)

