/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include "parser/Parser.h"
#include "scanner/Scanner.h"
#include <iostream>
#include <string>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

int main(const int argc, const char *argv[]) {
  // =================
  // Some setup stuff
  // =================
  if (argc != 2) {
    cerr << "Usage: oberon0c <filename>" << endl;
    exit(1);
  }
  path filename = argv[1];

  // Logging
  Logger logger;
  logger.setLevel(LogLevel::DEBUG);

  // =============
  // Actual Logic
  // =============
  // Scanning
  Scanner scanner(filename, logger);
  // TODO remove
  // auto token = scanner.next();
  // while (token->type() != TokenType::eof) {
  //   // cout << *token << endl; // TODO remove
  //   token = scanner.next();
  // }
  // // cout << *token << endl; // TODO remove

  // Parsing
  Parser parser(scanner, logger);
  auto ast = parser.parse();
  ast->print(cout);

  // Status print
  string status = (logger.getErrorCount() == 0 ? "complete" : "failed");
  logger.info("Compilation " + status + ": " +
              to_string(logger.getErrorCount()) + " error(s), " +
              to_string(logger.getWarningCount()) + " warning(s), " +
              to_string(logger.getInfoCount()) + " message(s).");
  exit(logger.getErrorCount() != 0);
}
