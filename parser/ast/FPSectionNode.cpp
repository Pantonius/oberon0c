#include "FPSectionNode.h"
#include "NodeVisitor.h"
#include "util/Logger.h"

void FPSectionNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void FPSectionNode::print(ostream &stream) const {
  auto fp_size = idents.size();
  if (fp_size > 0) {
    idents[0]->print(stream);
    for (size_t i = 1; i < fp_size; i++) {
      stream << ", ";
      idents[i]->print(stream);
    }
    stream << " : ";
    type->print(stream);
  }
}
