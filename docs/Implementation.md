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
The pattern matching is translated into a series of conditional branches. For now only pattern matching against literal values works.

## Overall Changes
### Parser

### Semantic Checker

### CodeGen

