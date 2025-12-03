#ifndef OBERON0C_NODE_H
#define OBERON0C_NODE_H

#include "scanner/Token.h"
#include <ostream>

enum class NodeType : char {
  // TODO sort these for more readability
  actual_parameters,
  array_type,
  assignment,
  const_declaration,
  declaration_sequence,
  expression,
  factor,
  field_list,
  formal_parameters,
  fp_section,
  if_statement,
  module,
  procedure_body,
  procedure_call,
  procedure_declaration,
  procedure_heading,
  record_type,
  repeat_statement,
  selector,
  simple_expr,
  statement,
  statement_sequence,
  term,
  type_declaration,
  type,
  var_declaration,
  while_statement,
};

class NodeVisitor;

class Node {

private:
  NodeType nodeType_;
  FilePos pos_;

public:
  explicit Node(const NodeType nodeType, FilePos pos)
      : nodeType_(nodeType), pos_(std::move(pos)) {};
  virtual ~Node();

  [[nodiscard]] NodeType getNodeType() const;
  [[nodiscard]] FilePos pos() const;

  virtual void accept(NodeVisitor &visitor) = 0;

  virtual void print(std::ostream &stream) const = 0;
  friend std::ostream &operator<<(std::ostream &stream, const Node &node);
};

#endif // OBERON0C_NODE_H
