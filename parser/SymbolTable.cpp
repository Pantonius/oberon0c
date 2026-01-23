#include "SymbolTable.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <cassert>
#include <exception>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

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

const DeclarationNode *SymbolTable::lookup(const std::string &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    if (auto node = scope.at(ident)) {
      return node;
    }
  }

  return {};
}

const DeclarationNode *SymbolTable::lookup(const IdentNode &ident) const {
  return this->lookup(ident.value);
}

const TypeNode *
SymbolTable::lookup_type(const IdentExpressionNode &ident_expr) {

  const DeclarationNode *decl_node = this->lookup(*ident_expr.ident);

  // Handle nullptr
  if (!decl_node) {
    logger_.error(ident_expr.ident->pos(),
                  ident_expr.ident->value + " is not declared!");
    throw NotDeclaredException(ident_expr);
    return {};
  }

  const TypeNode *type = decl_node->type;
  const Node *prev_selector = ident_expr.ident.get();
  for (unsigned long i = 0; i < ident_expr.selectors.size(); i++) {
    auto curr_selector = ident_expr.selectors.at(i).get();
    try {
      prev_selector = ident_expr.selectors.at(i - 1).get();
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
