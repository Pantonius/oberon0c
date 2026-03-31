MODULE CaseSumNonExhaustiveParam;

TYPE Compound = SUM None; Some(INTEGER, BOOLEAN) END;
VAR i : Compound;
  j : INTEGER;

BEGIN
  i := Compound.Some(42; TRUE);
  j := 0;
  CASE i OF
    Compound.Some(42; FALSE) : j := 8
    | Compound.Some(42; TRUE) : j := 2
    | Compound.None : j := 0
    (* non-exhaustive in Some variant -- lacking atleast Some(x; FALSE) and Some(x; TRUE) *)
  END
END CaseSumNonExhaustiveParam.
