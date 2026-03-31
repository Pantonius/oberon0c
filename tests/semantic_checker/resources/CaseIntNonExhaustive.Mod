MODULE CaseInt;

VAR i, j : INTEGER;

BEGIN
  j := 0;
  CASE i OF
    0 : j := 8
    | 1 : j := 12
    (* non-exhaustive cases but with correct types *)
  END
END CaseInt.
