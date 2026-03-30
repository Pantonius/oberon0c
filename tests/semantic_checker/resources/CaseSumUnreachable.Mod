MODULE CaseSum;

TYPE MaybeInt = SUM None; Some(INTEGER) END;
MaybeMaybeInt = SUM A; B(MaybeInt) END;

VAR i : MaybeInt;
  j : INTEGER;
  k : MaybeMaybeInt;

BEGIN
  i := MaybeInt.Some(42);
  k := MaybeMaybeInt.B(i);
  j := 0;
  CASE i OF
    MaybeInt.Some(42) : j := 8
    | MaybeInt.Some(42) : j := 7
    | MaybeInt.Some(x) : j := x
    | MaybeInt.Some(2) : j := 2
    | MaybeInt.None : j := 0
    | MaybeInt.None : j := 1
  END;
  CASE k OF
    MaybeMaybeInt.A : j := j * 2
    | MaybeMaybeInt.B(MaybeInt.None) : j := j / 2
    | MaybeMaybeInt.B(MaybeInt.Some(1)) : j := j * 1
  END
END CaseSum.
