MODULE Array;

TYPE INTARRAY = ARRAY 20 OF INTEGER;

VAR a: INTARRAY;

BEGIN
  a[-3] := 3;
  a[21] := 4;
  a[3] := TRUE
END Array.
