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

TEST_CASE("Test Assignments"
          "[full]") {

  SECTION("File Assignment.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/Assignment.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto ctx = std::make_unique<llvm::LLVMContext>();
    auto module = std::make_unique<llvm::Module>("test", *ctx);

    llvm::DataLayout data_layout = llvm::DataLayout();
    module->setDataLayout(data_layout);

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
    auto i = (int *)i_addr;

    REQUIRE(*i == 1);

    struct R {
      bool f0;
      int f1;
      bool operator==(const R &) const = default;
    };

    R r = {
        true,
        5,
    };

    auto r0_addr = exec_engine->getGlobalValueAddress("r0");
    auto r0 = (R *)r0_addr;

    auto r1_addr = exec_engine->getGlobalValueAddress("r1");
    auto r1 = (R *)r1_addr;

    REQUIRE(*r0 == *r1);
    REQUIRE(*r0 == r);
  }
}
