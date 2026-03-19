MODULE ProcedureDeclaration;

(* Length of the array to be sorted. *)
CONST Dim = 2 * 10;

(* Array type *)
TYPE INTARRAY = ARRAY Dim OF INTEGER;

(* Array to be sorted. *)
VAR a: INTARRAY;

PROCEDURE QSort(l, r: INTEGER);
  VAR i, j, x: INTEGER;
  BEGIN
      i := l;
      j := r;
      x := a[(r + l) DIV 2];
      WHILE i <= j DO
          WHILE a[i] < x DO i := i + 1 END;
          WHILE x < a[j] DO j := j - 1 END;
          IF i <= j THEN
              i := i + 1;
              j := j - 1
          END
      END;
      IF l < j THEN QSort(l, j) ELSIF i < r THEN QSort(i, r) END;
      IF i < r THEN QSort(i, r) END
  END QSort;
END ProcedureDeclaration.
