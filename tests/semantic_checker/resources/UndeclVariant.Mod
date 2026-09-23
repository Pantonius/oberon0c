MODULE UndeclVariant;
TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;
  j : INTEGER;
BEGIN
  i := MaybeInt.None;
  CASE i OF
    Undeclared.Some(x) : j := x
    | MaybeInt.None : j := 0
  END
END UndeclVariant.
