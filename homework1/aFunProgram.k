-- Joe Gildner
-- FunProgram
-- This function gets a char from serial input and prints it to the screen
-- until the 'q' char is entered on the firs

code FunProgram


  function main ()
      var c: char
      c = GetCh()

      while c != 'q'
        printChar(c)
        c = GetCh()
      endWhile

      debug

    endFunction

endCode
