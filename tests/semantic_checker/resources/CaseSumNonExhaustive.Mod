MODULE CaseSumNonExhaustive;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;
  j : INTEGER;

BEGIN
  i := MaybeInt.Some(42);
  j := 0;
  CASE i OF
    MaybeInt.Some(42) : j := 8
    | MaybeInt.Some(2) : j := 2
  END
END CaseSumNonExhaustive.
