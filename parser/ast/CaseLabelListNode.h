#ifndef OBERON0C_CASELABELLISTNODE_H
#define OBERON0C_CASELABELLISTNODE_H

#include "CaseLabelsNode.h"
#include "Node.h"
#include <memory>
#include <vector>

using std::unique_ptr;
using std::vector;

class CaseLabelListNode final : public Node {
public:
  CaseLabelListNode(const FilePos &pos)
      : Node(NodeType::case_label_list, pos) {}
  ~CaseLabelListNode() noexcept override;

  void accept(NodeVisitor &visitor) override;
  void print(std::ostream &stream) const override;

  vector<unique_ptr<CaseLabelsNode>> labels;
};

#endif // OBERON0C_CASELABELLISTNODE_H
