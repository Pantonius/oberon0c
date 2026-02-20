MODULE RecordFieldAssignment;

VAR person : RECORD
  id: INTEGER;
  adult: BOOLEAN
END;

(* Main program. *)
BEGIN
  person.id := 2;
  person.adult := TRUE
END RecordFieldAssignment.
