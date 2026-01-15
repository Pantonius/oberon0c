#include "SemanticChecker.h"
#include "ast/IdentNode.h"
#include "ast/ModuleNode.h"
#include <cstdlib>

void SemanticChecker::onModuleStart(const FilePos &pos,
                                    unique_ptr<IdentNode> ident) {
  auto module = make_unique<ModuleNode>(pos, std::move(ident));
  // TODO uniqueness check
  symbol_table_.beginScope();
  context_.set_module(std::move(module));
}

void SemanticChecker::onModuleEnd(const FilePos &pos,
                                  const unique_ptr<IdentNode> &ident) {
  if (context_.get_module()->ident->value != ident->value) {
    logger_.error(pos, "End identifier does not match module identifier.");
    exit(EXIT_FAILURE);
  }

  symbol_table_.endScope();
  // TODO check ident
}

unique_ptr<ConstDeclarationNode>
SemanticChecker::onConst(const FilePos &pos, unique_ptr<IdentNode> ident,
                         unique_ptr<ExpressionNode> expr) {

  // TODO CHECK expr is constant? type of that constant?
  if (!expr) {
    logger_.error(pos, "undefined constant value: " + ident->value);
    exit(EXIT_FAILURE);
  }

  if (!expr->is_const()) {
    logger_.error(pos,
                  "Non-constant value in const declaration: " + ident->value);
    exit(EXIT_FAILURE);
  }

  auto const_decl = make_unique<ConstDeclarationNode>(
      pos, std::move(ident), std::move(expr), expr ? expr->getType() : nullptr);
  expect_unique(const_decl->ident.get(), const_decl.get());

  return const_decl;
}

unique_ptr<VarDeclarationNode>
SemanticChecker::onVar(const FilePos &pos, unique_ptr<IdentNode> ident,
                       TypeNode *type) {
  if (!type) {
    logger_.error(pos, "Unspecified variable type: " + ident->value);
    exit(EXIT_FAILURE);
  }

  // if (!context_.find_type(type)) {
  //   logger_.error(pos, "Variable type is out of scope: " + ident->value);
  //   exit(EXIT_FAILURE);
  // }
  //
  auto var_decl =
      make_unique<VarDeclarationNode>(pos, std::move(ident), nullptr); // TODO

  expect_unique(var_decl->ident.get(), var_decl.get());

  return var_decl;
}

unique_ptr<ProcedureDeclarationNode> SemanticChecker::onProcedureStart(
    const FilePos &pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<FPSectionNode>> formal_paramenters) {
  // TODO type?
  auto proc = make_unique<ProcedureDeclarationNode>(
      pos, std::move(ident), formal_paramenters, nullptr);
  expect_unique(proc->ident.get(), proc.get());

  symbol_table_.beginScope();

  // TODO checks

  return proc;
}

void SemanticChecker::onProcedureEnd(const FilePos &pos,
                                     const ProcedureDeclarationNode *procedure,
                                     const unique_ptr<IdentNode> &ident) {
  if (procedure->ident->value != ident->value) {
    logger_.error(pos, "End identifier does not match procedure identifier.");
    exit(EXIT_FAILURE);
  }

  // TODO checks
  symbol_table_.endScope();
}

/* -------------------
 *  UTILITY FUNCTIONS
 * ------------------- */
void SemanticChecker::expect_unique(const IdentNode *ident, const Node *value) {
  if (symbol_table_.contains(ident->value)) {
    logger_.error(ident->pos(),
                  "Identifier already declared: " + ident->value + ".");
    exit(EXIT_FAILURE);
  }
  symbol_table_.insert(ident->value, value);
}
