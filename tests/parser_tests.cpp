#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test parsing correct syntax"
          "[parser]") {

  SECTION("file Sort0.Mod") {
    Logger logger;
    Scanner scanner("./resources/Sort0.Mod", logger);

    Parser parser(scanner, logger);

    parser.parse();
    //
    // REQUIRE(logger.getErrorCount() == 0);
  }
}

// TEST_CASE("Test syntax errors"
//           "[parser]") {
//
//   SECTION("missing semicolon") {
//
//     Logger logger;
//     Scanner scanner("./resources/Missing-Semicolon.Mod", logger);
//
//     Parser parser(scanner, logger);
//
//     parser.parse();
//
//     REQUIRE(logger.getErrorCount() >= 1);
//   }
// }
