# Milestone 6 - Language Extension for Oberon-0

## General Description
My oberon-0 language extension entales the addition of sum types and pattern matching. Sum type constructs like the `Maybe`/`Option` type in Rust and Haskell expand the expressivity of the language: `Maybe`/`Option` add an easy syntax for partial functions into the language. A classic example of a use case is the division function:
```pseudo
TYPE MaybeInt = Some(INTEGER) | None

PROCEDURE Divide(n, m: INTEGER): MaybeInt
...
END Divide.

BEGIN
    i = Divide(100, 0);
    MATCH i ON
        CASE Some(n) THEN WriteOut("GREAT!");
        CASE None THEN WriteOut(":(");
    END
END.
```

### Open Questions
- can we check exhaustiveness of the match cases?
    - alternatively add a mandatory default
- actual intermediate representation of sum types

## Syntax Extension

### Sum Types
Sum types yield a new type besides the builtin INTEGER, BOOLEAN, ARRAY and RECORD types. They also imply a new expression `SumTypeExpression` for instantiating SumType values.
Each variant is an identifier with an optional list of parameter types.
```
SumType = SumTypeVariant { "|" SumTypeVariant }
SumTypeVariant = ident ["(" type { "," type } ")"]
SumTypeExpression = ident "::" ident { "(" expression {";" expression} ")" }
```
The SumTypeExpression consists of the identifier for the sum type, the identifier for the variant and, optionally, the values for each of the parameters.

The following syntax is adapted:
```
type = ident | ArrayType | RecordType | SumType
expression = SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] | SumTypeExpression
```

### Pattern Matching
A new `MATCH` statement will have to be introduced, including `CASE` statements with patterns to match on.
```
MatchStatement = "MATCH" expression "ON" cases "END"
cases = "CASE" pattern "THEN" StatementSequence "END" {";" "CASE" pattern "THEN" StatementSequence "END" }
```
Patterns are either a wildcard (just some identifier that will be bound to the actual value) or an expression:
```
pattern = ident | expression
```

The following syntax is adapted:
```
statement = [assignment | ProcedureCall | IfStatement | WhileStatement | MatchStatement]
```

## Augmented Semantics
### Sum Types
Sum types need a semantic check for the existence of the declared parameter types.

The variants of the sum type imply a type declaration, meaning: `TYPE MaybeInt = Some(INTEGER) | None` implies the declaration of `MaybeInt::Some` as type `INTEGER -> MaybeInt` and `MaybeInt::None` as type `MaybeInt`.

The instantiation of a sum type variant will need a type check for the parameters, meaning: `MaybeInt::Some(TRUE) : BOOLEAN -> MaybeInt` is incompatible with the declared type of `MaybeInt::Some : INTEGER -> MaybeInt`.

### Pattern Matching
The cases of the match statement will need a semantic type compatibility check with the match expression. The following annotated example would be invalid:
```pseudo
MATCH expression : MaybeInt ON
    CASE 4 : INTEGER THEN ...
END
```

Each case statement should be unique within the match statement.

Possibly I can try to check for exhaustiveness of the match statement. If too complex (which it probably is in general), I can disallow recursive patterns such as:
```
MATCH expression ON
    CASE Some(a) THEN ...
    ...
END
```
(taking all the fun out of it)

In that example `a` would not be interpreted as a (named) wildcard, but instead as a declared and instantiated variable value.

Some initial thoughts on the matter of exhaustiveness for the non-recursive case:
- The base cases for pattern matching are expressions of type: INTEGER, BOOLEAN
    - a match statement on an INTEGER expression cannot be exhaustive
    - a match statement on a BOOLEAN expression is only exhaustive if both the TRUE and FALSE cases are checked (in this sense BOOLEAN can be recontextualized to be a builtin SumType and use the matching rules for SumType)
- The rest (ARRAY, RECORD, SumType) is derived from those basic types and each needs special consideration of exhaustiveness
    - a match statement on an ARRAY expression is only exhaustive if each combination of its type variants is represented by a case
        - an ARRAY of INTEGER cannot be exhaustive
    - a match statement on an RECORD expression is only exhaustive if each combination of its field type variants is represented by a case
        - a RECORD with an INTEGER field cannot be exhaustive
    - a match statement on a SumType expression is only exhaustive if it is exhaustive for each variant
        - a match statement is exhaustive for a variant, if each combination of its parameter type variants is represented by a case
            - a variant with an INTEGER argument cannot be exhaustive
        - therefore a SumType cannot be exhaustive if it has a variant with an INTEGER argument

The recursive case would be different in the constructed types (ARRAY, RECORD, SumType), because wildcards would be allowed. Arguably, recursive patterns can be recontextualized as match statements within match statements. In that case the compiler would need to translate flattened (recursive) patterns into a hierarchical structures (first matching on the first argument, then matching on each subsequent argument of the ARRAY/RECORD/Variant.

## Envisioned Code Shape
### Sum Types
In general a SumType is represented in the AST as a `SumTypeNode : TypeNode` with a `vector<TypeNode *>` holding all parameter types.

One possibility for realizing sum types in LLVM is utilizing the [PointerSumType](https://llvm.org/doxygen/classllvm_1_1PointerSumType.html) construct (which I am not familiar with as of now).

### Pattern Matching
*considering recursive patterns*

Some brief thoughts, because I am not that familiar with the LLVM IR for pattern matching. From what I gather:
- switch statements would be realized with the [`llvm::SwitchInst`](https://llvm.org/doxygen/classllvm_1_1SwitchInst.html), but only allow for matching an expression against concrete values; substitutions are not possible
- I will probably need to translate the pattern matching into a series of conditional branches. The following oberon pseudo code:
```pseudo
MATCH a ON
    CASE Some(2) THEN ... END;
    CASE Some(3) THEN ... END;
    CASE Some(b) THEN ... END;
    CASE None THEN ... END
END
```
would need to be translated into more atomic checks (not written in LLVM IR yet):
```pseudo
if a.type == Some then:
    if a.firstField == 2 then:
        ...
        @goto after
    else if a.firstField == 3 then:
        ...
        @goto after
    else
        b = a.firstField
        ...
        @goto after
if a.type == None then:
    ...
    @goto after
@lbl after
```
This structure of if-elsif-else would already arise in the AST, when calling the semantic checker on a match statement. The semantic checker checks for exhaustiveness and de-flattens recursive patterns to be expressed as recursive match statements. Each case of the transformed match statement is then an if/elseif/else branch.

## Examples and Tests
### Valid
```
MODULE Example1;

VAR i : INTEGER;

PROCEDURE Zero() : INTEGER;
BEGIN
    RETURN 0
END Zero;

BEGIN
    i = Zero();
END Example1.
```

```
MODULE Example2;

TYPE MaybeInt = Some : INTEGER | None;

VAR b : BOOLEAN;

PROCEDURE Frac(n, m : INTEGER) : MaybeInt;
BEGIN
    IF n % m = 0 THEN RETURN Some(n / m)
    ELSE None END
END Frac;

BEGIN
    MATCH Frac(10, 3) ON
        CASE Some(a) THEN b := True END;
        CASE None THEN b := False END
    END
END Example2.
```

### Invalid
```
MODULE Example3;

TYPE MaybeInt = Some : INTEGER | None;

VAR b : BOOLEAN;

PROCEDURE Frac(n, m : INTEGER) : MaybeInt;
BEGIN
    IF n % m = 0 THEN RETURN Some(n / m)
    ELSE None END
END Frac;

BEGIN
    MATCH Frac(10, 3) ON
        CASE 4 THEN b := True END;
    END
END Example3.
```
- The case patterns need to have a compatible type to the match expression.
- The match statement on a sum type expression needs to include all variants of that sum type.
