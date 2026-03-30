MODULE Assignment;

VAR i: INTEGER;
  a0, a1: ARRAY 4 OF INTEGER;
  r0, r1: RECORD
    f0: BOOLEAN;
    f1: INTEGER
  END;

BEGIN
i := 1;
a0[2] := 3;
a1 := a0;
r0.f0 := TRUE;
r0.f1 := 5;
r1 := r0
END Assignment.
