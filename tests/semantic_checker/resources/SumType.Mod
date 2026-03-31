MODULE SumType;

(* Correct syntax in declaration *)
TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i,j : MaybeInt;
  k : INTEGER;

BEGIN
  k := 42;
  (* ... and in both variant expressions *)
  i := MaybeInt.None; 
  j := MaybeInt.Some(k) 
END SumType.
