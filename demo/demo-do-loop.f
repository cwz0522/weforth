
.( Demo program to show you how to use do-loop )   \  <========== '.(' is compile time message. Print this message when compiling.

\ Demo how to use do-loop
: test1      ( --  ) 
    5 0 do
    	i . cr            \ i is inner loop's count
    loop
    cr
    10 0 do
    	i . cr            \ i is inner loop's count
    2 +loop
;
test1 cr 

: test2  ( -- )
    10 2 do                                           \ loop 2,3,4 ... 9
    	10 2 do                                       \ loop 2,3,4 ... 9
    		j . ." x" i . ." =" i j * . ."   "        \ i is inner loop's count, j is outter loop's count. They are both forth standard command words.
    	loop
    	cr
    loop
;
test2

\  'break' and '?break' are supposed to break do-loop as C Language. But they are not stable so far on WF06.EXE hcchen5600 2010/06/07 09:40:53 
\  use if .... else .... then when you need to break a do-loop. A safe trick to terminate do-loop is shown below ...

: test3      ( --  ) 
    10 0 do
    	i . cr            \ i is inner loop's count
    	i 7 = if          \ when reached 7 then terminate the loop, how?
    	    cr ." %%%%%%%%%% See the original return stack %%%%%%%%%%%" cr
    	    .ss           \ view the return stack, see the 7 and 10 ?
    		r> r> drop dup >r >r     \ <==================================== this is the trick
    	    cr ." %%%%%%%%%%% After the trick %%%%%%%%%%%%%%%% " cr
    		.ss           \ view again the return stack. We have made original 7 and 10 become two 7s. That terminates the loop when do the 'then'.
    	then
    loop
;
test3

bye

