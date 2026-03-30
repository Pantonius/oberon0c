#include "StatementSequenceNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void StatementSequenceNode::accept(NodeVisitor &visitor) {
  visitor.visit(*this);
}
void StatementSequenceNode::print(ostream &stream) const {
  for (size_t i = 0; i < stmts.size() - 1; i++) {
    stmts[i]->print(stream);
    stream << ";\n";
  }

  if (stmts.size() > 1) {
    stmts[stmts.size() - 1]->print(stream);
  }
}
