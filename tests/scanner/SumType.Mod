MODULE SumType;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i,j : MaybeInt;

BEGIN
  i := MaybeInt.None;
  j := MaybeInt.Some(43)
END SumType.
