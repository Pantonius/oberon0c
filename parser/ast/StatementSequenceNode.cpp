#include "StatementSequenceNode.h"

void StatementSequenceNode::addStatement(unique_ptr<StatementNode> stmt) {
  stmts_.push_back(std::move(stmt));
}
