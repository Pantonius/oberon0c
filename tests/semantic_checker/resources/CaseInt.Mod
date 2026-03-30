MODULE CaseInt;

VAR i, j : INTEGER;

BEGIN
  i := 2;
  j := 0;
  CASE i OF
    0 : j := 8
    | 1 : j := 12
    | x : j := x
  END;
  WriteInt(i);
  WriteLn;
  WriteInt(j);
  WriteLn
END CaseInt.
