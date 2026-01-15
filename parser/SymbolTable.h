#ifndef OBERON0C_SYMBOLTABLE_H
#define OBERON0C_SYMBOLTABLE_H

#include "parser/ast/Node.h"
#include "util/Logger.h"
#include <unordered_map>
#include <vector>

class SymbolTable {
public:
  explicit SymbolTable(Logger &logger) : logger_(logger), table_() {};
  ~SymbolTable() = default;

  void beginScope();
  void endScope();
  void insert(const std::string &name, const Node *node);
  const Node *lookup(const std::string &name) const;
  bool contains(const std::string &name) const;

private:
  Logger &logger_;
  std::vector<std::unordered_map<std::string, const Node *>> table_;
};

#endif // !OBERON0C_SYMBOLTABLE_H
