#include "./setup_mcjit.h"
#include "codegen/CodeGen.h"
#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <algorithm>
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iterator>
#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

TEST_CASE("Test Program"
          "[full]") {

  SECTION("File Sort0.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/Sort0.Mod";
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

    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});

    auto a_addr = exec_engine->getGlobalValueAddress("a");
    auto a = (int (*)[20])a_addr;

    int a_exp[20];

    for (int i = 0; i < 20; i++) {
      a_exp[i] = i + 1;
    }

    REQUIRE((*a)[5] == 6);
    REQUIRE(std::ranges::equal(*a, a_exp));
  }
}
