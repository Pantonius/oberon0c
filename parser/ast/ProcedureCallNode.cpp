#include "ProcedureCallNode.h"

void ProcedureCallNode::print(std::ostream &stream) const {
  stream << to_string(this->getNodeType()) << ident << std::endl;
}
