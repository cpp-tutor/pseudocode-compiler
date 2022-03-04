CONSTANT PI <- 3.14

SUBROUTINE circumference(radius : Real)
RETURN 2.0 * PI * radius
ENDSUBROUTINE

SUBROUTINE area(radius : Real)
RETURN PI * radius * radius
ENDSUBROUTINE

WHILE True
OUTPUT 'Please enter the radius:'
r <- STRING_TO_REAL(USERINPUT)
OUTPUT 'Circumference is: ' + REAL_TO_STRING(circumference(r))
OUTPUT 'Area is: ' + REAL_TO_STRING(area(r))
ENDWHILE
