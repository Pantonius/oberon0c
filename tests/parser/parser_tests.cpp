#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test parser"
          "[parser]") {

  Logger logger;
  SECTION("file Sort0.Mod") {
    Logger logger;
    const path path = "./resources/Sort0.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
  }

  SECTION("Parse boolean literal") {
    const path path = "./parser/resources/Boolean-Literal.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
  }
}
