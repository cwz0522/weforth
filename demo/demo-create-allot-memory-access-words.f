.( include dump.f for the memory dump command word 'd' )
include dump.f ?exit
: print-08x-hex s" %08x " swap 1 printf ;     \ make a method to print dword hex
cls
.( demo how to use 'create' , 'allot' , 'c@' , 'c!' , 'fill' , 'here' )

.( create buffer1, buffer2 )
                                  \ 'create' [ -- ] create nextword
                                  \ 'allot'  [ 4*n -- ] allocate space from dictionary sapce from 'here' with the given length.
create buffer1 32 allot           \ create a buffer and give it 32 bytes space
                                  
cr create buffer2 32 allot           \ create another buffer and give it 32 bytes space

.( dump buffer1)
buffer1 d dropall                 \ dump buffer1
.( dump buffer2)
buffer2 d dropall                 \ dump buffer1

cr .( fill bufffer1 with 0x55 )
                                  \ 'fill' [ address count c -- ]
buffer1 32 0x55 fill

cr .( change bufffer1 2nd byte from 0x55 to 0xAA )
0xAA
buffer1 1 +                       \ buffer1 last byte
c!                                \ store 0xAA to the given address

.( check buffer1)
buffer1 d drop
.( check buffer2)
buffer2 d drop                                  

cr .( copy 16 bytes from buffer1 to buffer2 )
                                  \ 'memcpy' [ b_dest b_source size -- b_dest ]
  buffer2 buffer1 16 memcpy drop

.( check buffer1)
buffer1 d drop
.( check buffer2)
buffer2 d drop                                  

cr .( read buffer2 2nd byte )
buffer2 1 + c@  print-hex-byte cr

bye  
                 