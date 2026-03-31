# Implementation
## Sum Types
### AST
In general a SumType is represented in the AST as a `SumTypeNode : TypeNode` with a `ProcedureType` holding all parameter types.

### CodeGen / LLVM
I opted to realize sum types as `llvm::StructType` of shape `{i32, [max_size * i8]}`, where the first field `i32` is the variant tag -- an integer number representing the saved sum type variant -- and the second field `[max_size * i8]` holds the realized variant. `max_size` is the size of the largest variant in bytes.

## Pattern Matching
### Parser
Patterns are realized as `PatternNode` and all have `type` that is:
- derived for `IdentPatternNode`
- or implied for `NumberPatternNode`, `BooleanPatternNode`, and `VariantPatternNode`

### CodeGen / LLVM
The pattern matching is translated into a series of conditional branches. As of now the simplest example work: Case statements over literal types (`CASE i : INTEGER OF ... END`) and case statements over sum type expressions with literal parameter types (`CASE i : MaybeInt OF ... END`).

## Overall Changes
### Parser
Added `sum_type()`, `pattern(...)`, `case_statement()`
Adapted `type()` and `expression()`

Also adapted `IdentExpressioNode` to hold actual parameters for the variant expression case.

### Semantic Checker
Added semantic checks for:
- sum type: `onSumType(...)`
- patterns `onNumberPatternNode(...)`, `onBooleanPatternNode(...)`, `onVariantPatternNode(...)`, `onIdentPattern(...)`
- case statements `onCaseStatementStart(...)`, `onCaseStatementEnd(...)`, `onCaseStatementCaseStart(...)`, `onCaseStatementCaseEnd(...)`
    - also exhaustiveness checks to `onCaseStatementEnd(...)` realized as `number_pattern_exhaustiveness(...)`, `boolean_pattern_exhaustiveness(...)`, `variant_pattern_exhaustiveness(...)`

Adapted `onIdentExpression(...)` to accomodate for variant expressions

### CodeGen
- sum type:
    - `visit(SumTypeNode &)`: which constructs the sum type to be a `llvm::StructType` consisting of a variant tag (`i32`) and payload (`[max_size * i8]`) with the realized variant object.
- case statement:
    - `visit(CaseStatementNode *)`: which translates the case statement structure into an if-elsif-else structure, but only for literals.
    - `literals(...)`, `variants(...)`, `literal_pattern(...)`, and `ident_pattern(...)`: aid in translating the case statement into a branching structure
    - and `getCaseValueField(...)`: aids in retrieving a field from a sum type value

Adapted `visit(IdentExpressioNode &)` for variant expressions.
