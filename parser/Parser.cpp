#include "Parser.h"

void Parser::parse() { module(); }

/* ident = letter {letter | digit} */
const string Parser::ident() {
  curr_token_ = scanner_.next();
  letter(curr_token_);
}

/* module = "MODULE" ident ";"
 *          [ ImportList ]
 *          DeclarationSequence
 *          [ "BEGIN" StatementSequence ]
 *          "END" ident "." */
// TODO
void Parser::module() {}

/* DeclarationSequence =
 *          { "CONST" { ConstDeclaration ";" }
 *          | "TYPE" { TypeDeclaration ";" }
 *          | "VAR" { VarDeclaration ";" }
 *          | { ProcedureDeclaration ";" } */
// TODO

/* ImportList = "IMPORT" import {"," import} ";" */
// TODO

/* Import = ident [":=" ident] */
// TODO

/* qualident = [ident "."] ident. */
// TODO

/* designator = qualident. */
// TODO

/* ProcedureHeading =
 *        "PROCEDURE" ident ["*"] FormalParameters */
// TODO

/* FormalParameters =
 *        "(" [FPSection {";" FPSection}] ")" ":"qualident */
// TODO

/* FPSection = ["VAR"] ident {"," ident} ":" FormalType */
// TODO

/* FormalType = qualident */
// TODO

/* ReturnStatement = "RETURN" [expression] */
// TODO

/* FunctionCall = designator "(" [ActualParameters] ")" */
// TODO

/* ActualParameters = expression {"," expression} */
// TODO
