set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

cmake_minimum_required (VERSION 2.8.11)
project (JavaCompilerFlex)

add_compile_options(-std=c++14) # CMake 2.8.12 or newer

find_package(BISON REQUIRED)
BISON_TARGET(MyParser grammer.ypp grammer.tab.cpp)

find_package(FLEX REQUIRED)
FLEX_TARGET(MyScanner grammer.lpp  grammer.cpp)

add_executable(Parser
    ${BISON_MyParser_OUTPUTS}
    ${FLEX_MyScanner_OUTPUTS}
    "types.h"
    "myfunctions.hpp"
    "myfunctions.cpp"
)
TARGET_LINK_LIBRARIES(Parser fl)













#include_directories("C:/Program Files (x86)/GnuWin32/include")
# LINK_DIRECTORIES("C:/Program Files (x86)/GnuWin32/lib")
# find_library(FOO_LIB libfl)
# LIST(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".a")
# TARGET_LINK_LIBRARIES(LexicalAnalyzerFlex libfl)
# add_library( libfl SHARED IMPORTED )
# set_target_properties( libfl PROPERTIES IMPORTED_LOCATION "C:/Program Files (x86)/GnuWin32/lib/libfl.a" )
