# Program which finds tries to find the letter "a" in a string

OUTPUT 'Please enter a word containing the letter "a":'
word <- USERINPUT
OUTPUT 'Word has ' + INT_TO_STRING(LEN(word)) + ' letters'
position <- POSITION(word, 'a')
IF position > 0 THEN
  OUTPUT 'Found "a" at position: ' + INT_TO_STRING(position)
ELSE
  OUTPUT 'Word does not contain the letter "a"'
ENDIF
