#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Test Case Exhaustiveness"
          "[sema][case]") {
  Logger logger;
  SECTION("file CaseInt.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseInt.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 0);
  }
  SECTION("file CaseIntNonExhaustive.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseIntNonExhaustive.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 1);
  }
  SECTION("file CaseBool.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseBool.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 0);
  }
  SECTION("file CaseBoolNonExhaustive.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseBoolNonExhaustive.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 1);
  }
  SECTION("file CaseSum.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseSum.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 0);
  }
  SECTION("file CaseSumNonExhaustiveVariant.Mod") {
    Logger logger;
    const path path =
        "./semantic_checker/resources/CaseSumNonExhaustiveVariant.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 1);
  }
  SECTION("file CaseSumNonExhaustiveParam.Mod") {
    Logger logger;
    const path path =
        "./semantic_checker/resources/CaseSumNonExhaustiveParam.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 1);
  }
  SECTION("file CaseSumNonExhaustive.Mod") {
    Logger logger;
    const path path = "./semantic_checker/resources/CaseSumNonExhaustive.Mod";
    Scanner scanner(path, logger);

    Parser parser(scanner, logger);

    parser.parse();

    REQUIRE(logger.getErrorCount() == 0);
    REQUIRE(logger.getWarningCount() == 2);
  }
}
