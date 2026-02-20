MODULE ArrayToArrayAssignment;

VAR foo, bar : ARRAY 10 OF INTEGER;

(* Main program. *)
BEGIN
  foo[4] := 1;
  bar := foo
END ArrayToArrayAssignment.
