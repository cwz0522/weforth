
cr cr
.( Testing stricmp [string1 string2 -- compare] case ignored )

: test-compare ( -- comapre ) s" abcd" s" ABCDE" stricmp ; 
test-compare 
.s 
.( 0 is equal, >0 string1 bigger, <0 string2 bigger )

bye