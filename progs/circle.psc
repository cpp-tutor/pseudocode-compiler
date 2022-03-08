CONSTANT PI <- 3.14

SUBROUTINE circumference(radius : Real)
RETURN 2.0 * PI * radius
ENDSUBROUTINE

SUBROUTINE area(radius : Real)
RETURN PI * radius * radius
ENDSUBROUTINE

FOR n <- 1 TO 4
OUTPUT 'Please enter the radius:'
r <- STRING_TO_REAL(USERINPUT)
OUTPUT 'Circumference is: ' + REAL_TO_STRING(circumference(r))
OUTPUT 'Area is: ' + REAL_TO_STRING(area(r))
ENDFOR
