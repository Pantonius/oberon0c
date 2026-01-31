#include "SymbolTable.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <cassert>
#include <cstdlib>
#include <exception>
#include <optional>
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
    return;
  }

  current_scope.insert({ident.value, node});
}

std::optional<const DeclarationNode *>
SymbolTable::lookup(const IdentNode &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    try {
      auto node = scope.at(ident.value);
      return node;
    } catch (const std::exception &e) {
    }
  }
  return {};
}

const TypeNode *
SymbolTable::lookup_type(const IdentNode &ident,
                         const vector<unique_ptr<SelectorNode>> &selectors) {

  auto lookup_node = this->lookup(ident);

  // Handle nullptr
  if (!lookup_node) {
    logger_.error(ident.pos(), ident.value + " is not declared!");
    throw NotDeclaredException(ident);
    return {};
  }

  auto decl_node = lookup_node.value();

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
        if (!array_type_node->is_in_bounds(array_selector->expression.get())) {
          throw OutOfRangeException(*array_selector);
        }
        type = array_type_node->type;
      } else {
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
          throw;
          return {};
        }
      } else {
        throw WrongTypeException(*prev_selector, "RECORD");
        return {};
      }
    } else {
      assert((void("Encountered unhandled SelectorNode type"), false));
    }
  }
  return type;
}
