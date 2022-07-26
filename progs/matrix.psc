# Transforms an (x,y) co-ordinate using 2-by-2 matrix multiplication

OUTPUT 'Please enter x co-ordinate:'
x <- STRING_TO_REAL(USERINPUT)
OUTPUT 'Please enter y co-ordinate:'
y <- STRING_TO_REAL(USERINPUT)
OUTPUT 'Please choose from:'
OUTPUT '  1) Reflection in x-axis'
OUTPUT '  2) Reflection in y-axis'
OUTPUT '  3) Scale by factor 2'
OUTPUT '  4) Rotation 90 degrees (anti-clockwise)'
transform <- STRING_TO_INT(USERINPUT)

matrix <- [[0.0, 0.0], [0.0, 0.0]]

IF transform = 1 THEN
  OUTPUT 'Reflection in x-axis'
  matrix[0][0] <- 1.0
  matrix[1][1] <- -1.0
ELSE IF transform = 2 THEN
  OUTPUT 'Reflection in y-axis'
  matrix[0][0] <- -1.0
  matrix[1][1] <- 1.0
ELSE IF transform = 3 THEN
  OUTPUT 'Scale by factor 2'
  matrix[0][0] <- 2.0
  matrix[1][1] <- 2.0
ELSE IF transform = 4 THEN
  OUTPUT 'Rotation by 90 degrees (anti-clockwise)'
  matrix[0][1] <- -1.0
  matrix[1][0] <- 1.0
ELSE
  OUTPUT 'Bad option'
ENDIF

x_t <- x * matrix[0][0] + y * matrix[0][1]
y_t <- x * matrix[1][0] + y * matrix[1][1]

OUTPUT 'Transformed: (' + REAL_TO_STRING(x_t) + ',' + REAL_TO_STRING(y_t) + ')'
