MODULE SumType;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;

BEGIN
  i := MaybeInt.None
END SumType.
