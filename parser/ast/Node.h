#ifndef OBERON0C_NODE_H
#define OBERON0C_NODE_H

#include "scanner/Token.h"
#include <ostream>

enum class NodeType : char {
  actual_parameters,
  array_type,
  assignment,
  const_declaration,
  declaration_sequence,
  expression,
  factor,
  formal_parameters,
  fp_section,
  if_statement,
  elsif_statement,
  module,
  procedure_body,
  procedure_call,
  procedure_declaration,
  procedure_heading,
  record_type,
  repeat_statement,
  selector,
  statement,
  statement_sequence,
  term,
  type,
  type_declaration,
  number_expression,
  ident_expression,
  binary_expression,
  unary_expression,
  var_declaration,
  while_statement,
};

class NodeVisitor;

class Node {

private:
  NodeType nodeType_;
  FilePos pos_;

public:
  Node(const NodeType nodeType, FilePos pos)
      : nodeType_(nodeType), pos_(std::move(pos)) {};
  virtual ~Node();

  [[nodiscard]] NodeType getNodeType() const;
  [[nodiscard]] FilePos pos() const;

  virtual void accept(NodeVisitor &) = 0;

  virtual void print(std::ostream &) const = 0;
  friend std::ostream &operator<<(std::ostream &, const Node &);
};

#endif // OBERON0C_NODE_H
