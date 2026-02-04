# Milestone 6 - Language Extension for Oberon-0
Some initial ideas to think about until the deadline:
<!-- - [ ] Higher-Order Functions / Lambda Expressions -->
<!--     - essentially the ProcedureType expanded -->
<!--     - type declarations for ProcedureType -->
<!--     - inherint return types, i.e. procedure calls now have type -->
<!--         - which means procedure calls are also expressions -->
<!--         - and addtional sema checks on type compatibility -->
<!--         - new base type of VOID (which can be encoded as a nullptr) -->
<!--     - #TODO declare before use is still in place, which should mean that circular types are still impossible (think about that a second longer --- I'm too lazy right now) -->
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
```
SumType = SumTypeVariant { "|" SumTypeVariant }
SumTypeVariant = ident ["(" type { "," type } ")"]
SumTypeExpression = ident "::" ident { "(" expression {";" expression} ")" }
```
The SumTypeExpression consists of the identifier for the sum type, the identifier for the variant and, optionally, the values for each of the parameters.

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
Patterns are either a wildcard (just some identifier that will be bound to the actual value) or an expression:
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
MATCH expression : MaybeInt ON
    CASE 4 : INTEGER THEN ...
END.
```

Possibly I can try to check for exhaustiveness of the match cases. If too complex, I can resort to a mandatory default case / disregard non-exhaustive match statements.

## Envisioned Code Shape
### Sum Types
One possibility for realizing sum types is utilizing the [PointerSumType](https://llvm.org/doxygen/classllvm_1_1PointerSumType.html) construct of LLVM.

### Procedures

### Pattern Matching
Some brief thoughts, because I am not that familiar with the LLVM IR for pattern matching. From what I gather:
- switch statements would be realized with the [`llvm::SwitchInst`](https://llvm.org/doxygen/classllvm_1_1SwitchInst.html), but only allow for matching an expression against concrete values; substitutions are not possible
- I will probably need to translate the pattern matching into a series of conditional branches. The following oberon pseudo code:
```pseudo
MATCH a ON
    CASE Some(2) THEN ... END;
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
    else
        b = a.firstField
        ...
        @goto after
if a.type == None then:
    ...
    @goto after
@lbl after
```
More basic:
- Each match is a sequence of case translations with a unique `@lbl after` to jump to after executing the match statement and the chosen case body.
- Each case is a sequence of nested `if` statements where
    - variants are matched by looking at the type of the expression (if variants are realized as their own \<TypeNode\*, llvm::type\> pair)
    - literals are matched by checking equality
    - (named) wildcards are matched by adding a substitution / variable binding

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
