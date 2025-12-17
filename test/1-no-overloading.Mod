(* Oberon-0 implementations of various sort algorithms. *)
MODULE Sort0;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

(* Array type *)
TYPE INTARRAY = ARRAY Dim OF INTEGER;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTARRAY;

(* Swaps the two values passed as var-parameters. *)
PROCEDURE Swap(VAR a, b: INTEGER);
VAR t: INTEGER;
BEGIN
    t := a;
    a := b; 
    b := t
END Swap;

(* Applies the insertion-sort algorithm the the global array. *)
PROCEDURE Swap(VAR a, b: INTARRAY);
VAR i, j: INTEGER;
BEGIN
    i := 1;
    j := 2;
    Swap(i, j);
    b := a
END Swap;

(*
(* Main program. *)
*)
BEGIN
    Swap(1,2)
END Sort0.                                    (* Wrong Identifier *)
