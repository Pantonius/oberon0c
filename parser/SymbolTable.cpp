#include "SymbolTable.h"
#include <ranges>
#include <string>
#include <unordered_map>

void SymbolTable::beginScope() {
  table_.push_back(std::unordered_map<std::string, const Node *>());
};

void SymbolTable::endScope() { table_.pop_back(); }

void SymbolTable::insert(const std::string &ident, const Node *node) {
  std::unordered_map<std::string, const Node *> &current_scope = table_.back();

  if (current_scope.contains(ident)) {
    logger_.error(node->pos(), "Identifier: \"" + ident + "\" already used!");
    return;
  }

  current_scope.insert({ident, node});
}

const Node *SymbolTable::lookup(const std::string &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    if (scope.contains(ident)) {
      return scope.at(ident);
    }
  }

  return {};
}

bool SymbolTable::contains(const std::string &ident) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    if (scope.contains(ident)) {
      return true;
    }
  }

  return false;
}
