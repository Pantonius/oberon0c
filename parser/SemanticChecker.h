#include "SymbolTable.h"
#include "ast/ASTContext.h"
#include "ast/ModuleNode.h"
#include "ast/TypeNode.h"
#include "global.h"
#include "parser/ast/DeclarationSequenceNode.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/IdentNode.h"
#include "parser/ast/StatementNode.h"
#include "util/Logger.h"
#include <memory>

using std::unique_ptr;

class SemanticChecker {
public:
  SemanticChecker(Logger &logger) : logger_(logger), symbol_table_(logger) {}
  ~SemanticChecker() = default;

  void onModuleStart(const FilePos , unique_ptr<IdentNode>);
  void onModuleEnd(const FilePos , const IdentNode &);

  unique_ptr<ConstDeclarationNode>
  onConst(const FilePos , unique_ptr<IdentNode>, unique_ptr<ExpressionNode>);

  unique_ptr<TypeDeclarationNode>
  onTypeDeclaration(const FilePos , unique_ptr<IdentNode>, TypeNode *);

  TypeNode *onIdentType(const FilePos , unique_ptr<IdentNode>);

  ArrayTypeNode *onArrayType(const FilePos , unique_ptr<ExpressionNode>,
                             TypeNode *);
  RecordTypeNode *
  onRecordType(const FilePos ,
               vector<std::pair<vector<unique_ptr<IdentNode>>, TypeNode *>>);

  vector<unique_ptr<VarDeclarationNode>>
  onVars(const FilePos , vector<unique_ptr<IdentNode>>, TypeNode *);

  ProcedureTypeNode *onProcedureType(
      const FilePos ,
      vector<std::tuple<vector<unique_ptr<IdentNode>>, bool, TypeNode *>>);

  unique_ptr<ProcedureDeclarationNode>
  onProcedureDeclaration(const FilePos , unique_ptr<IdentNode>,
                         ProcedureTypeNode *);
  void onProcedureEnd(const FilePos , const ProcedureDeclarationNode *,
                      const IdentNode &);

  unique_ptr<ProcedureCallNode>
  onProcedureCall(const FilePos , unique_ptr<IdentNode>,
                  vector<unique_ptr<SelectorNode>>,
                  vector<unique_ptr<ExpressionNode>>);

  unique_ptr<ExpressionNode> onUnaryExpression(const FilePos ,
                                               unique_ptr<ExpressionNode>,
                                               const UnaryOpType);

  unique_ptr<ExpressionNode> onBinaryExpression(const FilePos ,
                                                unique_ptr<ExpressionNode>,
                                                const BinaryOpType,
                                                unique_ptr<ExpressionNode>);
  unique_ptr<ExpressionNode>
  onIdentExpression(const FilePos , unique_ptr<IdentNode>,
                    vector<unique_ptr<SelectorNode>>);

  unique_ptr<AssignmentNode> onAssign(const FilePos , unique_ptr<IdentNode>,
                                      vector<unique_ptr<SelectorNode>>,
                                      unique_ptr<ExpressionNode>);

  unique_ptr<ArrayIndexNode> onArrayIndex(const FilePos ,
                                          unique_ptr<ExpressionNode>);

  ASTContext *get_context() { return &context_; }

  void expect_unique(const IdentNode *, const DeclarationNode *, bool = false);
  void expect_unique_within_scope(const IdentNode *, const DeclarationNode *);
  void expect_number(ExpressionNode *expr);
  void expect_bool(ExpressionNode *expr);

  template <typename L, typename T>
  unique_ptr<LiteralExpressionNode<T>>
  clone_literal(LiteralExpressionNode<T> *);

private:
  Logger &logger_;
  SymbolTable symbol_table_;
  ASTContext context_;
};
