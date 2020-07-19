cr
.( This file demos many things. )
.( 1. How to create a variable , forth 'variable' command word )
.( 2. How to create a buffer , forth allot command word )
.( 3. How to open-close a file , C fopen and fclose )
.( 4. How to read lines from a text file , C fgets. And check feof and ferror.  )
.( 5. How to check EOF during using a file, C feof )

.( Now I am going to open this .f file , read lines and print them. )
cr .( Press Enter to continue . . . )
getc drop

variable fp
create s_line 1024 allot      \ create a new word s_line and give it 1024 bytes space. s_line returns the starting address of the buffer.
                       
: demo-fopen-fgets  ( -- )
    argv 4 + @                \ argv[1] is this .f file's filename
    cr cr ." filename is " dup puts
    s" rt"  								  \ read only text file mode
    fopen                     \ open the file
    fp !                      \ save the return FILE* to fp
    ." FILE* is " fp @        \ show fp
    s" 0x%08x " swap 1 printf
    cr cr ." ------------------ Now dump the file ----------------- " cr
    1000 0 do                 \ print all lines of this file, I bet it won't be longer than 1000
	    s_line                  \ line buffer
	    20                      \ max 132 characters a line , I found this number doesn't matter !! this is a fgets's bug!
	    fp @ fgets  
      fp @ feof if 
          drop
          ." ------------------------------------------------------------ " cr
          s" End of file reached, ferror=%d" 
          fp @ ferror 
          1 printf cr
          r> r> drop dup >r >r   \ terminate the do-loop
      else    
	        s" %s" swap 1 printf      \ print the line
	    then
    loop               
    fp @ fclose drop
    ;
                              
demo-fopen-fgets                              

bye