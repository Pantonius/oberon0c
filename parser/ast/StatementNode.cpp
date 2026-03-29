#include "StatementNode.h"
#include "NodeVisitor.h"
#include "parser/ast/ASTContext.h"
#include "util/Logger.h"
#include <utility>

void AssignmentNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void AssignmentNode::print(ostream &stream) const {
  ident_expr->print(stream);

  stream << " = ";
  expression->print(stream);
}

void ElsIfStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ElsIfStatementNode::print(ostream &stream) const {
  stream << "ELSIF ";
  condition->print(stream);
  stream << "\n";
  if (body) {
    body->print(stream);
  }
  stream << "END";
}

void IfStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IfStatementNode::print(ostream &stream) const {
  stream << "IF ";
  condition->print(stream);
  stream << "\n";
  if (body) {
    body->print(stream);
  }
  stream << "END";
}

void ProcedureCallNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void ProcedureCallNode::print(ostream &stream) const {
  ident->print(stream);

  for (size_t i = 0; i < selectors.size(); i++) {
    selectors[i]->print(stream);
  }

  stream << "(";

  auto ap_size = actual_parameters.size();
  if (ap_size > 0) {
    actual_parameters[0]->print(stream);

    for (size_t i = 1; i < ap_size; i++) {
      stream << ", ";
      actual_parameters[i]->print(stream);
    }
  }
  stream << ")";
}

void WhileStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void WhileStatementNode::print(ostream &stream) const {
  stream << "WHILE ";
  condition->print(stream);
  stream << "DO\n";
  if (body) {
    body->print(stream);
  }
  stream << "\nEND";
}

void RepeatStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void RepeatStatementNode::print(ostream &stream) const {
  stream << "REPEAT\n";
  if (body) {
    body->print(stream);
  }
  stream << "\nUNTIL ";
  condition->print(stream);
}

void IdentPatternNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void IdentPatternNode::print(ostream &stream) const { stream << var; }

NumberPatternNode::NumberPatternNode(const FilePos pos, int32_t number)
    : LiteralPatternNode(pos, number, ASTContext::INTEGER) {}
void NumberPatternNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void NumberPatternNode::print(ostream &stream) const { stream << value; }

BooleanPatternNode::BooleanPatternNode(const FilePos pos, bool boolean)
    : LiteralPatternNode(pos, boolean, ASTContext::BOOLEAN) {}
void BooleanPatternNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void BooleanPatternNode::print(ostream &stream) const { stream << value; }

void VariantPatternNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void VariantPatternNode::print(ostream &stream) const {
  stream << sum_ident->value << "." << variant->ident->value;

  if (param_patterns.size() > 0) {
    stream << param_patterns.at(0);

    for (size_t i = 1; i < param_patterns.size(); i++) {
      stream << "; " << param_patterns.at(i);
    }
  }
}

void CaseStatementNode::accept(NodeVisitor &visitor) { visitor.visit(*this); }
void CaseStatementNode::print(ostream &stream) const {
  stream << "CASE " << value << " OF\n";

  if (cases_.size() > 0) {
    stream << cases_.at(0).first << " : " << cases_.at(0).second;

    for (size_t i = 1; i < cases_.size(); i++) {
      stream << "| " << cases_.at(0).first << " : " << cases_.at(0).second;
    }
  }
}

void CaseStatementNode::add_case(unique_ptr<PatternNode> pattern,
                                 unique_ptr<StatementSequenceNode> stmts) {
  cases_.emplace_back(std::move(pattern), std::move(stmts));
}

vector<std::pair<unique_ptr<PatternNode>, unique_ptr<StatementSequenceNode>>> *
CaseStatementNode::get_cases() {
  return &cases_;
}
