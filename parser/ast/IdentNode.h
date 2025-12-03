#ifndef OBERON0C_IDENTNODE_H
#define OBERON0C_IDENTNODE_H

#include "global.h"
#include "parser/ast/Node.h"

class IdentNode : public Node {
  IdentNode(FilePos &pos) : Node(NodeType::ident, pos) {}

  std::string ident;
};

#endif // !OBERON0C_IDENTNODE_H
