#include "SymbolTable.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/TypeNode.h"
#include <cassert>
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
SymbolTable::lookup(const IdentNode &ident, bool this_scope) const {
  for (auto &scope : std::ranges::views::reverse(table_)) {
    try {
      auto node = scope.at(ident.value);
      return node;
    } catch (const std::exception &e) {
    }
    if (this_scope)
      break;
  }
  return {};
}

TypeNode *
SymbolTable::lookup_type(const IdentNode &ident,
                         const vector<unique_ptr<SelectorNode>> &selectors) {

  auto lookup_node = this->lookup(ident);

  // Handle nullptr
  if (!lookup_node) {
    throw NotDeclaredException(ident);
    return {};
  }

  auto decl_node = lookup_node.value();

  TypeNode *type = decl_node->type;
  const Node *prev_selector = &ident;
  for (unsigned long i = 0; i < selectors.size(); i++) {
    auto curr_selector = selectors.at(i).get();
    try {
      prev_selector = selectors.at(i - 1).get();
    } catch (std::out_of_range &e) {
    }

    if (auto array_selector =
            dynamic_cast<const ArrayIndexNode *>(curr_selector)) {
      type = lookup_array_selector(prev_selector, type, array_selector);
    } else if (auto record_selector =
                   dynamic_cast<const RecordFieldNode *>(curr_selector)) {
      type = lookup_record_selector(prev_selector, type, record_selector);
    } else {
      assert((void("Encountered unhandled SelectorNode type"), false));
    }
  }
  return type;
}

TypeNode *SymbolTable::lookup_type(const IdentNode &ident,
                                   const SelectorNode &selector) {
  auto lookup_node = this->lookup(ident);

  // Handle nullptr
  if (!lookup_node) {
    throw NotDeclaredException(ident);
    return {};
  }

  auto decl_node = lookup_node.value();

  TypeNode *type = decl_node->type;
  const Node *prev_selector = &ident;

  if (auto array_selector = dynamic_cast<const ArrayIndexNode *>(&selector)) {
    type = lookup_array_selector(prev_selector, type, array_selector);
  } else if (auto record_selector =
                 dynamic_cast<const RecordFieldNode *>(&selector)) {
    type = lookup_record_selector(prev_selector, type, record_selector);
  } else {
    assert((void("Encountered unhandled SelectorNode type"), false));
  }

  return type;
}

TypeNode *SymbolTable::lookup_array_selector(const Node *prev_selector,
                                             const TypeNode *type,
                                             const ArrayIndexNode *index) {
  if (auto array_type_node = dynamic_cast<const ArrayTypeNode *>(type)) {
    auto is_in_bounds = array_type_node->is_in_bounds(index->expression.get());
    if (is_in_bounds && !is_in_bounds.value()) {
      throw OutOfRangeException(*index);
    }

    return array_type_node->type;
  } else {
    throw WrongTypeException(*prev_selector, "ARRAY");

    return {};
  }
}

TypeNode *SymbolTable::lookup_record_selector(const Node *prev_selector,
                                              const TypeNode *type,
                                              const RecordFieldNode *field) {
  if (auto record_type_node = dynamic_cast<const RecordTypeNode *>(type)) {
    try {
      auto record_field = record_type_node->find_field(*field->ident);
      return record_field->type;
    } catch (FieldNotFoundException &e) {
      throw;
      return {};
    }
  } else if (auto sum_type_node = dynamic_cast<const SumTypeNode *>(type)) {
    try {
      auto sum_variant = sum_type_node->find_variant(*field->ident);
      return sum_variant->type;
    } catch (FieldNotFoundException &e) {
      throw;
      return {};
    }
  } else {
    throw WrongTypeException(*prev_selector, "RECORD or SUM");
    return {};
  }
}

ProcedureTypeNode *SymbolTable::lookup_variant_proc_type(
    const IdentNode &ident, const unique_ptr<SelectorNode> &selector) {

  auto lookup_node = this->lookup(ident);

  // Handle nullptr
  if (!lookup_node) {
    throw NotDeclaredException(ident);
    return {};
  }

  auto decl_node = lookup_node.value();

  TypeNode *type = decl_node->type;
  if (auto record_selector =
          dynamic_cast<const RecordFieldNode *>(selector.get())) {
    if (auto sum_type_node = dynamic_cast<const SumTypeNode *>(type)) {
      try {
        auto sum_variant = sum_type_node->find_variant(*record_selector->ident);
        return sum_variant->parameter_types;
      } catch (FieldNotFoundException &e) {
        throw;
        return {};
      }
    } else {
      throw WrongTypeException(ident, "SUM");
      return {};
    }
  } else {
    assert((void("Encountered unhandled SelectorNode type"), false));
  }
}

const char *LookupException::what() const noexcept { return msg_.c_str(); }
