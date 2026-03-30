(* Oberon-0 implementations of various sort algorithms. *)
MODULE Sort0;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;
Bim = 3;

(* Array type *)
TYPE INTARRAY = ARRAY 5 OF INTEGER;

(* Array to be sorted. *)
VAR a: INTARRAY;

(* Main program. *)
BEGIN
  a[2] := a
END Sort0.
