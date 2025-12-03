#ifndef OBERON0C_SELECTORNODE_H
#define OBERON0C_SELECTORNODE_H

#include "Node.h"
#include "ProcedureCallNode.h"
#include "parser/ast/Node.h"
#include <memory>

using std::unique_ptr;

class ExpressionNode;
class IdentNode;

class SelectorNode final : public Node {
public:
  SelectorNode(const FilePos &pos) : Node(NodeType::selector, pos) {}
  ~SelectorNode() override = default;

  string ident;
  unique_ptr<ExpressionNode> expression;
};

#endif // !OBERON0C_SELECTORNODE_H
