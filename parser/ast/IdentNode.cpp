#include "IdentNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void IdentNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IdentNode::print(ostream &stream) const { stream << value; }
