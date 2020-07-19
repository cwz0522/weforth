
.( Demo program to show you how to use if-else-then)   \  <========== '.(' is compile time message. Print this message when compiling.

\              (              \             .(
\             if           then           else
\             do           loop          +loop              i          break
\         ?break        recurse       variable              @              !


\ Demo how to use if-then-else
: test1      ( n1 n2 -- sum ) \  '(  )' is also a comment. Good to show stack diagram.
     s" ���Q���H give me two numbers :" 0 printf      \  use printf() to show message
     0 &TOS swap drop         \ 0 is dummy place holder, replaced to its address
     0 &TOS swap drop         \ 0 is dummy place holder, replaced to its address
     over over                \ copy the two arguments' address
     s" %d %d" -rot           \ "format string" addr1 addr2 <==== scanf()'s arguments are ready now
     2 scanf drop 		      \ use scanf() to get two numbers, drop scanf()'s error code
     + 100 > if 
     	." �A�̨�ӥ[�_�Ӥw�g�W�L�@�ʷ��F�I" cr 
     else 
        ." ���C�~�~�T�e�~�L�q�C" cr
     then
;

test1

bye

\  Results of this demod program
\
\  D:\My Dropbox\learnings\Forth\Ksanavm\WF06>wf06 demo-if-then-else.f .s
\  
\  ���Q���H give me two numbers :12 34
\  ���C�~�~�T�e�~�L�q�C
\  Empty stack
\  
\  D:\My Dropbox\learnings\Forth\Ksanavm\WF06>wf06 demo-if-then-else.f .s
\  
\  ���Q���H give me two numbers :34 56
\  ���C�~�~�T�e�~�L�q�C
\  Empty stack
\  
\  D:\My Dropbox\learnings\Forth\Ksanavm\WF06>wf06 demo-if-then-else.f .s
\  
\  ���Q���H give me two numbers :45 67
\  �A�̨�ӥ[�_�Ӥw�g�W�L�@�ʷ��F�I
\  Empty stack


