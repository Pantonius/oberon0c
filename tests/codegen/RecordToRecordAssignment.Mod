MODULE RecordToRecordAssignment;

VAR person, personCloned : RECORD
  id: INTEGER;
  adult: BOOLEAN
END;

(* Main program. *)
BEGIN
  person.id := 2;
  person.adult := TRUE;
  personCloned := person
END RecordToRecordAssignment.
