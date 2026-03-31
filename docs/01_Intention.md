# Language Extension for Oberon-0: Sum Types and Case Statements

## General Description
My oberon-0 language extension entales the addition of sum types and pattern matching. Sum type constructs like the `Maybe`/`Option` type in Haskell and Rust expand the expressivity of the language: `Maybe`/`Option` add an easy syntax for partial functions into the language. A classic example of a use case is the division function:
```pseudo
TYPE MaybeInt = SUM Some(INTEGER); None END;

PROCEDURE Divide(n, m: INTEGER; VAR ret : MaybeInt)
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

## Syntax Extension

### Sum Types
Sum types yield a new type besides the builtin INTEGER, BOOLEAN, ARRAY and RECORD types. They also imply a new expression `VariantExpression` for instantiating SumType values.

Each variant is an identifier with an optional list of parameter types.
```
SumType = SUM SumTypeVariant { ";" SumTypeVariant } END
VariantDeclaration = ident ["(" type { "," type } ")"]
type = ident | ArrayType | RecordType | SumType
```

The VariantExpression consists of the identifier for the sum type, the identifier for the variant and, optionally, the values for each of the parameters.
```PROPOSED
VariantExpression = ident "." ident { "(" expression {";" expression} ")" }
expression = SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression] | VariantExpression
```

In the final implementation the VariantExpression syntax is merged into the factor index as follows:
```FINAL
expression = SimpleExpression [("=" | "#" | "<" | "<=" | ">" | ">=") SimpleExpression]
factor = ident selector ["(" expression { ";" expression } ")"] | integer | "(" expression ")" | "~" factor.
```
Making it 1-look-ahead.

### Pattern Matching
In accordance with the Oberon-7 specification, I introduced the `CASE` statement.
```
CaseStatement = "CASE" expression "OF" case { "|" case } "END"
case = pattern ":" StatementSequence
```
Patterns are either a wildcard (just some identifier that will be bound to the actual value), a literal value (number or boolean) or a variant pattern (two identifier seperated by a period and some patterns for the parameters of the variant):
```
pattern = ident {"." ident ["(" pattern { ";" pattern } ")"]} | number | boolean
```
As of now, there is no syntax for pattern matching on arrays or records (and therefore no semantic checking or code generation for them).

The following syntax is adapted:
```
statement = [assignment | ProcedureCall | IfStatement | WhileStatement | CaseStatement]
```

## Augmented Semantics
### Sum Types
Sum type declarations are checked for the existence of the declared parameter types.

The instantiation of a sum type variant is checked for:
- the existence of a sum type with the first given identifier,
- the existence of a variant with the second given  identifier,
- the correct number of parameters, according to the variant declaration
- the correct parameter types

### Pattern Matching
The cases of the CaseStatement are type checked with the match expression. The following annotated example would be invalid:
```pseudo
VAR expression : MaybeInt;
...

CASE expression OF
    4: ...
END
```

Ideally the cases should be exhaustive and unique. This is checked recursively following simple constraints per pattern:
- The base cases for pattern matching are expressions of type: INTEGER, BOOLEAN
    - a CaseStatement on an INTEGER expression cannot be exhaustive, unless matched with a wildcard pattern
    - a CaseStatement on a BOOLEAN expression is only exhaustive if both the TRUE and FALSE cases are checked or a wildcard pattern is used
- For SumType expressions the logic is derived from those basic types
    - a CaseStatement on a SumType expression is only exhaustive if it is exhaustive for each variant or if it is matched to a wildcard pattern
        - a CaseStatement is exhaustive for a variant, if each combination of its parameter type variants is represented by a case

The implementation is lacking a complete sum type exhaustiveness check. As of now each parameter pattern is only checked for exhaustiveness within its field. The combined exhaustiveness is not checked.


## Examples and Tests
### Valid
```
MODULE Example1;

VAR b : BOOLEAN;
    i : INTEGER;

BEGIN
    i := 4;
    CASE i OF
        10: b := True
        | x: b := False
END Example1.
```

```
MODULE Example2;

TYPE MaybeInt = SUM Some(INTEGER); None END;

VAR b : BOOLEAN;
    frac : MaybeInt;

PROCEDURE Frac(n, m : INTEGER; VAR ret : MaybeInt);
BEGIN
    IF n % m = 0 THEN ret := Some(n / m)
    ELSE ret := None END
END Frac;

BEGIN
    Frac(10, 3, frac);
    CASE frac OF
        Some(a): b := True
        | None: b := False
    END
END Example2.
```

### Invalid
```
MODULE Example3;

TYPE MaybeInt = SUM Some(INTEGER); None END;

VAR b : BOOLEAN;
    frac : MaybeInt;

PROCEDURE Frac(n, m : INTEGER; VAR ret : MaybeInt);
BEGIN
    IF n % m = 0 THEN ret := Some(n / m)
    ELSE ret := None END
END Frac;

BEGIN
    Frac(10, 3, frac)
    CASE frac OF
        4: b := True
    END
END Example3.
```
- The case patterns need to have a compatible type to the match expression.
- The CaseStatement on a sum type expression needs to include all variants of that sum type.
