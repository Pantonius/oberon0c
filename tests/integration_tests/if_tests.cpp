#include "./setup_mcjit.h"
#include "codegen/CodeGen.h"
#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cstdio>
#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <memory>

TEST_CASE("Test If"
          "[full]") {

  SECTION("File IfStatement.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/IfStatement.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto module = setup_llvm_module();

    CodeGenBuilder builder(logger, *module);
    builder.build(*ast);

    llvm::Function *main = module->getFunction("main");

    if (!main) {
      FAIL("No main function found in module");
    }

    auto var_i = module->getGlobalVariable("i", true);

    if (!var_i) {
      FAIL("No global variable i");
    }

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto i_addr = exec_engine->getGlobalValueAddress("i");
    auto i = (bool *)i_addr;

    REQUIRE(*i == true);
  }

  SECTION("File IfElseStatement.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/IfElseStatement.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto module = setup_llvm_module();

    CodeGenBuilder builder(logger, *module);
    builder.build(*ast);

    llvm::Function *main = module->getFunction("main");

    if (!main) {
      FAIL("No main function found in module");
    }

    auto var_i = module->getGlobalVariable("i", true);

    if (!var_i) {
      FAIL("No global variable i");
    }

    auto var_res = module->getGlobalVariable("res", true);

    if (!var_res) {
      FAIL("No global variable res");
    }

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto i_addr = exec_engine->getGlobalValueAddress("i");
    auto i = (int *)i_addr;
    auto res_addr = exec_engine->getGlobalValueAddress("res");
    auto res = (bool *)res_addr;

    REQUIRE(*i == 1);
    REQUIRE(*res == false);
  }

  SECTION("File IfElsifElseStatement.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/IfElsifElseStatement.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto module = setup_llvm_module();

    CodeGenBuilder builder(logger, *module);
    builder.build(*ast);

    llvm::Function *main = module->getFunction("main");

    if (!main) {
      FAIL("No main function found in module");
    }

    auto var_i = module->getGlobalVariable("i", true);

    if (!var_i) {
      FAIL("No global variable i");
    }

    auto var_res = module->getGlobalVariable("res", true);

    if (!var_res) {
      FAIL("No global variable res");
    }

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto i_addr = exec_engine->getGlobalValueAddress("i");
    auto i = (int *)i_addr;
    auto res_addr = exec_engine->getGlobalValueAddress("res");
    auto res = (int *)res_addr;

    REQUIRE(*i == 1);
    REQUIRE(*res == 42);
  }

  SECTION("File NestedIfStatements.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/NestedIfStatements.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto module = setup_llvm_module();

    CodeGenBuilder builder(logger, *module);
    builder.build(*ast);

    llvm::Function *main = module->getFunction("main");

    if (!main) {
      FAIL("No main function found in module");
    }

    auto var_i = module->getGlobalVariable("i", true);

    if (!var_i) {
      FAIL("No global variable i");
    }

    auto var_res = module->getGlobalVariable("res", true);

    if (!var_res) {
      FAIL("No global variable res");
    }

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto i_addr = exec_engine->getGlobalValueAddress("i");
    auto i = (int *)i_addr;
    auto res_addr = exec_engine->getGlobalValueAddress("res");
    auto res = (int *)res_addr;

    REQUIRE(*i == 2);
    REQUIRE(*res == 100);
  }
}
