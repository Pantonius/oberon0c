MODULE CaseBool;
(* same as CaseInt but with BOOLEAN type as the case expression *)

VAR i: BOOLEAN;
  j: INTEGER;

BEGIN
  i := TRUE;
  j := 0;
  CASE i OF
    FALSE : j := 12
    | TRUE : j := 42
    (* same with booleans *)
    (* exhaustive and correct types of cases *)
  END;
  (* codegen also works for this example *)
  (* on to the interesting part -- case stmts over sum type expressions *)
  WriteInt(j);
  WriteLn
END CaseBool.
