#include "ModuleNode.h"
#include "parser/ast/TypeNode.h"
#include <memory>

class ASTContext {
private:
  vector<unique_ptr<TypeNode>> types_;
  unique_ptr<ModuleNode> module_;

  unique_ptr<IdentTypeNode> std_ident_type_(string name) {
    return std::make_unique<IdentTypeNode>(
        EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, name));
  };

public:
  ASTContext() {}; // TODO add standard environment
  ~ASTContext() = default;

  // NOTE for multi-module use this should be a vector
  ModuleNode *get_module();
  void set_module(unique_ptr<ModuleNode>);

  IdentTypeNode *add_type(unique_ptr<IdentTypeNode> type);
  ArrayTypeNode *add_type(unique_ptr<ArrayTypeNode> type);
  RecordTypeNode *add_type(unique_ptr<RecordTypeNode> type);

  const unique_ptr<IdentTypeNode> STD_TYPES[2] = {std_ident_type_("INTEGER"),
                                                  std_ident_type_("BOOLEAN")};

  const static std::shared_ptr<ProcedureTypeNode> STD_PROCEDURE_TYPE;
};
