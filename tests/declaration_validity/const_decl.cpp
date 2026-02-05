#include "parser/Parser.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>

TEST_CASE("Test Const Declarations", "[sema][const_decl]") {
  SECTION("file Simple.Mod") {
    Logger logger;
    const path path = "./declaration_validity/const_decl/Simple.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    // TODO check for actual const decl value
  }

  SECTION("file ConstFold.Mod") {
    Logger logger;
    const path path = "./declaration_validity/const_decl/ConstFold.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    // TODO check for actual const decl value
  }

  SECTION("file ConstFoldSubst.Mod") {
    Logger logger;
    const path path = "./declaration_validity/const_decl/ConstFoldSubst.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    // TODO check for actual const decl values
  }

  // TODO make this work
  // SECTION("file NonConst1.Mod") {
  //   Logger logger;
  //   Scanner scanner("./declaration_validity/const_decl/NonConst1.Mod",
  //   logger);
  //
  //   Parser parser(scanner, logger);
  //
  //   parser.parse();
  //
  //   REQUIRE(logger.getErrorCount() == 1);
  // }
}
