# Guess a number between 1 and 100 in seven or fewer attempts

CONSTANT MIN <- 1
CONSTANT MAX <- 100
str <- USERINPUT
IF str /= '' THEN
  number_to_find <- STRING_TO_INT(str)
ELSE
  number_to_find <- RANDOM_INT(MIN,MAX)
ENDIF
OUTPUT 'Try to find: ' + INT_TO_STRING(number_to_find)

guess <- 0
attempts <- 0
low <- MIN
high <- MAX
WHILE guess /= number_to_find
  guess <- low + (high - low) DIV 2
  attempts <- attempts + 1
  OUTPUT 'Guessed: ' + INT_TO_STRING(guess)
  IF guess > number_to_find THEN
    OUTPUT 'Too high.'
    high <- guess - 1
  ELSE IF guess < number_to_find THEN
    OUTPUT 'Too low.'
    low <- guess + 1
  ELSE
    OUTPUT 'Got it in ' + INT_TO_STRING(attempts) + ' attempts!'
  ENDIF
ENDWHILE
