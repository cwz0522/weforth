
\ &vm->greetingswitch is 0 if autoexec.f is found. So it's always 0 (off) at this moment.

: decide-greeting-switch  ( -- ) argc 2 > not dup if &vm->greetingswitch ! else drop then ;  \ To show or not to show, you can make your own rules ! Here, I am just follow F Language's rule.
decide-greeting-switch .greeting cr                                    \ .greeting refers to &vm->greetingswitch. To turn it on "1 &vm->greetingswitch !", to turn it off "0 &vm->greetingswitch !"

: ?stop-include-this-line ( flag -- ) if 0 &vm->tib ! then ;           \ clear &vm->tib will stop the recent Eval() that is to abort including the recent line.
: ?stop-include-this-file ( flag -- ) if 1 &vm->stopinclude ! then ;   \ turn vm->stopinclude on, s_include() will break the loop
\ 1 &vm->greetingswitch !                                              \ 0 to turn off .( messages ), others to turn on. Up to you.

.( running autoexec.f )
.( include common.f08)  include common.f08  ?exit
.( include dump.f01)    include dump.f01    ?exit
.( include help.f01)    include help.f01    ?exit
.( include utility.f01) include utility.f01 ?exit

findword common.f08 not ?stop-include-this-file     \ If not defined then abort including because we need too many words that were defined in common.f08

: autoexec      ( -- ... )                          \ [ -- ... ] F Language style command line
                \ Include first argv[1] as a script file
                argc 1 - 0 > if
                   argv 4 + @ dup                   \ [ argv[1] argv[1] ]  follow F Language rule, first argument is script file name (argv 4 + @)
                   s" nul" over s" -"               \ argv[1] argv[1] nul argv[1] -
                   stricmp 0 =                      \ argv[1] argv[1] nul is-?
                   -rot                             \ argv[1] is-? argv[1] nul
                   stricmp 0 =                      \ argv[1] is-? isnul?
                   or if
                       drop
                   else
                     sinclude if argv 4 + @ 0 printf ." file not found !" cr then
                   then
                then
                \ Go through follow argv[2,3,...]
                argc 2 > if
                  argc 2 - 0 do
                    8 4 i * + argv + @ eval
                  loop
                then
                ;
autoexec
argc 2 > ?bye                                       \ argc > 2 means we already have command line commands to execute. Therefore bye-bye after finished them.
.( autoexec.f done )
