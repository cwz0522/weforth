
create buffer 1024 allot        \ create a buffer

12345678 buffer 10 itoa drop    \ itoa() translates the number into a string
buffer puts                     \ show the string

12345678 buffer 16 itoa drop    \ itoa() translates the number into a string but in hexdecimal 
buffer puts                     \ show the string
                                                                                               
123 456 buffer 10 ltoa drop
buffer puts
                                                                                               
bye