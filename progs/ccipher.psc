# Caesar cipher (ROT13)

OUTPUT 'Please enter a string:'
str <- USERINPUT
IF str = '' THEN
  str <- 'Orjner bs CYBGF!'
ENDIF
cipher <- ''
FOR letter IN str
  IF letter >= 'A' AND letter <= 'Z' THEN
    cipher <- cipher + CODE_TO_CHAR((CHAR_TO_CODE(letter) - CHAR_TO_CODE('A') + 13) MOD 26 + CHAR_TO_CODE('A'))
  ELSE IF letter >= 'a' AND letter <= 'z' THEN
    cipher <- cipher + CODE_TO_CHAR((CHAR_TO_CODE(letter) - CHAR_TO_CODE('a') + 13) MOD 26 + CHAR_TO_CODE('a'))
  ELSE
    cipher <- cipher + letter
  ENDIF
ENDFOR
OUTPUT 'Cipher: ' + cipher
