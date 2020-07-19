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
    cr cr ." ---------- Now dump 10 lines from the file ----------------- " cr
    10 0 do                   \ print 10 lines of the file
	    s_line                  \ line buffer
	    132                     \ max 132 characters a line
	    fp @
	    fgets   
	    s" %s" swap 1 printf      \ print the line
    loop               
    cr ."     ." cr
    cr ."     ." cr
    cr ."     ." cr cr
    ." ------------------------------------------------------------ " cr
    fp @ fclose .s
    ;
                              
demo-fopen-fgets                              

cr
.( This file demos many things. )
.( 1. How to create a variable )
.( 2. How to create a buffer )
.( 3. How to open-close a file )
.( 4. How to read lines from a text file )

bye