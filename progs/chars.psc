FOR page <- 32 TO 112 STEP 16
s <- ''
FOR char <- 0 TO 15
s <- s + CODE_TO_CHAR(page + char) + ' '
ENDFOR
OUTPUT s
ENDFOR
