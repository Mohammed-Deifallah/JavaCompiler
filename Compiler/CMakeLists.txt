# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.

file(GLOB lib_SRC
    "../lib/*.h"
    "../lib/*.cpp"
)

file(GLOB lib_lexical_SRC
    "../lib/lexical/*.h"
    "../lib/lexical/*.cpp"
)

file(GLOB lib_parser_SRC
    "../lib/parser/*.h"
    "../lib/parser/*.cpp"
)

file(GLOB rapidjson_SRC
    "../rapidjson/*.h"
    "../rapidjson/*.cpp"
)

file(GLOB here_SRC
    "*.h"
    "*.cpp"
)

add_executable(Compiler ${here_SRC} ${lib_SRC} ${rapidjson_SRC}  ${lib_lexical_SRC} ${lib_parser_SRC} )
include_directories("../lib" "../lib/lexical" "../lib/parser")

set_target_properties( Compiler
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"

    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
    
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
)
