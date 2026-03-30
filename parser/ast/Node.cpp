/*
 * Base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#include "Node.h"

Node::~Node() = default;

NodeType Node::getNodeType() const { return nodeType_; }

std::ostream &operator<<(std::ostream &stream, const NodeType &type) {
  switch (type) {
  case NodeType::array_selector:
    return stream << "array_selector";
  case NodeType::array_type:
    return stream << "array_type";
  case NodeType::assignment:
    return stream << "assignment";
  case NodeType::binary_expression:
    return stream << "binary_expression";
  case NodeType::boolean:
    return stream << "boolean";
  case NodeType::case_statement:
    return stream << "case_statement";
  case NodeType::const_declaration:
    return stream << "const_declaration";
  case NodeType::declaration_sequence:
    return stream << "declaration_sequence";
  case NodeType::elsif_statement:
    return stream << "elsif_statement";
  case NodeType::field:
    return stream << "field";
  case NodeType::fp_section:
    return stream << "fp_section";
  case NodeType::ident:
    return stream << "ident";
  case NodeType::ident_expression:
    return stream << "ident_expression";
  case NodeType::ident_pattern:
    return stream << "ident_pattern";
  case NodeType::ident_type:
    return stream << "ident_type";
  case NodeType::if_statement:
    return stream << "if_statement";
  case NodeType::literal_pattern:
    return stream << "literal_pattern";
  case NodeType::module:
    return stream << "module";
  case NodeType::number:
    return stream << "number";
  case NodeType::param_declaration:
    return stream << "param_declaration";
  case NodeType::procedure_body:
    return stream << "procedure_body";
  case NodeType::procedure_call:
    return stream << "procedure_call";
  case NodeType::procedure_declaration:
    return stream << "procedure_declaration";
  case NodeType::procedure_heading:
    return stream << "procedure_heading";
  case NodeType::procedure_type:
    return stream << "procedure_type";
  case NodeType::record_selector:
    return stream << "record_selector";
  case NodeType::record_type:
    return stream << "record_type";
  case NodeType::repeat_statement:
    return stream << "repeat_statement";
  case NodeType::statement:
    return stream << "statement";
  case NodeType::statement_sequence:
    return stream << "statement_sequence";
  case NodeType::std_type:
    return stream << "std_type";
  case NodeType::sum_type:
    return stream << "sum_type";
  case NodeType::type_declaration:
    return stream << "type_declaration";
  case NodeType::unary_expression:
    return stream << "unary_expression";
  case NodeType::var_declaration:
    return stream << "var_declaration";
  case NodeType::variant_declaration:
    return stream << "variant_declaration";
  case NodeType::variant_pattern:
    return stream << "variant_pattern";
  case NodeType::while_statement:
    return stream << "while_statement";
  default:
    return stream << "UNKNOWN";
  }
}

FilePos Node::pos() const { return pos_; }

std::ostream &operator<<(std::ostream &stream, const Node &node) {
  node.print(stream);
  return stream;
}
