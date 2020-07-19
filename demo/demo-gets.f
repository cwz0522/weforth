     
: str s" --------------------------- " ;  \ dummy string   
: demo-gets  ( stringz -- stringz or NULL when failed )
     ." What's your name ? " 
     str
     gets                               \ gets leaves the string pointer or NULL
     ." Hello " str puts                           \ see if str have been changed
     ;
demo-gets
.s
bye     
