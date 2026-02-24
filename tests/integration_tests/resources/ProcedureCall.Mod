MODULE ProcedureCall;

VAR i, j : INTEGER;

PROCEDURE Swap(VAR i, j: INTEGER);
  VAR t: INTEGER;
BEGIN
  t := i;
  i := j;
  j := t
END Swap;

BEGIN
  i := 1;
  j := 2;
  Swap(i, j)
END ProcedureCall.
