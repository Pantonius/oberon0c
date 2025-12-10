(* Oberon-0 implementations of various sort algorithms. *)
MODULE Sort0;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

(* Array type *)
TYPE INTARRAY = ARRAY Dim OF INTEGER;

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTARRAY;
WRONGRECORD: RECORD rec: INTEGER END;  (* Double declaration in record *)

(* Initializes the array. *)
PROCEDURE Init();
VAR i : INTEGER;
BEGIN
    i := 0;

    a[1][456] := 1;                         (* Bounds *)
    a[1].u := 1;                            (* Selector *)

    WHILE i < Dim DO
        a[i] := Dim-i;
        i := i + 1;
        i := -a;                            (* a is not a number *)
        i := ~a                             (* a is not a bool *)
    END
END Init;


(* Main program. *)
BEGIN
    Init;
    QuickSort
END Sort0.                                    (* Wrong Identifier *)
