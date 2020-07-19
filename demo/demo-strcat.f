
create s_dest 100 allot

: hi s" hello " ;
: her s" Cindy " ;
: invit s" want to go Starbucks with me? " ;

s_dest hi strcat drop
s_dest puts

s_dest her strcat drop
s_dest puts

s_dest invit strcat drop
s_dest puts

bye
