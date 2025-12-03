#ifndef OBERON0C_SELECTORNODE_H
#define OBERON0C_SELECTORNODE_H

#include "global.h"
#include "parser/ast/Node.h"
#include <memory>

using std::unique_ptr;

class ExpressionNode;
class IdentNode;

class SelectorNode final : public Node {
  SelectorNode(const FilePos &pos) : Node(NodeType::selector, pos) {}

  unique_ptr<IdentNode> ident;
  unique_ptr<ExpressionNode> expression;
};

#endif // !OBERON0C_SELECTORNODE_H
