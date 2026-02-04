# Milestone 6 - Language Extension for Oberon-0
Some initial ideas to think about until the deadline:
- [ ] Higher-Order Functions / Lambda Expressions
    - essentially the ProcedureType expanded
    - type declarations for ProcedureType
    - inherint return types, i.e. procedure calls now have type
        - which means procedure calls are also expressions
        - and addtional sema checks on type compatibility
        - new base type of VOID (which can be encoded as a nullptr)
    - #TODO declare before use is still in place, which should mean that circular types are still impossible (think about that a second longer --- I'm too lazy right now)
<!-- - [ ] Probabilistic Programming
    - add some additional constructs like:
        - RANDOM(from: INTEGER, to: INTEGER)
        - prob X > 0.2
        - and probabilistic variables?
    - #TODO think about the complexity of the task
-->
- [ ] Pattern Matching
    - add a MATCH expr1 ON CASE expr2 THEN ... construct
    - sema check: the type of expr1 and expr2 has to be equal
    - will need to add pattern matching logic
    - pattern matching may yield variable bindings for the case bodies (each of the CASEs is a new scope)
- [ ] [Sum Types](https://stanford-cs242.github.io/f19/lectures/03-2-algebraic-data-types.html#sum-types)
    - something like `TYPE Option = Some : INTEGER | None;`

## General Description
My oberon-0 language extension entales the addition of sum types, procedure return types and pattern matching. Sum type constructs like the `Maybe`/`Option` type in Rust and Haskell expand the expressivity of the language: `Maybe`/`Option` add an easy syntax for partial functions into the language. A classic example of a use case is the division function:
```pseudo
TYPE MaybeInt = Some(INTEGER) | None

PROCEDURE Divide(n, m: INTEGER): MaybeInt
...
END Divide.

BEGIN
    i = Divide(100, 0);
    MATCH i OF
        CASE Some(n) THEN WriteOut("GREAT!");
        CASE None THEN WriteOut(":(");
    END
END.
```
## Syntax Extension

### Sum Types
```
SumType = ident [":" type] { "|" ident [":" type] }
SumTypeExpression = ident { "(" expression {";" expression} ")" }
```

### Procedure Return Statements / Procedure Types
The procedure heading will have to be augmented to add an optional return type:
```
ProcedureHeading = "PROCEDURE" ident [FormalParameters] { ":" type }
```
If no return type is given, the `VOID` type (the procedure does not return any value) is implied.

The return statement introduces a new keyword `RETURN`:
```
ReturnStmt = "RETURN" expression
```

### Pattern Matching
A new match statement will have to be introduced, including case statements with patterns to match on.
```
MatchStmt = "MATCH" expression "ON" cases "END"
cases = "CASE" pattern "THEN" StatementSequence "END" {";" "CASE" pattern "THEN" StatementSequence "END" }
```
Patterns are either a wildcard (just some identifier) or an expression:
```
pattern = ident | expression
```

## Augmented Semantics
### Sum Types
Sum types need a semantic check for the existance of the declared variant types.

The variants of the sum type imply a type declaration, meaning: `TYPE MaybeInt = Some(INTEGER) | None` implies the declaration of `Some` as type `INTEGER -> MaybeInt` and `None` as type `MaybeInt`.

The construction of a sum type variant will need a type check for the fields, meaning: `Some(TRUE) : BOOLEAN -> MaybeInt` is incompatible with the declared `Some` type `INTEGER -> MaybeInt`.

### Procedures
The added return type of procedures will yield an implicit ProcedureType
```
ProcedureType = type "->" type ["->" type]
```
which consists of the input and output types (written in EBNF though they are not intended as syntactic constructs; just as internal constructs of the compiler). The last type is the return type, every other type is a formal parameter type.

The return statement will need a semantic type compatibility check with the declared return type.

### Pattern Matching
The cases of the match statement will need a semantic type compatibility check with the match expression. The following type annotated example would be invalid:
```pseudo
MATCH expression : MaybeInt OF
    CASE 4 : INTEGER THEN ...
END.
```

## Envisioned Code Shape


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
