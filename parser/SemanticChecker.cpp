#include "SemanticChecker.h"
#include "ast/IdentNode.h"
#include "ast/ModuleNode.h"
#include "parser/ast/TypeNode.h"
#include <cstdlib>
#include <memory>

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
}

unique_ptr<ConstDeclarationNode>
SemanticChecker::onConst(const FilePos &pos, unique_ptr<IdentNode> ident,
                         unique_ptr<ExpressionNode> expr) {

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
  expect_unique(const_decl->ident.get(), const_decl->type);

  return const_decl;
}

vector<unique_ptr<VarDeclarationNode>>
SemanticChecker::onVars(const FilePos &pos,
                        vector<unique_ptr<IdentNode>> idents,
                        const TypeNode *type) {
  if (!type) {
    logger_.error(pos, "Unspecified variable type.");
    exit(EXIT_FAILURE);
  }

  vector<unique_ptr<VarDeclarationNode>> var_decls;
  for (auto &ident : idents) {
    auto var_decl =
        make_unique<VarDeclarationNode>(pos, std::move(ident), type);

    expect_unique(var_decl->ident.get(), var_decl->type);
  }

  return var_decls;
}

unique_ptr<TypeDeclarationNode> SemanticChecker::onTypeDeclaration(
    const FilePos &pos, unique_ptr<IdentNode> ident, const TypeNode *type) {
  auto type_declaration =
      make_unique<TypeDeclarationNode>(pos, std::move(ident), type);

  expect_unique(type_declaration->ident.get(), type_declaration->type);

  return type_declaration;
}

const TypeNode *SemanticChecker::onIdentType(const FilePos &pos,
                                             unique_ptr<IdentNode> ident) {
  for (size_t i = 0;
       i < sizeof(context_.STD_TYPES) / sizeof(context_.STD_TYPES[0]); i++) {
    if (ident->value == context_.STD_TYPES[i]->ident->value)
      return context_.STD_TYPES[i].get();
  }

  TypeDeclarationNode const *type_decl =
      dynamic_cast<const TypeDeclarationNode *>(
          symbol_table_.lookup(ident->value));

  if (!type_decl) {
    logger_.error(pos, "Specified type \"" + ident->value +
                           "\" is not declared in the current scope.");
    exit(EXIT_FAILURE);
  }

  return type_decl->type;
}

const ArrayTypeNode *SemanticChecker::onArrayType(
    const FilePos &pos, unique_ptr<ExpressionNode> expr, const TypeNode *type) {
  if (!expr) {
    logger_.error(pos, "Undefined array size.");
    exit(EXIT_FAILURE);
  }

  if (!expr->is_const()) {
    logger_.error(expr->pos(), "Array size is not constant.");
    exit(EXIT_FAILURE);
  }

  auto array_type = std::make_unique<ArrayTypeNode>(pos, std::move(expr), type);
  auto ptr = context_.add_type(std::move(array_type));

  return ptr;
}

const RecordTypeNode *
SemanticChecker::onRecordType(const FilePos &pos,
                              vector<unique_ptr<FieldNode>> fields) {
  auto record_type = std::make_unique<RecordTypeNode>(pos, fields);
  // TODO check fields?

  auto ptr = context_.add_type(std::move(record_type));

  return ptr;
}

unique_ptr<ProcedureDeclarationNode> SemanticChecker::onProcedureStart(
    const FilePos &pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<FPSectionNode>> formal_paramenters) {
  // TODO type?
  auto proc = make_unique<ProcedureDeclarationNode>(
      pos, std::move(ident), std::move(formal_paramenters),
      ASTContext::STD_PROCEDURE_TYPE.get());
  expect_unique(proc->ident.get(), proc->type);

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
void SemanticChecker::expect_unique(const IdentNode *ident,
                                    const TypeNode *value) {
  if (symbol_table_.lookup(*ident)) {
    logger_.error(ident->pos(),
                  "Identifier already declared: " + ident->value + ".");
    exit(EXIT_FAILURE);
  }
  symbol_table_.insert(*ident, value);
}
