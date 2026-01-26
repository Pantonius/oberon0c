#include "SymbolTable.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <cassert>
#include <cstdlib>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>

void SymbolTable::beginScope() {
  table_.push_back(std::unordered_map<std::string, const DeclarationNode *>());
};

void SymbolTable::endScope() { table_.pop_back(); }

void SymbolTable::insert(const IdentNode &ident, const DeclarationNode *node) {
  std::unordered_map<std::string, const DeclarationNode *> &current_scope =
      table_.back();

  if (current_scope.contains(ident.value)) {
    logger_.error(node->pos(),
                  "Identifier: \"" + ident.value + "\" already used!");
    return;
  }

  current_scope.insert({ident.value, node});
}

const DeclarationNode *SymbolTable::lookup(const IdentNode &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    try {
      auto node = scope.at(ident.value);
      return node;
    } catch (const std::out_of_range e) {
    }
  }

  return {};
}

const TypeNode *
SymbolTable::lookup_type(const IdentNode &ident,
                         const vector<unique_ptr<SelectorNode>> &selectors) {

  const DeclarationNode *decl_node = this->lookup(ident);

  // Handle nullptr
  if (!decl_node) {
    logger_.error(ident.pos(), ident.value + " is not declared!");
    throw NotDeclaredException(ident);
    return {};
  }

  const TypeNode *type = decl_node->type;
  const Node *prev_selector = &ident;
  for (unsigned long i = 0; i < selectors.size(); i++) {
    auto curr_selector = selectors.at(i).get();
    try {
      prev_selector = selectors.at(i - 1).get();
    } catch (std::out_of_range &e) {
    }

    if (auto array_selector =
            dynamic_cast<const ArrayIndexNode *>(curr_selector)) {
      if (auto array_type_node = dynamic_cast<const ArrayTypeNode *>(type)) {
        type = array_type_node->type;
      } else {

        logger_.error(prev_selector->pos(),
                      to_string(prev_selector) + " is not of type ARRAY");
        throw WrongTypeException(*prev_selector, "ARRAY");

        return {};
      }
    } else if (auto record_selector =
                   dynamic_cast<const RecordFieldNode *>(curr_selector)) {
      if (auto record_type_node = dynamic_cast<const RecordTypeNode *>(type)) {
        try {
          auto record_field =
              record_type_node->find_field(*record_selector->ident);
          type = record_field->type;
        } catch (FieldNotFoundException &e) {
          logger_.error(prev_selector->pos(), e.what());
          throw;
          return {};
        }
      } else {
        logger_.error(prev_selector->pos(),
                      to_string(prev_selector) + " is not of type RECORD");
        throw WrongTypeException(*prev_selector, "RECORD");
        return {};
      }
    } else {
      assert((void("Encountered unhandled SelectorNode type"), false));
    }
  }
  return type;
}
