#include "ModuleNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void ModuleNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ModuleNode::print(ostream &stream) const {
  stream << "MODULE ";
  ident->print(stream);
  stream << "\n";
  statement_sequence_->print(stream);
  stream << "END ";
  ident->print(stream);
  stream << ";";
}
