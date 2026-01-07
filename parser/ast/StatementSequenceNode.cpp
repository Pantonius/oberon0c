#include "StatementSequenceNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void StatementSequenceNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void StatementSequenceNode::print(ostream &stream) const {
  auto size = stmts.size();
  for (size_t i = 0; i < size - 1; i++) {
    stmts[i]->print(stream);
    stream << ";\n";
  }

  if (size > 1) {
    stmts[size - 1]->print(stream);
  }
}
