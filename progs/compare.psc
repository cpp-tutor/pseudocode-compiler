# Compare two Integers with all the boolean tests supported

SUBROUTINE BOOL_TO_STRING(condition : Boolean)
  IF condition THEN
    str <- 'True'
  ELSE
    str <- 'False'
  ENDIF
  RETURN str
ENDSUBROUTINE

OUTPUT 'Please enter an Integer:'
a <- STRING_TO_INT(USERINPUT)
OUTPUT 'Please enter one more Integer:'
b <- STRING_TO_INT(USERINPUT)
OUTPUT 'Sum: ' + INT_TO_STRING(a + b)
difference <- a - b
IF difference < 0 THEN
  difference <- -difference
ENDIF
OUTPUT 'Difference: ' + INT_TO_STRING(difference)
OUTPUT 'Product: ' + INT_TO_STRING(a * b)
OUTPUT 'Result of division: ' + INT_TO_STRING(a DIV b) + ', remainder: ' + INT_TO_STRING(a MOD b)

OUTPUT INT_TO_STRING(a) + ' = ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a = b)
OUTPUT INT_TO_STRING(a) + ' /= ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a /= b)
OUTPUT INT_TO_STRING(a) + ' < ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a < b)
OUTPUT INT_TO_STRING(a) + ' <= ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a <= b)
OUTPUT INT_TO_STRING(a) + ' >= ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a >= b)
OUTPUT INT_TO_STRING(a) + ' > ' + INT_TO_STRING(b) + ': ' + BOOL_TO_STRING(a > b)
