
cls
.( demo 'execute' and ' tick )

cr .( ' someword  returns the someword's CFA  [ -- CFA ]  )

3 7         \ put 3 and 7 in stack" cr
' +         \ get + command's CFA. The ' reads 'tick' is a standard word." cr

.( we have 3 7 and CFA of + in stack now )
.s

.( CFA is actually the execution entry point of a forth command word. 'execute' command executes it [ CFA -- ... ] )
execute     \ execute '+' comamnd's CFA equals to execute '+' command " cr
.s          \ check the result, should be 10 " cr cr
.( see, 3 + 7 = 10 , correct !! )

.( you usually won't use these words. However, these are standard forth command words )

bye