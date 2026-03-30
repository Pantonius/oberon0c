MODULE SumType;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i,j : MaybeInt;
  k : INTEGER;

BEGIN
  k := 42;
  i := MaybeInt.None;
  j := MaybeInt.Some(k)
END SumType.
