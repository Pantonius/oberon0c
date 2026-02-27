MODULE NestedIfStatements;

VAR i: INTEGER;
  res: INTEGER;

BEGIN
  i := 2;
  IF i < 50 THEN
    IF i < 25 THEN
      IF i < 13 THEN
        res := 100
      ELSE
        res := -3
      END
    ELSE
      res := -2
    END
  ELSE
    res := -1
  END
END NestedIfStatements.
