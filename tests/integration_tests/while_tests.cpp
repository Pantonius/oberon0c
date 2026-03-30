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

TEST_CASE("Test While"
          "[full]") {

  SECTION("File WhileStatement.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/WhileStatement.Mod";
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

    auto var_j = module->getGlobalVariable("j", true);

    if (!var_j) {
      FAIL("No global variable i");
    }

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto i_addr = exec_engine->getGlobalValueAddress("i");
    auto i = (int *)i_addr;

    auto j_addr = exec_engine->getGlobalValueAddress("j");
    auto j = (int *)j_addr;

    REQUIRE(*i == 11);
    REQUIRE(*j == 55);
  }
}
