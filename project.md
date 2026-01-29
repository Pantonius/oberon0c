# Milestone 6 - Language Extension for Oberon-0

Some initial ideas to think about until the deadline:
- [ ] Higher-Order Functions / Lambda Expressions
    - essentially the ProcedureType expanded
    - type declarations for ProcedureType
    - possibly return types, i.e. procedure calls now have type
        - which means procedure calls are also expressions
        - and addtional sema checks on type compatibility
    - #TODO declare before use is still in place, which should mean that circular types are still impossible (think about that a second longer --- I'm too lazy right now)
- [ ] Probabilistic Programming
    - add some additional constructs like:
        - RANDOM(from: INTEGER, to: INTEGER)
        - prob X > 0.2
        - and probabilistic variables?
    - #TODO think about the complexity of the task
- [ ] Pattern Matching
    - add a MATCH var ON CASE ... construct
- [ ] Enumeration Type
    - may be connected with pattern matching
    - may be connected with higher-order functions
    - something like `ENUM Option OF Some(e: Type) OR None`

## General Description

## Syntax Extension

## Augmented Semantics

## Envisioned Code Shape

## Examples and Tests
