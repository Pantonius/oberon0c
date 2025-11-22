/*
 * Parser of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include "ast/Node.h"
#include "scanner/Scanner.h"
#include <string>

using std::string;

class Parser {

private:
  Scanner &scanner_;
  Token &curr_token_;
  Logger &logger_;

  const string ident();

  void module();
  void procedure();

public:
  explicit Parser(Scanner &scanner, Logger &logger)
      : scanner_(scanner), logger_(logger) {};
  ~Parser() = default;
  void parse();
};

#endif // OBERON0C_PARSER_H
