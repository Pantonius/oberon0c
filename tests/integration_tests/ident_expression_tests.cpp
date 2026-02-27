#include "./setup_mcjit.h"
#include "codegen/CodeGen.h"
#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test Ident Expression", "[full]") {

  SECTION("File Ident-Expression.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/Ident-Expression.Mod";
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
    auto i = (int *)i_addr;

    REQUIRE(*i == 8);
  }
}
