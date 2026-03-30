MODULE Case;

VAR i, j : BOOLEAN;

BEGIN
  j := FALSE;
  CASE i OF
    FALSE : j := TRUE
    | TRUE : j := FALSE
  END
END Case.
