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

## Syntax Extension
```
ProcedureType = type -> type { -> type }
ReturnStmt = "RETURN" expression
SumType = ident [":" type] { "|" ident [":" type] }
SumTypeExpression = ident { "(" expression {";" expression} ")" }
MatchStmt = "MATCH" expression "ON" cases "END"
cases = "CASE" pattern "THEN" StatementSequence "END" {";" "CASE" pattern "THEN" StatementSequence "END" }
pattern = ident | expression
```

## Augmented Semantics

## Envisioned Code Shape

## Examples and Tests
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
