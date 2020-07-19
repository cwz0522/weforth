
: str  s" Hello World !!       " ;
: sprintf-test 
     str puts
     str 6 + s" %d+%d=%d" 2 3 over over + 3 sprintf str puts .s 
     ;
sprintf-test
