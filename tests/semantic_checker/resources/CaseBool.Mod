MODULE CaseBool;
(* same as CaseInt but with BOOLEAN type as the case expression *)

VAR i: BOOLEAN;
  j: INTEGER;

BEGIN
  i := FALSE;
  j := 0;
  CASE i OF
    FALSE : j := 12
    | TRUE : j := 42
    (* exhaustive and correct types of cases *)
  END;
  (* codegen also works for this example *)
  WriteInt(j);
  WriteLn
END CaseBool.
