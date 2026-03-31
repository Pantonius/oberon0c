#include "SemanticChecker.h"
#include "ast/DeclarationSequenceNode.h"
#include "ast/IdentNode.h"
#include "ast/ModuleNode.h"
#include "ast/TypeNode.h"
#include "global.h"
#include "parser/SymbolTable.h"
#include "parser/ast/ASTContext.h"
#include "parser/ast/ExpressionNode.h"
#include "parser/ast/StatementNode.h"
#include "util/Logger.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <optional>
#include <queue>
#include <sys/types.h>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

using FormalParameterType =
    vector<std::tuple<vector<unique_ptr<IdentNode>>, bool, TypeNode *>>;

void SemanticChecker::onModuleStart(const FilePos pos,
                                    unique_ptr<IdentNode> ident) {
  auto module = make_unique<ModuleNode>(pos, std::move(ident));
  symbol_table_.beginScope();
  context_.set_module(std::move(module));

  // introduce std environment: WriteInt(val: Integer), WriteLn()
  // auto write_int_arg = std::make_unique<ParamDeclarationNode>(
  //     EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "val"), false,
  //     ASTContext::INTEGER);
  // vector<unique_ptr<ParamDeclarationNode>> write_int_params;
  // write_int_params.push_back(std::move(write_int_arg));
  // // TODO this type should be held somewhere (ASTContext?)
  // auto write_int_type = std::make_unique<ProcedureTypeNode>(
  //     EMPTY_POS, std::move(write_int_params));
  // auto write_int_decl = std::make_unique<ProcedureDeclarationNode>(
  //     EMPTY_POS, std::make_unique<IdentNode>(EMPTY_POS, "WriteInt"),
  //     write_int_type.get());
  //
  // symbol_table_.insert(*write_int_decl->ident, write_int_decl.get());
}

void SemanticChecker::onModuleEnd(const FilePos pos, const IdentNode &ident) {
  if (context_.get_module()->ident->value != ident.value) {
    logger_.error(pos, "End identifier does not match module identifier.");
    exit(EXIT_FAILURE);
  }

  symbol_table_.endScope();
}

unique_ptr<ConstDeclarationNode>
SemanticChecker::onConst(const FilePos pos, unique_ptr<IdentNode> ident,
                         unique_ptr<ExpressionNode> expr) {

  if (!expr) {
    logger_.error(pos, "Undefined constant value: " + ident->value);
    throw UndeclaredArgumentException(ident->value);
    return {};
  }

  if (!expr->is_const()) {
    logger_.error(pos,
                  "Non-constant value in const declaration: " + ident->value);
    throw NonConstException(*expr);
    return {};
  }

  auto type = expr->type;
  auto const_decl = make_unique<ConstDeclarationNode>(pos, std::move(ident),
                                                      std::move(expr), type);
  expect_unique(const_decl->ident.get(), const_decl.get());

  return const_decl;
}

vector<unique_ptr<VarDeclarationNode>>
SemanticChecker::onVars(const FilePos pos, vector<unique_ptr<IdentNode>> idents,
                        TypeNode *type) {
  if (!type) {
    logger_.error(pos, "Unspecified variable type.");
    exit(EXIT_FAILURE);
  }

  vector<unique_ptr<VarDeclarationNode>> var_decls;
  for (size_t i = 0; i < idents.size(); i++) {
    auto var_decl =
        make_unique<VarDeclarationNode>(pos, std::move(idents[i]), type);

    expect_unique(var_decl->ident.get(), var_decl.get());
    var_decls.push_back(std::move(var_decl));
  }

  return var_decls;
}

unique_ptr<TypeDeclarationNode> SemanticChecker::onTypeDeclaration(
    const FilePos pos, unique_ptr<IdentNode> ident, TypeNode *type) {
  auto type_declaration =
      make_unique<TypeDeclarationNode>(pos, std::move(ident), type);

  expect_unique(type_declaration->ident.get(), type_declaration.get());

  return type_declaration;
}

TypeNode *SemanticChecker::onIdentType(const FilePos pos,
                                       unique_ptr<IdentNode> ident) {
  try {
    auto std_type = ASTContext::std_types.at(ident->value);
    return std_type;
  } catch (...) {
  }

  auto type_decl_ = symbol_table_.lookup(*ident);

  if (!type_decl_) {
    logger_.error(pos, "Specified type '" + ident->value +
                           "' is not declared in the current scope.");
    return {};
  }

  if (auto type_decl =
          dynamic_cast<const TypeDeclarationNode *>(type_decl_.value())) {
    return type_decl->type;
  }

  logger_.error(pos, "'" + to_string(*ident) + "' is not a type.");
  return {};
}

ArrayTypeNode *SemanticChecker::onArrayType(const FilePos pos,
                                            unique_ptr<ExpressionNode> expr,
                                            TypeNode *type) {
  if (!expr) {
    logger_.error(pos, "Undefined array size.");
    exit(EXIT_FAILURE);
  }

  auto num = dynamic_unique_ptr_copy_cast<NumberExpressionNode>(expr.get());

  if (!num) {
    logger_.error(pos, "Array size is not a constant number.");
    throw NonConstException(*expr);
    return {};
  }

  if (num->value < 0) {
    logger_.error(num->pos(), "Array size cannot be negative");
    throw NegativeIntegerException(*num);
    return {};
  }

  auto array_type = std::make_unique<ArrayTypeNode>(pos, std::move(num), type);
  auto ptr = context_.add_type(std::move(array_type));

  return ptr;
}

RecordTypeNode *SemanticChecker::onRecordType(
    const FilePos pos,
    vector<std::pair<vector<unique_ptr<IdentNode>>, TypeNode *>> fields) {

  symbol_table_.beginScope();

  vector<unique_ptr<VarDeclarationNode>> vars;
  for (size_t i = 0; i < fields.size(); i++) {
    auto decls = std::move(fields[i].first);
    auto type = fields[i].second;

    for (size_t j = 0; j < decls.size(); j++) {
      auto var_decl =
          make_unique<VarDeclarationNode>(pos, std::move(decls[j]), type);

      expect_unique_within_scope(var_decl->ident.get(), var_decl.get());
      vars.push_back(std::move(var_decl));
    }
  }

  symbol_table_.endScope();

  auto record_type = std::make_unique<RecordTypeNode>(pos, std::move(vars));
  auto ptr = context_.add_type(std::move(record_type));

  return ptr;
}

SumTypeNode *SemanticChecker::onSumType(
    const FilePos pos,
    vector<std::pair<unique_ptr<IdentNode>, vector<TypeNode *>>>
        proto_variants) {
  auto sum_type = std::make_unique<SumTypeNode>(pos);

  symbol_table_.beginScope();

  vector<unique_ptr<VariantDeclarationNode>> variants;
  for (auto &proto_variant : proto_variants) {
    auto variant_pos = proto_variant.first->pos();

    FormalParameterType params;
    for (size_t i = 0; i < proto_variant.second.size(); i++) {
      auto type = proto_variant.second.at(i);
      auto ident = std::make_unique<IdentNode>(type->pos(), to_string(i));

      vector<unique_ptr<IdentNode>> vec;
      vec.push_back(std::move(ident));

      params.emplace_back(std::move(vec), false, type);
    }
    auto procedure_type = onProcedureType(variant_pos, std::move(params));

    auto variant = std::make_unique<VariantDeclarationNode>(
        variant_pos, std::move(proto_variant.first), procedure_type,
        sum_type.get());

    expect_unique_within_scope(variant->ident.get(), variant.get());

    variants.push_back(std::move(variant));
  }

  sum_type->setVariants(std::move(variants));

  symbol_table_.endScope();

  auto ptr = context_.add_type(std::move(sum_type));

  return ptr;
}

unique_ptr<ExpressionNode> SemanticChecker::onIdentExpression(
    const FilePos pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<SelectorNode>> selectors,
    vector<unique_ptr<ExpressionNode>> actual_params) {

  // get ident type for more detailed error messages
  TypeNode *type = nullptr;
  const DeclarationNode *decl;
  try {
    type = symbol_table_.lookup_type(*ident, selectors);
    decl = symbol_table_.lookup(*ident).value();
    if (!decl) {
      logger_.error(ident->pos(), to_string(*ident) + " is not a variable.");
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), std::move(actual_params),
        decl, type, false);
  } catch (FieldNotFoundException &e) {
    if (decl->type->getNodeType() == NodeType::sum_type) {
      logger_.error(e.field().pos(), "No such variant: " + decl->ident->value +
                                         "." + e.field().value);
      exit(EXIT_FAILURE);
    } else {
      logger_.error(e.field().pos(), e.what());
      exit(EXIT_FAILURE);
    }
  }

  if (decl->getNodeType() == NodeType::type_declaration) {
    if (decl->type->getNodeType() != NodeType::sum_type) {
      logger_.error(decl->pos(), "Expected sum type.");
      exit(EXIT_FAILURE);
    }

    if (selectors.size() == 0) {
      logger_.error(pos,
                    "Unqualified variant expressions are not supported yet.");
      exit(EXIT_FAILURE);
    }

    if (selectors.size() != 1) {
      logger_.error(pos, "Malformed variant selector.");
      exit(EXIT_FAILURE);
    }

    ProcedureTypeNode *variant_proc_type;
    auto variant = dynamic_cast<const RecordFieldNode *>(selectors.at(0).get());
    try {
      variant_proc_type =
          symbol_table_.lookup_variant_proc_type(*ident, selectors.at(0));
    } catch (FieldNotFoundException &e) {
      logger_.error(selectors.at(0)->pos(),
                    "No such variant: " + ident->value + "." +
                        to_string(variant->ident->value));
      exit(EXIT_FAILURE);
    }

    // parameter count check
    if (variant_proc_type->formal_parameters.size() != actual_params.size()) {
      logger_.error(pos, "Number of given parameters does not match declared "
                         "variant parameters.");
      exit(EXIT_FAILURE);
    }

    // parameter type check
    for (size_t i = 0; i < actual_params.size(); i++) {
      if (actual_params.at(i)->type !=
          variant_proc_type->formal_parameters.at(i)->type) {
        logger_.error(
            actual_params.at(i)->pos(),
            "Given type " + to_string(actual_params.at(i)->type) +
                " does not match expected type " +
                to_string(variant_proc_type->formal_parameters.at(i)->type));
        exit(EXIT_FAILURE);
      }
    }

    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), std::move(actual_params),
        decl, type, false);
  } else if (decl->getNodeType() == NodeType::const_declaration) {
    if (actual_params.size() > 0) {
      logger_.error(actual_params.at(0)->pos(),
                    "Constant expressions should not have parameters.");
      exit(EXIT_FAILURE);
    }

    auto const_decl = dynamic_cast<const ConstDeclarationNode *>(decl);

    auto expr = const_decl->expression.get();
    switch (expr->getNodeType()) {
    case NodeType::boolean:
      return clone_literal<BooleanExpressionNode>(
          dynamic_cast<BooleanExpressionNode *>(expr));
    case NodeType::number:
      return clone_literal<NumberExpressionNode>(
          dynamic_cast<NumberExpressionNode *>(expr));
    default:
      logger_.error(pos, "Constant of invalid node type.");
      exit(EXIT_FAILURE);
    }
  } else if (decl->getNodeType() == NodeType::var_declaration ||
             decl->getNodeType() == NodeType::param_declaration) {

    if (actual_params.size() > 0) {
      logger_.error(
          actual_params.at(0)->pos(),
          "Variable and parameter expressions should not have parameters.");
      exit(EXIT_FAILURE);
    }

    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, type, false);
  } else {
    logger_.error(pos, "Identifier is not a constant or variable.");
    exit(EXIT_FAILURE);
  }
}

unique_ptr<IdentExpressionNode> SemanticChecker::onIdentExpressionReference(
    const FilePos pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<SelectorNode>> selectors) {
  // get ident type for more detailed error messages
  TypeNode *type = nullptr;
  const DeclarationNode *decl;
  try {
    type = symbol_table_.lookup_type(*ident, selectors);
    decl = symbol_table_.lookup(*ident).value();
    if (!decl) {
      logger_.error(ident->pos(), to_string(*ident) + " is not a variable.");
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, type, true);
  }

  // lookup ident declaration
  auto node_lookup = symbol_table_.lookup(*ident);
  if (!node_lookup) {
    logger_.error(pos, "Undeclared identifier.");
    exit(EXIT_FAILURE);
  }

  auto node = node_lookup.value();

  if (node->getNodeType() == NodeType::var_declaration ||
      node->getNodeType() == NodeType::param_declaration) {

    return std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, type, true);
  } else {
    logger_.error(
        pos,
        "Identifier is not a variable or parameter. Cannot pass by reference.");
    exit(EXIT_FAILURE);
  }
}

ProcedureTypeNode *
SemanticChecker::onProcedureType(const FilePos pos,
                                 FormalParameterType formal_parameters) {
  symbol_table_.beginScope();

  vector<unique_ptr<ParamDeclarationNode>> params;
  for (size_t i = 0; i < formal_parameters.size(); i++) {
    auto idents = std::get<0>(std::move(formal_parameters[i]));
    auto by_reference = std::get<1>(formal_parameters[i]);
    auto type = std::get<2>(formal_parameters[i]);

    for (size_t j = 0; j < idents.size(); j++) {
      auto ident_pos = idents[j]->pos();
      auto param = std::make_unique<ParamDeclarationNode>(
          ident_pos, std::move(idents[j]), by_reference, type);

      expect_unique_within_scope(param->ident.get(), param.get());

      params.push_back(std::move(param));
    }
  }
  symbol_table_.endScope();

  auto proc_type = std::make_unique<ProcedureTypeNode>(pos, std::move(params));
  auto ptr = context_.add_type(std::move(proc_type));

  return ptr;
}

unique_ptr<ProcedureDeclarationNode> SemanticChecker::onProcedureDeclaration(
    const FilePos pos, unique_ptr<IdentNode> ident, ProcedureTypeNode *type) {
  auto proc_decl =
      std::make_unique<ProcedureDeclarationNode>(pos, std::move(ident), type);
  expect_unique(proc_decl->ident.get(), proc_decl.get());

  symbol_table_.beginScope();

  // introduce formal parameters into scope
  for (size_t i = 0; i < type->formal_parameters.size(); i++) {
    symbol_table_.insert(*type->formal_parameters[i]->ident,
                         type->formal_parameters[i].get());
  }

  return proc_decl;
}

void SemanticChecker::onProcedureEnd(const FilePos pos,
                                     const ProcedureDeclarationNode *procedure,
                                     const IdentNode &ident) {
  if (procedure->ident->value != ident.value) {
    logger_.error(pos, "End identifier does not match procedure identifier.");
    exit(EXIT_FAILURE);
  }

  // TODO checks
  symbol_table_.endScope();
}

const ProcedureTypeNode *SemanticChecker::onProcedureCallStart(
    const FilePos pos, const IdentNode &ident,
    const vector<unique_ptr<SelectorNode>> &selectors) {

  if (selectors.size() > 0) {
    logger_.error(pos, "Cannot handle selectors in procedure calls as of now.");
    exit(EXIT_FAILURE);
  }

  // get procedure declaration
  auto opt_decl = symbol_table_.lookup(ident);
  if (!opt_decl) {
    logger_.error(pos, "Undeclared procedure identifier.");
    exit(EXIT_FAILURE);
  }
  if (opt_decl.value()->getNodeType() != NodeType::procedure_declaration) {
    logger_.error(pos,
                  "Identifier is not associated with a procedure declaration.");
    exit(EXIT_FAILURE);
  }
  auto proc_decl =
      dynamic_cast<const ProcedureDeclarationNode *>(opt_decl.value());

  // look into procedure type
  return dynamic_cast<const ProcedureTypeNode *>(proc_decl->type);
}

unique_ptr<ProcedureCallNode> SemanticChecker::onProcedureCall(
    const FilePos pos, unique_ptr<IdentNode> ident,
    vector<unique_ptr<SelectorNode>> selectors,
    vector<unique_ptr<ExpressionNode>> actual_params) {

  // get procedure declaration
  auto opt_decl = symbol_table_.lookup(*ident);
  if (!opt_decl) {
    logger_.error(pos, "Undeclared procedure identifier.");
    exit(EXIT_FAILURE);
  }
  if (opt_decl.value()->getNodeType() != NodeType::procedure_declaration) {
    logger_.error(pos,
                  "Identifier is not associated with a procedure declaration.");
    exit(EXIT_FAILURE);
  }
  auto proc_decl =
      dynamic_cast<const ProcedureDeclarationNode *>(opt_decl.value());

  // look into procedure type
  auto proc_type = dynamic_cast<const ProcedureTypeNode *>(proc_decl->type);

  // match argument types
  if (proc_type->formal_parameters.size() != actual_params.size()) {
    logger_.error(pos, "Number of given parameters does not match declared "
                       "procedure parameters.");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < proc_type->formal_parameters.size(); i++) {
    // NOTE type compatibility for now just means identity
    // TODO add logic for type casting
    if (proc_type->formal_parameters[i]->type != actual_params[i]->type) {
      logger_.error(actual_params[i]->pos(),
                    "Parameter type does not match declared formal type.");
      exit(EXIT_FAILURE);
    }

    if (proc_type->formal_parameters[i]->by_reference) {
      if (actual_params[i]->getNodeType() != NodeType::ident_expression) {
        logger_.error(actual_params[i]->pos(),
                      "Passed parameter is not declared.");
        exit(EXIT_FAILURE);
      }

      auto ident_expr =
          dynamic_cast<const IdentExpressionNode *>(actual_params[i].get());

      auto opt_decl = symbol_table_.lookup(*ident_expr->ident);
      if (!opt_decl ||
          opt_decl.value()->getNodeType() != NodeType::var_declaration) {
        logger_.error(actual_params[i]->pos(),
                      "Identifier is not associated with a var declaration.");
        exit(EXIT_FAILURE);
      }
    }
  }

  return std::make_unique<ProcedureCallNode>(
      pos, std::move(ident), std::move(selectors), std::move(actual_params),
      proc_decl);
}

unique_ptr<IdentPatternNode>
SemanticChecker::onIdentPattern(const FilePos pos, unique_ptr<IdentNode> ident,
                                TypeNode *value_type) {
  auto var_decl =
      make_unique<VarDeclarationNode>(pos, std::move(ident), value_type);
  expect_unique(var_decl->ident.get(), var_decl.get());

  return make_unique<IdentPatternNode>(pos, std::move(var_decl), value_type);
}

unique_ptr<NumberPatternNode>
SemanticChecker::onNumberPattern(const FilePos pos, int32_t number,
                                 TypeNode *type) {
  if (type != ASTContext::INTEGER) {
    logger_.error(pos, "Expected pattern of type " + to_string(type) +
                           " but got INTEGER.");
    exit(EXIT_FAILURE);
  }

  return std::make_unique<NumberPatternNode>(pos, number);
}
unique_ptr<BooleanPatternNode>
SemanticChecker::onBooleanPattern(const FilePos pos, bool boolean,
                                  TypeNode *type) {
  if (type != ASTContext::BOOLEAN) {
    logger_.error(pos, "Expected pattern of type " + to_string(type) +
                           " but got BOOLEAN.");
    exit(EXIT_FAILURE);
  }

  return std::make_unique<BooleanPatternNode>(pos, boolean);
}

unique_ptr<VariantPatternNode> SemanticChecker::onVariantPattern(
    const FilePos pos, unique_ptr<IdentNode> sum_ident,
    unique_ptr<RecordFieldNode> variant,
    vector<unique_ptr<PatternNode>> param_pats, TypeNode *value_type) {

  TypeNode *type = nullptr;
  const DeclarationNode *decl;
  try {
    type = symbol_table_.lookup_type(*sum_ident, *variant);
    decl = symbol_table_.lookup(*sum_ident).value();
    if (!decl) {
      logger_.error(sum_ident->pos(),
                    to_string(*sum_ident) + " is not declared.");
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    exit(EXIT_FAILURE);
  } catch (FieldNotFoundException &e) {
    logger_.error(variant->pos(), "No such variant: " + sum_ident->value + "." +
                                      e.field().value);
    exit(EXIT_FAILURE);
  }

  if (decl->type->getNodeType() != NodeType::sum_type) {
    logger_.error(pos, sum_ident->value + "." + variant->ident->value +
                           " is not a sum type variant: " +
                           to_string(decl->getNodeType()));
    exit(EXIT_FAILURE);
  }

  auto sum_type = dynamic_cast<const SumTypeNode *>(decl->type);

  const VariantDeclarationNode *variant_decl;
  try {
    variant_decl = sum_type->find_variant(*variant->ident);
  } catch (FieldNotFoundException &e) {
    logger_.error(variant->pos(), "No such variant: " + sum_ident->value + "." +
                                      e.field().value);
    exit(EXIT_FAILURE);
  }

  if (variant_decl->type != value_type) {
    logger_.error(pos,
                  "Pattern does not match the type of the case expression.");
    exit(EXIT_FAILURE);
  }

  if (variant_decl->parameter_types->formal_parameters.size() !=
      param_pats.size()) {
    logger_.error(
        pos,
        "Variant pattern has " + to_string(param_pats.size()) +
            " patterns for actual " +
            to_string(variant_decl->parameter_types->formal_parameters.size()) +
            " parameters.");
    exit(EXIT_FAILURE);
  }

  for (size_t i = 0; i < param_pats.size(); i++) {
    if (variant_decl->parameter_types->formal_parameters.at(i)->type !=
        param_pats.at(i)->type) {
      logger_.error(param_pats.at(i)->pos(),
                    "Pattern for parameter at index " + to_string(i) +
                        " does not match declared type.");
      exit(EXIT_FAILURE);
    }
  }

  return make_unique<VariantPatternNode>(pos, std::move(sum_ident),
                                         std::move(variant),
                                         std::move(param_pats), type);
}

unique_ptr<CaseStatementNode>
SemanticChecker::onCaseStatementStart(const FilePos pos,
                                      unique_ptr<ExpressionNode> expr) {
  return make_unique<CaseStatementNode>(pos, std::move(expr));
}

void SemanticChecker::onCaseStatementCaseStart() { symbol_table_.beginScope(); }
void SemanticChecker::onCaseStatementCaseEnd(
    CaseStatementNode &case_stmt, unique_ptr<PatternNode> pattern,
    unique_ptr<StatementSequenceNode> stmts) {
  symbol_table_.endScope();

  case_stmt.add_case(std::move(pattern), std::move(stmts));
}

std::tuple<bool, std::unordered_map<string, vector<u_int>>, vector<u_int>>
SemanticChecker::variant_pattern_exhaustiveness(
    const FilePos pos, const SumTypeNode *sum_type,
    std::map<u_int, const PatternNode *> case_patterns, bool is_last) {
  bool is_exhaustive = true;
  std::unordered_map<string, vector<u_int>> variant_map;
  vector<u_int> wildcard_cases;

  for (auto &variant : sum_type->variants) {
    variant_map[variant->ident->value] = {};
  }

  for (const auto &[i, pattern] : case_patterns) {
    if (is_last && wildcard_cases.size() > 0) {
      logger_.warning(pattern->pos(), "Unreachable case.");
      continue;
    }

    if (pattern->getNodeType() == NodeType::ident_pattern) {
      wildcard_cases.push_back(i);
    } else if (pattern->getNodeType() == NodeType::variant_pattern) {
      auto variant_pattern = dynamic_cast<const VariantPatternNode *>(pattern);
      variant_map[variant_pattern->variant->ident->value].push_back(i);
    } else {
      logger_.error(pattern->pos(), "UNEXPECTED KIND OF PATTERN");
      exit(EXIT_FAILURE);
    }
  }

  if (wildcard_cases.size() == 0) {
    for (auto v : variant_map) {
      // NOTE if this throws, call a developer because he messed something up
      auto v_variant = sum_type->find_variant(v.first).value();

      if (v.second.size() == 0) {
        logger_.warning(pos, "Non-exhaustive patterns: Missing cases for " +
                                 v.first + " variant.");
        is_exhaustive = false;
      } else if (v_variant->parameter_types->formal_parameters.size() > 0) {
        // Variant with parameters
        auto vertically_exhaustive_cases = std::make_unique<CaseTree>(
            VariantCaseTreeKey(v_variant->ident->value), v.second, 0);
        std::queue<CaseTree *> leafs;
        // WorkList :P
        leafs.push(vertically_exhaustive_cases.get());

        while (leafs.size() > 0) {
          auto curr_leaf = leafs.front();
          leafs.pop(); // why the heck does pop not return the object?!

          auto curr_param_index = curr_leaf->level;
          if (curr_param_index >=
              v_variant->parameter_types->formal_parameters.size()) {
            break; // already done
          }

          // for each parameter index
          auto param_type = v_variant->parameter_types->formal_parameters
                                .at(curr_param_index)
                                ->type;

          std::map<u_int, const PatternNode *> sub_case_patterns;

          for (auto ci : curr_leaf->cases) {
            // just the patterns of that variant
            auto case_variant_pattern =
                dynamic_cast<const VariantPatternNode *>(case_patterns.at(ci));

            sub_case_patterns[ci] =
                case_variant_pattern->param_patterns.at(curr_param_index).get();
          }

          if (sub_case_patterns.size() == 0) {
            logger_.error(pos, "No patterns for parameter at index " +
                                   to_string(curr_param_index) +
                                   " in variant " + v.first);
            continue;
          }

          auto pos = sub_case_patterns
                         .at(curr_leaf->cases.at(curr_leaf->cases.size() - 1))
                         ->pos();

          if (param_type == ASTContext::INTEGER) {
            auto result = number_pattern_exhaustiveness(
                pos, sub_case_patterns,
                curr_param_index + 1 ==
                    v_variant->parameter_types->formal_parameters.size());

            // 0: is_exhaustive, 1: literal_cases, 2: wildcard_cases
            if (std::get<0>(result)) {
              for (const auto &[literal, literal_cases] : std::get<1>(result)) {
                auto exhaust_set = std::make_unique<CaseTree>(
                    (NumberCaseTreeKey(literal)), literal_cases,
                    curr_leaf->level + 1);

                // add into worklist
                leafs.push(exhaust_set.get());

                // add into tree
                curr_leaf->children.push_back(std::move(exhaust_set));
              }

              auto exhaust_set = std::make_unique<CaseTree>(
                  (IdentCaseTreeKey()), std::get<2>(result),
                  curr_leaf->level + 1);

              // add into worklist
              leafs.push(exhaust_set.get());

              // add into tree
              curr_leaf->children.push_back(std::move(exhaust_set));
            } else {
              is_exhaustive = false;
            }
          } else if (param_type == ASTContext::BOOLEAN) {
            auto result = boolean_pattern_exhaustiveness(
                pos, sub_case_patterns,
                curr_param_index + 1 ==
                    v_variant->parameter_types->formal_parameters.size());

            // 0: is_exhaustive, 1 : true_cases, 2: false_case,
            // 3: wildcard_cases
            if (std::get<0>(result)) {

              auto true_exhaust_set = std::make_unique<CaseTree>(
                  (BoolCaseTreeKey(true)), std::get<1>(result),
                  curr_leaf->level + 1);
              auto false_exhaust_set = std::make_unique<CaseTree>(
                  (BoolCaseTreeKey(false)), std::get<2>(result),
                  curr_leaf->level + 1);
              auto wildcard_exhaust_set = std::make_unique<CaseTree>(
                  (IdentCaseTreeKey()), std::get<3>(result),
                  curr_leaf->level + 1);

              // add into worklist
              leafs.push(true_exhaust_set.get());
              leafs.push(false_exhaust_set.get());
              leafs.push(wildcard_exhaust_set.get());

              // add into tree
              curr_leaf->children.push_back(std::move(true_exhaust_set));
              curr_leaf->children.push_back(std::move(false_exhaust_set));
              curr_leaf->children.push_back(std::move(wildcard_exhaust_set));
            } else {
              is_exhaustive = false;
            }
          } else if (param_type->getNodeType() == NodeType::sum_type) {
            auto result = variant_pattern_exhaustiveness(
                pos, dynamic_cast<const SumTypeNode *>(param_type),
                sub_case_patterns,
                curr_param_index + 1 ==
                    v_variant->parameter_types->formal_parameters.size());

            if (std::get<0>(result)) {
              for (const auto &[variant, variant_cases] : std::get<1>(result)) {
                auto variant_exhaust_set = std::make_unique<CaseTree>(
                    (VariantCaseTreeKey(variant)), variant_cases,
                    curr_leaf->level + 1);
                // add into worklist
                leafs.push(variant_exhaust_set.get());
                // add into tree
                curr_leaf->children.push_back(std::move(variant_exhaust_set));
              }
            } else {
              is_exhaustive = false;
            }
          }
        }
      } else if (is_last && v.second.size() > 1) {
        // Variant without parameters
        for (size_t i = 1; i < v.second.size(); i++) {
          logger_.warning(case_patterns.at(v.second.at(i))->pos(),
                          "Duplicate case (unreachable).");
        }
      }
    }
  }

  return std::make_tuple(is_exhaustive, variant_map, wildcard_cases);
}

std::tuple<bool, std::unordered_map<int, vector<u_int>>, vector<u_int>>
SemanticChecker::number_pattern_exhaustiveness(
    const FilePos pos, std::map<u_int, const PatternNode *> case_patterns,
    bool is_last) {

  bool is_exhaustive = true;
  std::unordered_map<int, vector<u_int>>
      literal_cases; // literal_value -> case_indecies
  vector<u_int> wildcard_cases;

  for (const auto &[i, pattern] : case_patterns) {
    if (is_last && wildcard_cases.size() > 0) {
      logger_.warning(pattern->pos(), "Unreachable case.");
      continue;
    }
    if (pattern->getNodeType() == NodeType::literal_pattern) {
      auto literal_pattern = dynamic_cast<const NumberPatternNode *>(pattern);

      if (is_last && literal_cases.contains(literal_pattern->value)) {
        logger_.warning(pattern->pos(), "Duplicate case (unreachable).");
        continue;
      }

      literal_cases[literal_pattern->value].push_back(i);
    } else if (pattern->getNodeType() == NodeType::ident_pattern) {
      wildcard_cases.push_back(i);
    } else {
      logger_.error(pattern->pos(), "UNEXPECTED KIND OF PATTERN");
      exit(EXIT_FAILURE);
    }
  }

  if (wildcard_cases.size() == 0) {
    is_exhaustive = false;
    logger_.warning(
        pos, "Non-exhaustive case-statement: Missing an INTEGER case with "
             "an identifier pattern.");
  }

  return std::make_tuple(is_exhaustive, literal_cases, wildcard_cases);
}

std::tuple<bool, vector<u_int>, vector<u_int>, vector<u_int>>
SemanticChecker::boolean_pattern_exhaustiveness(
    const FilePos pos, std::map<u_int, const PatternNode *> case_patterns,
    bool is_last) {
  bool is_exhaustive = true;
  vector<u_int> true_cases;
  vector<u_int> false_cases;
  vector<u_int> wildcard_cases;

  for (const auto &[i, pattern] : case_patterns) {
    if (is_last && (wildcard_cases.size() > 0 ||
                    (true_cases.size() > 0 && false_cases.size() > 0))) {
      logger_.warning(pattern->pos(), "Unreachable case.");
      continue;
    }

    if (pattern->getNodeType() == NodeType::literal_pattern) {
      auto bool_pattern = dynamic_cast<const BooleanPatternNode *>(pattern);

      if (bool_pattern->value) {
        if (is_last && true_cases.size() != 0) {
          logger_.warning(pattern->pos(), "Duplicate TRUE case (unreachable).");
          continue;
        }
        true_cases.push_back(i);
      } else {
        if (is_last && false_cases.size() != 0) {
          logger_.warning(pattern->pos(),
                          "Duplicate FALSE case (unreachable).");
          continue;
        }
        false_cases.push_back(i);
      }
    } else {
      wildcard_cases.push_back(i);
    }
  }

  if (wildcard_cases.size() == 0) {
    if (true_cases.size() == 0) {
      is_exhaustive = false;
      logger_.warning(pos, "Non-exhaustive case-statement: Missing TRUE case.");
    }
    if (false_cases.size() == 0) {
      is_exhaustive = false;
      logger_.warning(pos,
                      "Non-exhaustive case-statement: Missing FALSE case.");
    }
  }

  return std::make_tuple(is_exhaustive, true_cases, false_cases,
                         wildcard_cases);
}

void SemanticChecker::onCaseStatementEnd(CaseStatementNode &case_stmt) {
  if (case_stmt.get_cases()->size() == 0) {
    logger_.error(case_stmt.pos(),
                  "No cases specified in the case expression.");
    exit(EXIT_FAILURE);
  }

  std::map<u_int, const PatternNode *> case_patterns;
  for (u_int i = 0; i < case_stmt.get_cases()->size(); i++) {
    case_patterns[i] = case_stmt.get_cases()->at(i).first.get();
  }

  bool is_exhaustive = true;
  if (case_stmt.value->type->getNodeType() == NodeType::sum_type) {
    auto sum_type = dynamic_cast<const SumTypeNode *>(case_stmt.value->type);

    auto result = variant_pattern_exhaustiveness(case_stmt.pos(), sum_type,
                                                 case_patterns, true);
    is_exhaustive = std::get<0>(result);
  } else if (case_stmt.value->type == ASTContext::INTEGER) {
    auto result =
        number_pattern_exhaustiveness(case_stmt.pos(), case_patterns, true);
    is_exhaustive = std::get<0>(result);
  } else if (case_stmt.value->type == ASTContext::BOOLEAN) {
    auto result =
        boolean_pattern_exhaustiveness(case_stmt.pos(), case_patterns, true);
    is_exhaustive = std::get<0>(result);
  } else {
    logger_.error(case_stmt.value->pos(), "UNEXPECTED VALUE TYPE");
    exit(EXIT_FAILURE);
  }
}

unique_ptr<ExpressionNode> SemanticChecker::onUnaryExpression(
    const FilePos pos, unique_ptr<ExpressionNode> expr, const UnaryOpType op) {

  if (!expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  TypeNode *type;
  switch (op) {
  case UnaryOpType::u_not:
    expect_bool(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto boolean = dynamic_cast<BooleanExpressionNode *>(expr.get());

      int value = boolean->value;

      if (op == UnaryOpType::u_not)
        value = !boolean->value;

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  case UnaryOpType::plus:
  case UnaryOpType::minus:
    expect_number(expr.get());

    if (expr->getNodeType() == NodeType::number) {
      auto number = dynamic_cast<NumberExpressionNode *>(expr.get());

      int value = number->value;

      if (op == UnaryOpType::minus)
        value = -number->value;

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::INTEGER;
    break;
  default:
    logger_.error(expr->pos(), "Unknown operation");
    exit(EXIT_FAILURE);
  }

  return std::make_unique<UnaryExpressionNode>(pos, op, std::move(expr), type);
}

unique_ptr<ExpressionNode> SemanticChecker::onBinaryExpression(
    const FilePos pos, unique_ptr<ExpressionNode> left_expr,
    const BinaryOpType op, unique_ptr<ExpressionNode> right_expr) {

  if (!left_expr || !right_expr) {
    logger_.error(pos, "Undefined expression.");
    exit(EXIT_FAILURE);
  }

  if (left_expr->type != right_expr->type) {
    logger_.error(right_expr->pos(), "Expression types do not match.");
    exit(EXIT_FAILURE);
  }

  // check type according to op
  TypeNode *type;
  switch (op) {
  case BinaryOpType::b_and:
  case BinaryOpType::b_or:
    expect_bool(left_expr.get());
    expect_bool(right_expr.get());

    // Literals
    if (left_expr->getNodeType() == NodeType::boolean &&
        right_expr->getNodeType() == NodeType::boolean) {
      auto left_bool = dynamic_cast<BooleanExpressionNode *>(left_expr.get());
      auto right_bool = dynamic_cast<BooleanExpressionNode *>(right_expr.get());

      bool value;
      if (op == BinaryOpType::b_and)
        value = left_bool->value & right_bool->value;
      else
        value = left_bool->value | right_bool->value;

      return std::make_unique<BooleanExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  case BinaryOpType::plus:
  case BinaryOpType::minus:
  case BinaryOpType::times:
  case BinaryOpType::div:
  case BinaryOpType::divide:
  case BinaryOpType::mod:
    expect_number(left_expr.get());
    expect_number(right_expr.get());

    // Literals
    if (left_expr->getNodeType() == NodeType::number &&
        right_expr->getNodeType() == NodeType::number) {
      auto left_number = dynamic_cast<NumberExpressionNode *>(left_expr.get());
      auto right_number =
          dynamic_cast<NumberExpressionNode *>(right_expr.get());

      int value;
      switch (op) {
      case BinaryOpType::plus:
        value = left_number->value + right_number->value;
        break;
      case BinaryOpType::minus:
        value = left_number->value - right_number->value;
        type = ASTContext::INTEGER;
        break;
      case BinaryOpType::times:
        value = left_number->value * right_number->value;
        break;
      case BinaryOpType::div:
      case BinaryOpType::divide:
        value = left_number->value / right_number->value;
        break;
      case BinaryOpType::mod:
        value = left_number->value -
                right_number->value *
                    floor(float(left_number->value) / right_number->value);

        if (value < 0)
          value -= right_number->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }

      return std::make_unique<NumberExpressionNode>(pos, value);
    }

    type = ASTContext::INTEGER;
    break;
  default:
    if (left_expr->getNodeType() == NodeType::number &&
        right_expr->getNodeType() == NodeType::number) {
      auto left_number = dynamic_cast<NumberExpressionNode *>(left_expr.get());
      auto right_number =
          dynamic_cast<NumberExpressionNode *>(right_expr.get());

      bool value;
      switch (op) {
      case BinaryOpType::eq:
        value = left_number->value == right_number->value;
        break;
      case BinaryOpType::neq:
        value = left_number->value != right_number->value;
        break;
      case BinaryOpType::lt:
        value = left_number->value < right_number->value;
        break;
      case BinaryOpType::leq:
        value = left_number->value <= right_number->value;
        break;
      case BinaryOpType::gt:
        value = left_number->value > right_number->value;
        break;
      case BinaryOpType::geq:
        value = left_number->value >= right_number->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }
      return std::make_unique<BooleanExpressionNode>(pos, value);
    } else if (left_expr->getNodeType() == NodeType::boolean &&
               right_expr->getNodeType() == NodeType::boolean) {
      auto left_boolean =
          dynamic_cast<BooleanExpressionNode *>(left_expr.get());
      auto right_boolean =
          dynamic_cast<BooleanExpressionNode *>(right_expr.get());

      bool value;
      switch (op) {
      case BinaryOpType::eq:
        value = left_boolean->value == right_boolean->value;
        break;
      case BinaryOpType::neq:
        value = left_boolean->value != right_boolean->value;
        break;
      case BinaryOpType::lt:
        value = left_boolean->value < right_boolean->value;
        break;
      case BinaryOpType::leq:
        value = left_boolean->value <= right_boolean->value;
        break;
      case BinaryOpType::gt:
        value = left_boolean->value > right_boolean->value;
        break;
      case BinaryOpType::geq:
        value = left_boolean->value >= right_boolean->value;
        break;
      default:
        logger_.error(pos, "INTERNAL ERROR");
        exit(EXIT_FAILURE);
      }
      return std::make_unique<BooleanExpressionNode>(pos, value);
    }

    type = ASTContext::BOOLEAN;
    break;
  }

  return std::make_unique<BinaryExpressionNode>(pos, std::move(left_expr), op,
                                                std::move(right_expr), type);
}
unique_ptr<AssignmentNode>
SemanticChecker::onAssign(const FilePos pos, unique_ptr<IdentNode> ident,
                          vector<unique_ptr<SelectorNode>> selectors,
                          unique_ptr<ExpressionNode> expr) {
  TypeNode *lhs_type;
  const DeclarationNode *decl;
  try {
    lhs_type = symbol_table_.lookup_type(*ident, selectors);
    if (auto opt_decl = symbol_table_.lookup(*ident)) {
      if ((*opt_decl)->getNodeType() == NodeType::var_declaration ||
          (*opt_decl)->getNodeType() == NodeType::param_declaration) {
        decl = *opt_decl;
      } else {
        throw WrongNodeTypeException(
            *ident, "VarDeclarationNode / ParamDeclarationNode");
      }
    }
  } catch (LookupException &e) {
    logger_.error(e.get_node().pos(), e.what());
    auto ident_expr = std::make_unique<IdentExpressionNode>(
        pos, std::move(ident), std::move(selectors), decl, lhs_type, true);
    return std::make_unique<AssignmentNode>(pos, std::move(ident_expr),
                                            std::move(expr));
  }

  auto ident_expr = std::make_unique<IdentExpressionNode>(
      pos, std::move(ident), std::move(selectors), decl, lhs_type, true);
  if (ident_expr->type == nullptr) {
    logger_.error(pos, "'" + to_string(ident_expr.get()) +
                           "' has no associated type");
  } else if (expr->type == nullptr) {
    logger_.error(pos,
                  "'" + to_string(expr.get()) + "' has no associated type");
  } else if ((ident_expr->type != expr->type)) {
    logger_.error(pos, "Can not assign '" + to_string(expr.get()) + ": " +
                           to_string(expr->type) + "' to '" +
                           to_string(ident_expr.get()) + ": " +
                           to_string(ident_expr->type) + "'");
  }

  return std::make_unique<AssignmentNode>(pos, std::move(ident_expr),
                                          std::move(expr));
}

unique_ptr<ArrayIndexNode>
SemanticChecker::onArrayIndex(const FilePos pos,
                              unique_ptr<ExpressionNode> expr_) {

  if (expr_->type != ASTContext::INTEGER) {
    logger_.error(pos, "Array index is not an INTEGER");
  }
  if (auto number_expr =
          dynamic_unique_ptr_copy_cast<NumberExpressionNode>(expr_.get())) {

    if (number_expr->value < 0) {
      logger_.error(pos, "Array index cannot be negative");
    }
    return std::make_unique<ArrayIndexNode>(pos, std::move(number_expr));
  }

  return std::make_unique<ArrayIndexNode>(pos, std::move(expr_));
}

/* -------------------
 *  UTILITY FUNCTIONS
 * ------------------- */
void SemanticChecker::expect_unique(const IdentNode *ident,
                                    const DeclarationNode *value,
                                    bool this_scope) {
  if (auto decl = symbol_table_.lookup(*ident, this_scope)) {
    logger_.error(ident->pos(), "Identifier already declared here: " +
                                    to_string(decl.value()->pos()) + ":" +
                                    to_string(*decl));
    throw DuplicateFieldException(*ident);
  }
  symbol_table_.insert(*ident, value);
  return;
}

void SemanticChecker::expect_unique_within_scope(const IdentNode *ident,
                                                 const DeclarationNode *value) {
  expect_unique(ident, value, true);
}

void SemanticChecker::expect_bool(ExpressionNode *expr) {
  if (expr->type != ASTContext::BOOLEAN) {
    logger_.error(expr->pos(), "Expression should be of type " +
                                   to_string(ASTContext::BOOLEAN) + ".");
    // exit(EXIT_FAILURE);
  }
}

void SemanticChecker::expect_number(ExpressionNode *expr) {
  if (expr->type != ASTContext::INTEGER) {
    logger_.error(expr->pos(), "Expression should be of type " +
                                   to_string(ASTContext::INTEGER) + ".");
    // exit(EXIT_FAILURE);
  }
}

template <typename L, typename T>
unique_ptr<LiteralExpressionNode<T>>
SemanticChecker::clone_literal(LiteralExpressionNode<T> *literal) {
  if (literal) {
    return std::make_unique<L>(literal->pos(), literal->value);
  }

  return nullptr;
}
