MODULE IfElsifElseStatement;

VAR i: INTEGER;
  res: INTEGER;

BEGIN
  i := 1;
  IF i < 0 THEN
    res := i
  ELSIF i # 1 THEN
    res := 1
  ELSE
    res := 42
  END
END IfElsifElseStatement.
