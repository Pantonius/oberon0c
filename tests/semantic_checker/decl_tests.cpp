#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include "util/Logger.h"
#include <catch2/catch_test_macros.hpp>
#include <string>
#include <sys/wait.h>
#include <unistd.h>

namespace {

const std::string RESOURCES = "./semantic_checker/resources/";

// Parse `file` in this process. Only safe for inputs that are known to recover.
void parse(Logger &logger, const std::string &file) {
  const path path = RESOURCES + file;
  Scanner scanner(path, logger);
  Parser parser(scanner, logger);

  parser.parse();
}

enum class Outcome {
  recovered, // parse returned; errors were logged and execution went on
  hard_exit, // a code path called exit(EXIT_FAILURE)
  uncaught_exception, // an exception escaped parse()
  crashed,            // killed by a signal
};

std::ostream &operator<<(std::ostream &stream, const Outcome &outcome) {
  switch (outcome) {
  case Outcome::recovered:
    return stream << "recovered";
  case Outcome::hard_exit:
    return stream << "hard_exit";
  case Outcome::uncaught_exception:
    return stream << "uncaught_exception";
  case Outcome::crashed:
    return stream << "crashed";
  }
  return stream << "?";
}

// Parse `file` in a forked child.
//
// Several declaration errors still terminate the process rather than recover,
// which would take the whole Catch2 binary down with them. Forking turns that
// into an assertable outcome, so these cases can be regression tests while the
// error-recovery work is in progress.
Outcome parse_isolated(const std::string &file) {
  const pid_t pid = fork();
  REQUIRE(pid >= 0);

  if (pid == 0) {
    Logger logger;
    try {
      parse(logger, file);
    } catch (...) {
      _exit(2);
    }
    _exit(0);
  }

  int status = 0;
  REQUIRE(waitpid(pid, &status, 0) == pid);

  if (WIFSIGNALED(status)) {
    return Outcome::crashed;
  }
  switch (WEXITSTATUS(status)) {
  case 0:
    return Outcome::recovered;
  case 2:
    return Outcome::uncaught_exception;
  default:
    return Outcome::hard_exit;
  }
}

} // namespace

// Undeclared identifiers the semantic checker recovers from: exactly one error
// per undeclared name, and no diagnostics derived from the missing type.
TEST_CASE("Sema Undeclared Identifier Recovery", "[sema][decl]") {
  Logger logger;

  SECTION("case selector") {
    parse(logger, "Undecl.Mod");
    REQUIRE(logger.getErrorCount() == 1);
    REQUIRE(logger.getWarningCount() == 0);
  }

  SECTION("operand of an arithmetic expression") {
    parse(logger, "UndeclExpr.Mod");
    REQUIRE(logger.getErrorCount() == 1);
    REQUIRE(logger.getWarningCount() == 0);
  }

  SECTION("assignment target") {
    parse(logger, "UndeclAssign.Mod");
    REQUIRE(logger.getErrorCount() == 1);
    REQUIRE(logger.getWarningCount() == 0);
  }

  // both operands are reported; neither is silenced by the other
  SECTION("both operands of an arithmetic expression") {
    parse(logger, "UndeclMulti.Mod");
    REQUIRE(logger.getErrorCount() == 2);
    REQUIRE(logger.getWarningCount() == 0);
  }

  // the recovery pattern carries a placeholder variant index, so exhaustiveness
  // must stay quiet rather than report duplicate/missing cases
  SECTION("sum type qualifier in a variant pattern") {
    parse(logger, "UndeclVariant.Mod");
    REQUIRE(logger.getErrorCount() == 1);
    REQUIRE(logger.getWarningCount() == 0);
  }
}

// Declaration errors that do NOT recover yet. These fail until the remaining
// exit(EXIT_FAILURE) paths are converted to log-and-continue.
TEST_CASE("Sema Declaration Error Recovery", "[sema][decl]") {
  // onVars calls exit(EXIT_FAILURE) when onIdentType returns no type
  SECTION("undeclared type in a variable declaration") {
    REQUIRE(parse_isolated("UndeclType.Mod") == Outcome::recovered);
  }

  // expect_unique throws DuplicateFieldException, which nothing catches
  SECTION("duplicate variable declaration") {
    REQUIRE(parse_isolated("DuplicateVar.Mod") == Outcome::recovered);
  }
  SECTION("duplicate variable declaration with mismatching type") {
    REQUIRE(parse_isolated("DuplicateVarMismatchingType.Mod") ==
            Outcome::recovered);
  }

  // onProcedureCallStart calls exit(EXIT_FAILURE)
  SECTION("call to an undeclared procedure") {
    REQUIRE(parse_isolated("UndeclProc.Mod") == Outcome::recovered);
  }
}
