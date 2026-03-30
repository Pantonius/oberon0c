MODULE Case;

VAR i: BOOLEAN;
  j: INTEGER;

BEGIN
  i := FALSE;
  j := 0;
  CASE i OF
    FALSE : j := 12
    | TRUE : j := 42
  END;
  WriteInt(j);
  WriteLn
END Case.
