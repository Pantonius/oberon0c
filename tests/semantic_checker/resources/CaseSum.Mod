MODULE CaseSum;

(* correct sum type declaration *)
TYPE MaybeInt = SUM None; Some(INTEGER) END;
VAR i : MaybeInt;
  j : INTEGER;

BEGIN
(* and correct use of variant expressions *)
  i := MaybeInt.Some(42);
  j := 0;
  CASE i OF
    MaybeInt.Some(42) : j := 8
    | MaybeInt.Some(2) : j := 2
    | MaybeInt.Some(x) : j := x
    | MaybeInt.None : j := 0
    (* for case stmts over sum types code generation only works if the parameter types are not sum types *)
    (* sadly *)
    (* exhaustive cases with correct types of patterns *)
  END;
  (* Which means this works: *)
  WriteInt(j); WriteLn
END CaseSum.
