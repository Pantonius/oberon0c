/*
 * Base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H

#include "../../util/Logger.h"
#include <list>
#include <ostream>
#include <string>

enum class NodeType : char {
  // TODO sort these for more readability
  module,
  import_list,
  import,
  declaration_sequence,
  const_declaration,
  type_declaration,
  var_declaration,
  procedure_declaration,
  procedure_heading,
  procedure_body,
  procedure_call,
  function_call,
  actual_parameters,
  expression,
  simple_expr,
  term,
  designator,
  set,
  element,
  statement,
  if_statement,
  case_statement,
  with_statement,
  while_statement,
  repeat_statement,
  loop_statement,
  exit_statement,
  return_statement,
  statement_sequence,
  factor,
  assignment,
  clause,
  case_label_list,
  case_labels,
  qual_ident,
  formal_parameters,
  fp_section,
  formal_type
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

#endif // OBERON0C_AST_H
