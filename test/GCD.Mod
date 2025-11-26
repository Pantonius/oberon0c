MODULE GCD;


  TYPE INTARRAY1 = ARRAY 5 OF INTEGER;
  INTARRAY2 = ARRAY 10 OF INTARRAY1;
  RECORD1 = RECORD a, b: INTEGER END;
  VAR 
    a: INTARRAY2;
    record2: RECORD c, d: RECORD1 END;
    x, y, z: INTEGER;
    flag: INTEGER;

  PROCEDURE GCD(b: INTEGER; VAR result: INTEGER);
  VAR 
    temp: INTEGER;
  BEGIN
    a[1][2] := 1;
    a[1][456] := 1;
    a[1].u := 1;
    record2.c.a := 2;
    record2.c[5] := 2;
    WHILE b # 0 DO
      temp := b
    END;
    result := a
  END GCD;

BEGIN
  (* Test variable assignments and expressions *)
  x := 42;
  y := 18;

  (* Call the GCD procedure *)
  GCD(x, y, z);

  (* Test conditionals *)
  IF z = 6 THEN
    flag := 1
  ELSE
    flag := 0
  END;

  (* Test loops *)
  x := 10;
  WHILE x > 0 DO
    x := x - 1
  END;

  (* Test nested structures *)
  IF flag = 1 THEN
    x := x + 1;
    IF x > 5 THEN
      x := 5
    END
  END
END GCD.
