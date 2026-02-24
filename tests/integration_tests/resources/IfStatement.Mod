MODULE IfStatement;

VAR i: BOOLEAN;

BEGIN
  i := FALSE;
  IF TRUE THEN
    i := TRUE
  ELSIF FALSE THEN
    i := FALSE
  END
END IfStatement.
