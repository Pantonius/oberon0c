#include "parser/Parser.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test Const Declarations", "[sema][const_decl]") {
  SECTION("file Simple.Mod") {
    Logger logger;
    Scanner scanner("./declaration_validity/const_decl/Simple.Mod", logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    // TODO check for actual const decl value
  }

  SECTION("file ConstFold.Mod") {
    Logger logger;
    Scanner scanner("./declaration_validity/const_decl/ConstFold.Mod", logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    // TODO check for actual const decl value
  }

  SECTION("file ConstFoldSubst.Mod") {
    Logger logger;
    Scanner scanner("./declaration_validity/const_decl/ConstFoldSubst.Mod",
                    logger);

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
