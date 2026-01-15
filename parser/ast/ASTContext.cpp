#include "ASTContext.h"
#include "TypeNode.h"

ModuleNode *ASTContext::get_module() { return module_.get(); }

void ASTContext::set_module(unique_ptr<ModuleNode> module) {
  module_ = std::move(module);
}

void ASTContext::add_type(unique_ptr<TypeNode> type) {
  types_.push_back(std::move(type));
}

TypeNode *ASTContext::find_type(IdentTypeNode *type) {
  for (size_t i = 0; i < types_.size(); i++) {
    if (type->getNodeType() == types_[i]->getNodeType()) {
      auto symb_type = dynamic_cast<const IdentTypeNode *>(types_[i].get());

      if (type->ident->value == symb_type->ident->value) {
        return types_[i].get();
      }
    }
  }

  return nullptr;
}

// TypeNode *ASTContext::find_type(ArrayTypeNode *type) {
//   for (size_t i = 0; i < types_.size(); i++) {
//     if (type->getNodeType() == types_[i]->getNodeType()) {
//       auto symb_type = dynamic_cast<const ArrayTypeNode *>(types_[i].get());
//
//       // TODO
//     }
//   }
//
//   return nullptr;
// }
//
// TypeNode *ASTContext::find_type(RecordTypeNode *type) {
//   for (size_t i = 0; i < types_.size(); i++) {
//     if (type->getNodeType() == types_[i]->getNodeType()) {
//       auto symb_type = dynamic_cast<const RecordTypeNode *>(types_[i].get());
//
//       // TODO
//     }
//   }
//
//   return nullptr;
// }
