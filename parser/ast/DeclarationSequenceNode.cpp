#include "DeclarationSequenceNode.h"

vector<unique_ptr<ConstDeclarationNode>> &DeclarationSequenceNode::constants() {
  return consts_;
}

vector<unique_ptr<TypeDeclarationNode>> &DeclarationSequenceNode::types() {
  return types_;
}

vector<unique_ptr<VarDeclarationNode>> &DeclarationSequenceNode::variables() {
  return vars_;
}

vector<unique_ptr<ProcedureDeclarationNode>> &
DeclarationSequenceNode::procedures() {
  return procs_;
}
