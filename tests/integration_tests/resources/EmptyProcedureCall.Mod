MODULE EmptyProcedureCall;

VAR i, j : INTEGER;

PROCEDURE Sq();
BEGIN
  i := i * i
END Sq;

BEGIN
  i := 4;
  Sq
END EmptyProcedureCall.
