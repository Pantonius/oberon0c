#include "./setup_mcjit.h"
#include "codegen/CodeGen.h"
#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_message.hpp>
#include <catch2/catch_test_macros.hpp>
#include <llvm/ADT/APInt.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>

TEST_CASE("testProcedureDeclaration"
          "[full]") {

  SECTION("File Sort0.Mod") {
    Logger logger(LogLevel::DEBUG, std::cout, std::cerr);
    const path path = "./integration_tests/resources/ProcedureDeclaration.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    auto ast = parser.parse();

    auto module = setup_llvm_module();

    CodeGenBuilder builder(logger, *module);
    builder.build(*ast);

    llvm::Function *q_sort = module->getFunction("QSort");

    if (!q_sort) {
      FAIL("No function 'QSort' found in module");
    }

    llvm::Function *main = module->getFunction("main");

    if (!q_sort) {
      FAIL("No main function found in module");
    }
    auto exec_engine = create_jit(std::move(module));

    exec_engine->runFunction(main, {});
  }
}
