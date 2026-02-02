#ifndef OBERON0C_SYMBOLTABLE_H
#define OBERON0C_SYMBOLTABLE_H

#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/Node.h"
#include "parser/ast/TypeNode.h"
#include "util/Logger.h"
#include <exception>
#include <optional>
#include <unordered_map>
#include <vector>

class SymbolTable {
public:
  explicit SymbolTable(Logger &logger) : logger_(logger), table_() {};
  ~SymbolTable() = default;

  void beginScope();
  void endScope();
  void insert(const IdentNode &ident, const DeclarationNode *node);

  /**
   * Lookup `ident` in `SymbolTable`.
   *
   * @param ident the identifier to search for
   * @param this_scope wether the check should only pertain to the current scope
   * (default: false)
   *
   * @returns `std::optional<const DeclarationNode *>`
   */
  std::optional<const DeclarationNode *> lookup(const IdentNode &ident,
                                                bool this_scope = false) const;

  const TypeNode *
  lookup_type(const IdentNode &ident,
              const vector<unique_ptr<SelectorNode>> &selectors);

private:
  Logger &logger_;
  std::vector<std::unordered_map<std::string, const DeclarationNode *>> table_;
};

class LookupException : public std::exception {
private:
  const Node &node_;
  const string msg_;

public:
  LookupException(const Node &node)
      : node_(node),
        msg_(to_string(&node) + " could not be found in symbol table") {}
  LookupException(const Node &node, const string msg)
      : node_(node), msg_(msg) {}

  const char *what() const noexcept override { return msg_.c_str(); }
  const Node &get_node() const noexcept { return node_; }
};

class WrongTypeException : public LookupException {
private:
  const string req_type_;

public:
  WrongTypeException(const Node &node, const string req_type)
      : LookupException(node,
                        to_string(&node) + " is not a " + req_type + " type"),
        req_type_(req_type) {}
};

class NotDeclaredException : public LookupException {
public:
  NotDeclaredException(const Node &node) : LookupException(node) {}
};

class OutOfRangeException : public LookupException {
public:
  OutOfRangeException(const ArrayIndexNode &node)
      : LookupException(node, "Index is out of bounds") {}
};

class NullIdentException {
public:
  NullIdentException() {}
};

#endif // !OBERON0C_SYMBOLTABLE_H
