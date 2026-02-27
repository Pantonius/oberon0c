#include "ASTContext.h"
#include "TypeNode.h"
#include "global.h"
#include "parser/ast/IdentNode.h"
#include <memory>
#include <unordered_set>

const std::unordered_map<std::string, StdTypeNode *const>
    ASTContext::std_types = {
        {"BOOLEAN",
         new StdTypeNode(EMPTY_POS,
                         std::make_unique<IdentNode>(EMPTY_POS, "BOOLEAN"),
                         StdType::BOOLEAN)},
        {"INTEGER",
         new StdTypeNode(EMPTY_POS,
                         std::make_unique<IdentNode>(EMPTY_POS, "INTEGER"),
                         StdType::INTEGER)}};

StdTypeNode *const ASTContext::BOOLEAN = ASTContext::std_types.at("BOOLEAN");
StdTypeNode *const ASTContext::INTEGER = ASTContext::std_types.at("INTEGER");

ModuleNode *ASTContext::get_module() { return module_.get(); }

void ASTContext::set_module(unique_ptr<ModuleNode> module) {
  module_ = std::move(module);
}

IdentTypeNode *ASTContext::add_type(unique_ptr<IdentTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
ArrayTypeNode *ASTContext::add_type(unique_ptr<ArrayTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
RecordTypeNode *ASTContext::add_type(unique_ptr<RecordTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
ProcedureTypeNode *ASTContext::add_type(unique_ptr<ProcedureTypeNode> type) {
  auto ptr = type.get();
  types_.push_back(std::move(type));

  return ptr;
}
