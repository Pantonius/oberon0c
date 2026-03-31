MODULE CaseBoolNonExhaustive;
(* same as CaseBool but with BOOLEAN type as the case expression *)

VAR i, j : BOOLEAN;

BEGIN
  j := FALSE;
  CASE i OF
    FALSE : j := FALSE
    (* non-exhaustive cases but with correct type *)
  END
END CaseBoolNonExhaustive.
