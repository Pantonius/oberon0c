#include "ast/ModuleNode.h"
#include "parser/SymbolTable.h"
#include "util/Logger.h"
#include <memory>

using std::unique_ptr;

class SemanticChecker {
public:
  SemanticChecker(Logger &logger) : logger_(logger), symbol_table_(logger) {}
  ~SemanticChecker() = default;

  unique_ptr<ModuleNode> onModuleStart(const FilePos &, unique_ptr<IdentNode>);
  void onModuleEnd(const FilePos &, const unique_ptr<IdentNode> &);

  unique_ptr<ConstDeclarationNode>
  onConst(const FilePos &, unique_ptr<IdentNode>, unique_ptr<ExpressionNode>);
  unique_ptr<TypeDeclarationNode> onType(const FilePos &, unique_ptr<IdentNode>,
                                         unique_ptr<TypeNode>);
  unique_ptr<VarDeclarationNode> onVar(const FilePos &, unique_ptr<IdentNode>,
                                       unique_ptr<ExpressionNode>);

  unique_ptr<ProcedureDeclarationNode>
  onProcedureStart(const FilePos &, unique_ptr<IdentNode>,
                   vector<unique_ptr<FPSectionNode>>);
  void onProcedureEnd(const FilePos &, const unique_ptr<IdentNode> &);

  void expect_unique(const IdentNode *, const Node *);

private:
  Logger &logger_;
  SymbolTable symbol_table_;
};
