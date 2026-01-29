#ifndef OBERON0C_NODE_H
#define OBERON0C_NODE_H

#include "scanner/Token.h"
#include <ostream>

enum class NodeType : char {
  array_type,
  assignment,
  binary_expression,
  boolean,
  const_declaration,
  declaration_sequence,
  elsif_statement,
  fp_section,
  field,
  ident,
  ident_expression,
  ident_type,
  if_statement,
  module,
  number,
  param_declaration,
  procedure_body,
  procedure_call,
  procedure_declaration,
  procedure_heading,
  procedure_type,
  record_type,
  repeat_statement,
  array_selector,
  record_selector,
  statement,
  statement_sequence,
  type_declaration,
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
