#include "codegen/CodeGen.h"
#include "init_llvm.h"
#include "parser/Parser.h"
#include "parser/ast/ASTContext.h"
#include "scanner/Scanner.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("CodeGen Literal Expression", "[code_gen]") {
  Logger logger = Logger();

  const path path = "./code_gen/resources/Literal-Expressions.Mod";
  Scanner scanner(path, logger);

  Parser parser(scanner, logger);

  auto ast = unique_ptr<ASTContext>(parser.parse());

  static auto ctx = std::make_unique<llvm::LLVMContext>();
  static auto module = std::make_unique<llvm::Module>("test", *ctx);

  auto builder = CodeGenBuilder(logger, *module);

  builder.build(*ast);

  auto const_a = module->getNamedValue("a");
}
