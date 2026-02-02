cmake_minimum_required(VERSION 3.30)

set(OBERON0_C "oberon0c")
project(${OBERON0_C} VERSION 0.0.1)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # warning level 4
  add_compile_options(/Wall /W4 /external:anglebrackets /external:W0)
else ()
  # lots of warnings and all warnings as errors -Wglobal-constructors -Wexit-time-destructors
  add_compile_options(-W -Wall -Wextra -Wpedantic -Wno-error=deprecated -Wunreachable-code -Winvalid-pch -Wcast-align -Wformat=2 -Wformat-nonliteral -Wmissing-declarations -Wmissing-noreturn -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wsign-conversion)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>)
  set(CMAKE_CXX_FLAGS_DEBUG "-g")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wweak-vtables)
    if (WIN32)
      add_compile_options(-fms-extensions)
    endif ()
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # using GCC
  endif ()
endif ()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

# define DEBUG macro
if(CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-D_DEBUG)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

# add the cmake folder of the project for extension modules
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(Boost_USE_MULTITHREADED ON)
set(Boost_NO_WARN_NEW_VERSIONS 1)

# add source tree to search path to avoid relative includes
include_directories(BEFORE .)

set(UTIL_SOURCES
        util/Logger.cpp util/Logger.h)

set(SCANNER_SOURCES
        scanner/Scanner.cpp scanner/Scanner.h
        scanner/Token.cpp scanner/Token.h
        scanner/LiteralToken.cpp scanner/LiteralToken.h
        scanner/IdentToken.cpp scanner/IdentToken.h
        scanner/UndefinedToken.cpp scanner/UndefinedToken.h)

set(PARSER_SOURCES
        parser/Parser.cpp parser/Parser.h
        parser/ast/ASTContext.h parser/ast/ASTContext.cpp
        parser/ast/DeclarationSequenceNode.h parser/ast/DeclarationSequenceNode.cpp
        parser/ast/ExpressionNode.h parser/ast/ExpressionNode.cpp
        parser/ast/IdentNode.h parser/ast/IdentNode.cpp
        parser/ast/ModuleNode.h parser/ast/ModuleNode.cpp
        parser/ast/Node.cpp parser/ast/Node.h
        parser/ast/NodeVisitor.h parser/ast/NodeVisitor.cpp
        parser/ast/StatementNode.h parser/ast/StatementNode.cpp
        parser/ast/StatementSequenceNode.h parser/ast/StatementSequenceNode.cpp
        parser/ast/TypeNode.h parser/ast/TypeNode.cpp
        parser/SymbolTable.h parser/SymbolTable.cpp
        parser/SemanticChecker.h parser/SemanticChecker.cpp
      )


set(ALL_SOURCES global.h ${UTIL_SOURCES} ${SCANNER_SOURCES} ${PARSER_SOURCES})

add_executable(${OBERON0_C} main.cpp ${ALL_SOURCES})

find_package(Boost REQUIRED COMPONENTS program_options)
if (Boost_FOUND)
  message(STATUS "Found Boost: ${Boost_VERSION}")
  target_link_libraries(${OBERON0_C} PRIVATE Boost::headers Boost::program_options)
endif ()

find_package(LLVM REQUIRED CONFIG)
if (LLVM_FOUND)
  if (${LLVM_VERSION} LESS 16.0.0)
    message(STATUS "Warning: found legacy LLVM: ${LLVM_VERSION}")
    add_definitions(-D_LLVM_LEGACY)
  else ()
    message(STATUS "Found LLVM: ${LLVM_VERSION}")
    if (${LLVM_VERSION} MATCHES "16.*.*")
      add_definitions(-D_LLVM_16)
    elseif (${LLVM_VERSION} MATCHES "17.*.*")
      add_definitions(-D_LLVM_17)
    elseif (${LLVM_VERSION} MATCHES "18.*.*")
      add_definitions(-D_LLVM_18)
    endif()
  endif ()
  add_definitions(${LLVM_DEFINITIONS})
  include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
  llvm_map_components_to_libnames(llvm_libs core support passes ${LLVM_TARGETS_TO_BUILD})
  target_link_libraries(${OBERON0_C} PRIVATE ${llvm_libs})
endif ()

configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

enable_testing()

set(CMAKE_CTEST_ARGUMENTS "--output-on-failure")

set(TEST_SOURCES
  tests/declaration_validity/const_decl.cpp
  tests/parser_tests.cpp
  tests/symbol_table_tests.cpp
  tests/semantic_checker/ident_use_tests.cpp
)

# Configure Unit Tests
find_package(Catch2 3 REQUIRED)
# These tests can use the Catch2-provided main
add_executable(oberon0c_tests ${TEST_SOURCES} ${ALL_SOURCES})
target_link_libraries(oberon0c_tests PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(oberon0c_tests  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/tests REPORTER "console::out=-::colour-mode=ansi")
