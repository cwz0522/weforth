
\ this example is from KsanaVM forth lesson 16, thanks to Sir yap.

\ calculate factorial using recursion
: fac ( n -- fac_n )
  dup 2 < if
    drop 1                \ fac(0) = fac(1) = 1
  else 
    dup 1 - recurse *     \ fac(n) = n * fac(n-1)         when we want to call 'fac' here, use 'recurse' instead when 'fac' is still under construction.
  then 
;
cr .( fac of 10 is ) 10 fac . cr cr bye