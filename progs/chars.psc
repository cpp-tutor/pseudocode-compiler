# Output all ASCII 7-bit (UTF-7) characters from 32 to 127, in a table

FOR page <- 32 TO 112 STEP 16
  s <- ''
  FOR char <- 0 TO 15
    s <- s + CODE_TO_CHAR(page + char) + ' '
  ENDFOR
  OUTPUT s
ENDFOR
