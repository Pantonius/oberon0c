#include "SymbolTable.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

void SymbolTable::beginScope() {
  table_.push_back(
      std::unordered_map<std::string,
                         std::pair<const IdentNode, const TypeNode *>>());
};

void SymbolTable::endScope() { table_.pop_back(); }

void SymbolTable::insert(const IdentNode &ident, const TypeNode *node) {
  std::unordered_map<std::string, std::pair<const IdentNode, const TypeNode *>>
      &current_scope = table_.back();

  if (current_scope.contains(ident.value)) {
    logger_.error(node->pos(),
                  "Identifier: \"" + ident.value + "\" already used!");
    return;
  }

  current_scope.insert({ident.value, {ident, node}});
}

const TypeNode *SymbolTable::lookup(const std::string &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    if (scope.contains(ident)) {
      return scope.at(ident).second;
    }
  }

  return {};
}

const TypeNode *
SymbolTable::lookup(const IdentNode &ident,
                    const vector<unique_ptr<SelectorNode>> &selectors) {

  const TypeNode *type_node;
  // WARN: Should either be a reference or a smart pointer
  const Node *to_check = &ident;

  for (auto &scope : std::ranges::views::reverse(table_)) {

    try {
      type_node = scope.at(ident.value).second;
    } catch (std::out_of_range e) {
      return {};
    }

    for (auto &selector : selectors) {
      if (auto array_selector =
              dynamic_cast<const ArrayIndexNode *>(selector.get())) {
        if (auto array_type_node =
                dynamic_cast<const ArrayTypeNode *>(type_node)) {
          type_node = array_type_node->type;
          to_check = array_selector;
        } else {
          // TODO: Handle type error!

          stringstream ss;
          string s_node;
          to_check->print(ss);
          ss >> s_node;
          logger_.error(to_check->pos(), s_node + " is not of type ARRAY");

          return {};
        }
      } else if (auto record_selector =
                     dynamic_cast<const RecordFieldNode *>(selector.get())) {
        if (auto record_type_node =
                dynamic_cast<const RecordTypeNode *>(type_node)) {
          if (auto record_field =
                  record_type_node->find_field(*record_selector->ident)) {
            type_node = record_field->type;
            to_check = record_selector;
          } else {
            // TODO: Handle type error!

            stringstream ss;
            string s_node;
            to_check->print(ss);
            ss >> s_node;
            logger_.error(to_check->pos(),
                          s_node + " is not of type RECORD or missing field '" +
                              record_selector->ident->value + "'");

            return {};
          }
        }
      }
    }
  }
  return type_node;
}
