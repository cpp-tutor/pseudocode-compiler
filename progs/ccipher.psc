# Caesar cipher (ROT13)

OUTPUT 'Please enter a string:'
str <- USERINPUT
IF str = '' THEN
  str <- 'Beware of PLOTS!'
ENDIF
cipher <- ''
FOR letter IN str
  code <- CHAR_TO_CODE(letter)
  IF code >= CHAR_TO_CODE('A') AND code <= CHAR_TO_CODE('Z') THEN
    code <- (code - CHAR_TO_CODE('A') + 13) MOD 26 + CHAR_TO_CODE('A')
  ENDIF
  IF code >= CHAR_TO_CODE('a') AND code <= CHAR_TO_CODE('z') THEN
    code <- (code - CHAR_TO_CODE('a') + 13) MOD 26 + CHAR_TO_CODE('a')
  ENDIF
  cipher <- cipher + CODE_TO_CHAR(code)
ENDFOR
OUTPUT 'Cipher: ' + cipher
