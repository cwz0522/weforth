
: demo-scanf                            \ read formated data from stdin
     ." type in two decimal numbers n and m in this format ==> (n,m) : "
     dropall                            \ clear stack
     0 0                                \ put dummy values in stack
     s" (%d,%d)" &vm->stack dup 4 +     \ scanf arguments, read two values
     2 scanf                            \ read 2 values in this example         ( format a1 a2 ... n -- number )
     ;
demo-scanf .ss
stdin fflush                            \ It's good to do a fflush on stdin after scanf when you have got what you wnat. Avoid a lot of problems.

cr
: demo-sscanf                           \ note, double s!  Read data from the given string.
     ." Try to read two decimal numbers, (12345,67890), from a formated string." cr
     dropall                            \ clear stack
     0 0                                \ put dummy values in stack
     s" (12345,67890)"                  \ this is the input string
     s" (%d,%d)" &vm->stack dup 4 +     \ scanf arguments, read two values
     2 sscanf                           \ read 2 values in this example         ( stringz format a1 a2 ... n -- number )
     ;
demo-sscanf .ss

bye