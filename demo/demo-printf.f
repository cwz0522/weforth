cls                                        \ clear screen

: hello ." Hello world !! " cr ;           \ Forth standard print string method
hello                                      \ run it , the output goes to stdout so it can be redirected to a file.
.s

: demo-printf    ( format a1 a2 ... n -- )
        s" Hello world !! This is %d.%c%c" \ format string
        2010                               \ argument(s)
        13 10                              \ CR LF
        3 printf                           \ give number of arguments and then call printf
        ;                                                                           

demo-printf																 \ run it , the output goes to stdout so it can be redirected to a file.
.s

: demo-fprintf   ( FILE* format a1 a2 ... n -- error )
        stderr                             \ standard FILE* to stderr
        s" Hello world !! print to stderr. %d + %d = %d %c%c"     \ format string
        123                                \ argument(s)
        456
        over over +
        13 10                              \ CR LF
        5 fprintf                          \ give number of arguments and then call printf
        ;                                                                           
demo-fprintf							   \ run it , the output goes to stderr so it will NOT be redirected to a text file, try it !!
.s

bye