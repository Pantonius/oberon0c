#ifndef OBERON0C_NODE_H
#define OBERON0C_NODE_H

#include "scanner/Token.h"
#include <ostream>
#include <valarray>

enum class NodeType : char {
  array_selector,
  array_type,
  assignment,
  binary_expression,
  boolean,
  case_statement,
  const_declaration,
  declaration_sequence,
  elsif_statement,
  field,
  fp_section,
  ident,
  ident_expression,
  ident_pattern,
  ident_type,
  if_statement,
  literal_pattern,
  module,
  number,
  param_declaration,
  procedure_body,
  procedure_call,
  procedure_declaration,
  procedure_heading,
  procedure_type,
  record_selector,
  record_type,
  repeat_statement,
  statement,
  statement_sequence,
  std_type,
  sum_type,
  type_declaration,
  unary_expression,
  var_declaration,
  variant_declaration,
  variant_pattern,
  while_statement,
};
std::ostream &operator<<(std::ostream &stream, const NodeType &type);

class NodeVisitor;

class Node {

private:
  NodeType nodeType_;
  FilePos pos_;

public:
  Node(const NodeType nodeType, FilePos pos)
      : nodeType_(nodeType), pos_(std::move(pos)) {};
  virtual ~Node();

  bool operator==(const Node &) const = default;

  [[nodiscard]] NodeType getNodeType() const;
  [[nodiscard]] FilePos pos() const;

  virtual void accept(NodeVisitor &) = 0;

  virtual void print(std::ostream &) const = 0;
  friend std::ostream &operator<<(std::ostream &, const Node &);
};

#endif // OBERON0C_NODE_H
