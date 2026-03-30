MODULE CaseSumNonExhaustiveVariant;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;
  j : INTEGER;

BEGIN
  i := MaybeInt.Some(42);
  j := 0;
  CASE i OF
    MaybeInt.Some(x) : j := x
  END
END CaseSumNonExhaustiveVariant.
