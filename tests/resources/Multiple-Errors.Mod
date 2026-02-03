(* Oberon-0 implementations of various sort algorithms. *)
MODULE Errors;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;
Dim = 5;                                    (* Double declaration *)

(* Array type *)
TYPE INTARRAY = ARRAY Dim OF INTEGER;
WRONGARRAY = ARRAY 4 < 5 OF Dim;            (* Not const and not type *)

(* Array to be sorted. *)
VAR a: ARRAY Dim OF INTARRAY;
WRONGRECORD: RECORD rec, rec: INTEGER END;  (* Double declaration in record *)

(* Initializes the array. *)
PROCEDURE Init();
CONST notconst = a;                         (* Not constant expression *)
VAR i: HEHEXD;                              (* Undeclared type *)
u: a;                                       (* Not a type *)
BEGIN
    b := Dim;                               (* Not declared *)
    i := 0;

    a[1][456] := 1;                         (* Bounds *)
    a[1].u := 1;                            (* Selector *)

    WHILE i < Dim DO
        a[i] := Dim - i;
        i := i + 1;
        i := -a;                            (* a is not a number *)
        i := ~a                             (* a is not a bool *)
    END
END Init;
(* Main program. *)
BEGIN
    Init
END Wrong.                                    (* Wrong Identifier *)
