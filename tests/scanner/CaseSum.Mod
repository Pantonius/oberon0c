MODULE CaseSum;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;
  j : INTEGER;

BEGIN
  i := MaybeInt.Some(42);
  j := 0;
  CASE i OF
    MaybeInt.Some(42) : j := 8
    | MaybeInt.Some(x) : j := x
    | MaybeInt.None : j := 12
  END
END CaseSum.
