# Calculate circumference and area of a circle using formulas

CONSTANT PI <- 3.1416

SUBROUTINE circumference(radius : Real)
  RETURN 2.0 * PI * radius
ENDSUBROUTINE

SUBROUTINE area(radius : Real)
  RETURN PI * radius * radius
ENDSUBROUTINE

OUTPUT 'Please enter the radius of the circle:'
r <- STRING_TO_REAL(USERINPUT)
OUTPUT 'Circumference is: ' + REAL_TO_STRING(circumference(r))
OUTPUT 'Area is: ' + REAL_TO_STRING(area(r))
