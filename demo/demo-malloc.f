
create s_dest 100 allot

: hi s" hello world !! " ;
s_dest hi strcat drop
s_dest puts

100 malloc constant buffer

buffer s_dest 80 memcpy drop
buffer puts


bye
