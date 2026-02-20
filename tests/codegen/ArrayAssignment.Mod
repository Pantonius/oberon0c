MODULE ArrayAssignment;

VAR arr : ARRAY 10 OF INTEGER;

(* Main program. *)
BEGIN
  arr[5] := 1;
  arr[1] := arr[5]
END ArrayAssignment.
