# Find the sum of the first N prime numbers

OUTPUT 'Please enter the number of primes to find:'
str <- USERINPUT
IF str /= '' THEN
  primes <- STRING_TO_INT(str)
ELSE
  OUTPUT 'Okay, I chose 100'
  primes <- 100
ENDIF
total <- 0
printout <- ''
n <- 2
WHILE primes > 0
  is_prime <- True
  p <- 2
  WHILE p * p <= n
    IF n MOD p = 0 THEN
      is_prime <- False
    ENDIF
    p <- p + 1
  ENDWHILE
  IF is_prime THEN
    printout <- printout + INT_TO_STRING(n) + ' '
    total <- total + n
    primes <- primes - 1
  ENDIF
  n <- n + 1
ENDWHILE

OUTPUT 'Sum: ' + INT_TO_STRING(total)
OUTPUT printout
