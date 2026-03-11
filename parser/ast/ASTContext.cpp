#include "ASTContext.h"
#include "TypeNode.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/IdentNode.h"
#include <memory>

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

// const FilePos pos, unique_ptr<IdentNode> proc_name,
//                            ProcedureTypeNode *const type_node
const std::unordered_map<StdProc, ProcedureDeclarationNode *const>
    ASTContext::std_procs = {
        {StdProc::WRITE_INT,
         new ProcedureDeclarationNode(
             EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "WriteInt"),
             new ProcedureTypeNode(
                 EMPTY_POS,
                 [] {
                   vector<unique_ptr<ParamDeclarationNode>> params;
                   params.push_back(std::make_unique<ParamDeclarationNode>(
                       EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "val"),
                       false, ASTContext::INTEGER));
                   return params;
                 }()))},
        {StdProc::WRITE_LN,
         new ProcedureDeclarationNode(
             EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "WriteLn"),
             new ProcedureTypeNode(EMPTY_POS, {}))},
};

ProcedureDeclarationNode *const ASTContext::WRITE_INT =
    ASTContext::std_procs.at(StdProc::WRITE_INT);
ProcedureDeclarationNode *const ASTContext::WRITE_LN =
    ASTContext::std_procs.at(StdProc::WRITE_LN);

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
