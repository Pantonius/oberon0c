#include "ImportNode.h"

void ImportNode::print(std::ostream &stream) const {
  stream << to_string(this->getNodeType()) << alias_ident << std::endl;
}
