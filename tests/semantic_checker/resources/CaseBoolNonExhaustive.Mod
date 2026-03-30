MODULE CaseBoolNonExhaustive;

VAR i, j : BOOLEAN;

BEGIN
  j := FALSE;
  CASE i OF
    FALSE : j := FALSE
  END
END CaseBoolNonExhaustive.
