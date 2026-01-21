#ifndef OBERON0C_SYMBOLTABLE_H
#define OBERON0C_SYMBOLTABLE_H

#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/Node.h"
#include "parser/ast/TypeNode.h"
#include "util/Logger.h"
#include <unordered_map>
#include <utility>
#include <vector>

class SymbolTable {
public:
  explicit SymbolTable(Logger &logger) : logger_(logger), table_() {};
  ~SymbolTable() = default;

  void beginScope();
  void endScope();
  void insert(const IdentNode &ident, const TypeNode *node);

  const TypeNode *lookup(const std::string &name) const;

  // TODO: Change 'unique_ptr' to 'shared_ptr'
  const TypeNode *lookup(const IdentNode &ident,
                         const vector<unique_ptr<SelectorNode>> &selector =
                             vector<unique_ptr<SelectorNode>>());

private:
  Logger &logger_;
  std::vector<std::unordered_map<std::string,
                                 std::pair<const IdentNode, const TypeNode *>>>
      table_;
};

#endif // !OBERON0C_SYMBOLTABLE_H
