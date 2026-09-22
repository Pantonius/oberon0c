MODULE SumType;

(* Correct syntax in declaration *)
TYPE MaybeInt = SUM None; Some(INTEGER) END;
(* sum type declarations consist of atleast one variant declaration *)
VAR i,j : MaybeInt;
  k : INTEGER;

BEGIN
  k := 42;
  (* ... and in both variant expressions *)
  i := MaybeInt.None; 
  j := MaybeInt.Some(TRUE) 

  (* moving on to case stmts *)
  (* also for sum types btw *)
END SumType.
