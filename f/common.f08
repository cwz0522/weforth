
findword common.f08 ?stop-include-this-file  \ if defined then skip
findword message_switch ?stop-include-this-line &vm->greetingswitch @ constant message_switch
0 &vm->greetingswitch !          \ 0 to turn off .( messages ), others to turn on. Up to you.

.( compiling common.f08 )

\
\ Forth Virtual Machine Common Words for WF06
\
\ I am so happy writing this file. It's easy to create Forth words yet they're very powerful and beautiful !! h.c. chen 2008/11/07 12:59:52 PM
\

\ 先給你一個小驚喜！
alias     bye       再見      \ 中文嘜也通！ 與 F Language 不同，現在大小寫有分。 同一個 word 想用不同的大小寫時 alias 一下就可以了。

alias     mod       %         \ '%' operator of F Language and C Language. mod is Forth word, '%' becomes an alias.
alias     break     leave     \ 'break' is from C Language. F-PC calls it 'leave'
alias     ?break    ?leave    \ We don't have C Language's 'continue' so far. Forth words are usually small, 'continue' 比較少用。  -hcchen5600 2008/11/09 19:58
alias     findword  '         \ the ' read 'tick', it's a standard forth command word. It returns the CFA of the given word name. If the CFA is executable then we can use 'execute' to execute it.

\ KsanaVM Forth basic words

4 constant CELLSIZE           \ Win32 or EFI Shell are both 32 bits systems

\ Forth basic words

: cells   ( n -- 4n )
          CELLSIZE * 
          ;
alias     cells     CELLS     

          \ Forth's traditional 'and' 'or' are bitwise operations (other than boolean).
          \ If true is always -1 and false is 0 then no problem. But our users are familar with C Language, they need idiot proof.
          \ 為此，我刻意把 'not' 的結果限定為 0 或 -1。要做 bitwise 1's complement 請用 -1 xor。 h.c. chen 2008/11/07 13:37:46 PM

: AND     ( a b -- a && b )   \ boolean version 大寫版
          not not             \ standardize to boolean
          swap
          not not             \ standardize to boolean
          and
          ;

: OR      ( a b -- a || b )   \ boolean version 大寫版
          not not             \ standardize to boolean
          swap
          not not             \ standardize to boolean
          or
          ;

alias     not       NOT       \ 大寫版 'not' itself is a boolean operation

: <=      ( n1 n2 -- n1<=n2 )
          > NOT
          ;
alias     <=        =<

: >=      ( n1 n2 -- n1>=n2 )
          < NOT
          ;
alias     >=        =>

: <>      ( n1 n2 -- n1<>n2 )
          = NOT
          ;
alias     <>        !=

: dropall ( ... -- )          \ drops entire data stack. I clone this instruction from F Language. h.c. chen 2008/11/07 13:05:53 PM
          -1 &vm->sp !
          ;


\ Forth standard words

: ?  ( a1 -- )                \ print the value in the given address
          @ . ;

: 2dup    ( n1 n2 -- n1 n2 n1 n2 )
          over over           \ Note! Forth words themself are compatible. But you better know whether is your system 16 or 32 bits?
          ;

: max     ( n1 n2 -- max )
          2dup                \ n1 n2 n1 n2
          <                   \ n1 n2 (n1<n2)
          if swap             \ n2 n1
          then
          drop                \ n2 if n1<n2
          ;                   \ n1 if n1>=n2

: min     ( n1 n2 -- min )
          2dup                \ n1 n2 n1 n2
          >                   \ n1 n2 (n1>n2)
          if swap             \ n2 n1
          then
          drop                \ n2 if n1>n2
          ;                   \ n1 if n1<=n2

: negate  ( n1 -- -n1 )
          0                   \ n1 0
          swap                \ 0 n1
          -                   \ 0-n1
          ;

: abs     ( n1 -- |n1| )
          dup                 \ n1 n1
          0<                  \ n1 n1<0
          if negate then
          ;

: j       ( -- j )            \ a copy of idx_j value
                              \ R[ limit_k idx_k limit_j idx_j limit_i idx_i ret_addr -- ]  ret_addr is not existing when i j k are called from command promt or the first level colon word.
                              \    0       1     2       3     4       5     6              however, i j k must be called from a colon word, j k themself are colon words too therefore we always have the ret_addr
          &vm->rstack         \ &limit_k <--- base address of return stack
          &vm->rp @           \ &limit_k 6
          3 -                 \ &limit_k 3
          CELLSIZE *          \ &limit_k 3*CELLSIZE
          +                   \ &limit_k + 3*CELLSIZE
          @                   \ get j value
          ;

: k       ( -- k )            \ a copy of idx_k value
          &vm->rstack
          &vm->rp @
          5 -                 \ &limit_k 1
          CELLSIZE *
          +                   \ &limit_k + CELLSIZE
          @
          ;

: rot     ( 1 2 3 -- 2 3 1 )  \ Rotate 3 cells   hcchen5600 2010/05/27 13:06:22
          >r swap r> swap ;

: -rot    ( 1 2 3 -- 3 1 2 )  \ Rotate 3 cells   hcchen5600 2010/05/27 13:06:26
          swap >r swap r> ;

: &TOS    ( -- &TOS )         \ get top of stack address
          &vm->sp @           \ how many stack cells so far?
          CELLSIZE *          \ total length
          &vm->stack + ;      \ add base address

: ?exit   ( yes -- )          \ if TOS != 0 then terminate the program. Return TOS value to errorlevel.
          if exit then ;

\ C stdio words
: printf      1 + dup 00 c-function-entry call-c-function drop ;  \ [ stringz a1 a2 a3 ... num-of-a -- ]
: fprintf     2 + dup 01 c-function-entry call-c-function ;       \ [ FILI* format ... n -- ]    \ int fprintf ( FILE * stream, const char * format, ... );
: sprintf     2 + dup 02 c-function-entry call-c-function drop ;  \ [ stringz format ... n -- ]    \ int sprintf ( char * str, const char * format, ... );
\ : vsprintf          03 c-function-entry call-c-function ;
: puts            1 1 04 c-function-entry call-c-function drop ;  \ [ stringz -- ] tested ok
\ : fputs         2 2 05 c-function-entry call-c-function ;
: putc     stdout 2 2 06 c-function-entry call-c-function drop ;  \ [ char -- ]
\ : fputchar      2 2 07 c-function-entry call-c-function ;       \ [ FILE* char -- ]
: scanf       1 + dup 08 c-function-entry call-c-function ;       \ [format ... n -- number ]   int  scanf ( const char * format, ... )
\ : fscanf            09 c-function-entry call-c-function ;
: sscanf      2 + dup 10 c-function-entry call-c-function ;       \ int sscanf ( const char * str, const char * format, ...)
: fflush          1 1 11 c-function-entry call-c-function ;       \ int fflush ( FILE * stream )
: gets            1 1 12 c-function-entry call-c-function ;       \ [ char* -- char* or NULL when failed ]    char * gets ( char * str );
: fgets           3 3 13 c-function-entry call-c-function ;       \ [ target-string max-length FILE* -- target-string or NULL when failed ] char *fgets(char *s, int n, FILE *stream) return the string or NULL when failed
: getc            0 &TOS swap drop stdin swap                     \ [ -- char ]  get a key , user need to press <Enter>
                  2 2 14 c-function-entry call-c-function ;
\ : fgetc         2 2 14 c-function-entry call-c-function ;
\ : fgetchar      0 0 15 c-function-entry call-c-function ;       \ [ -- char ]
: fopen           2 2 16 c-function-entry call-c-function ;       \ [ path-string mode-string -- FILE* ]  FILE * fopen(const char * path,const char * mode);
: fclose          1 1 17 c-function-entry call-c-function ;       \ [ FILE* -- OK=0 ]
: feof            1 1 18 c-function-entry call-c-function ;       \ [ FILE* -- eof? ]
: ferror          1 1 19 c-function-entry call-c-function ;       \ [ FILE* -- error# ]
: strncpy         3 3 20 c-function-entry call-c-function ;       \ [ s_dest s_source size -- s_dest ]
: strupr          1 1 21 c-function-entry call-c-function ;       \ [ string -- string ]
: strlwr          1 1 22 c-function-entry call-c-function ;       \ [ string -- string ]
: strcat          2 2 23 c-function-entry call-c-function ;       \ [ s_dest s_source -- s_dest ]
: strlen          1 1 24 c-function-entry call-c-function ;       \ [ stringz -- len ] tested ok
: strcpy          2 2 25 c-function-entry call-c-function ;       \ [ s_dest s_source -- s_dest ]
: strcmp          2 2 26 c-function-entry call-c-function ;       \ [ string1 string2 -- compare ] 0 equal, >0 string1 bigger, <0 string2 bigger
: stricmp         2 2 27 c-function-entry call-c-function ;       \ [ string1 string2 -- compare ] 0 equal, >0 string1 bigger, <0 string2 bigger
: strerror        1 1 28 c-function-entry call-c-function ;       \ [ error# -- string] The strerror function maps errnum to an error-message string, returning a pointer to the string */
: memcpy          3 3 29 c-function-entry call-c-function ;       \ [ b_dest b_source size -- b_dest ]
: malloc          1 1 30 c-function-entry call-c-function ;       \ [ size -- buffer* ]      extern void *malloc(unsigned int num_bytes);
: itoa            3 3 31 c-function-entry call-c-function ;       \ [ n string radix -- string ]       char *itoa(int value, char *string, int radix);
\ : ltoa          3 3 32 c-function-entry call-c-function ;       \ [ long_high long_low string radix -- string ]  char *ltoa(long value,char *string,int radix);
: atoi            1 1 33 c-function-entry call-c-function ;       \ [ string -- int ]   int atoi(const char *nptr);
\ : atol              34 c-function-entry call-c-function ;
\ : ultoa             35 c-function-entry call-c-function ;
\ : atexit            36 c-function-entry call-c-function ;
\ : freopen           37 c-function-entry call-c-function ;       \ FILE *freopen(char *filename, char *type, FILE *stream);
\ : fdopen            38 c-function-entry call-c-function ;
: fseek           3 3 39 c-function-entry call-c-function ;       \ [ FILE* offset fromwhere -- error ] int fseek(FILE *stream, long offset, int fromwhere);  return 0=OK
: ftell           1 1 40 c-function-entry call-c-function ;       \ [ FILE* -- pointer ]  long ftell(FILE *stream); Gets the current position of a file pointer.
: ungetc          2 2 41 c-function-entry call-c-function ;       \ [ char FILE* -- char or EOF=failed ] int ungetc(char c, FILE *stream); Pushes a character back onto the stream.

\ Below section all dropped. I give it up! These functions are too annoying. Now I think nobody would use WFXX to control screen anyway !!
\ : GetStdHandle    1 0 45 c-function-entry call-c-function ;  \ [ handle-id -- HANDLE ]
\ .( "d:\Program Files\Microsoft Visual Studio 8\VC\PlatformSDK\Include\WinBase.h" )
\ -10 GetStdHandle constant hSTD_OUTPUT_HANDLE       \ #define STD_INPUT_HANDLE    ((DWORD)-10)
\ .( after hSTD_OUTPUT_HANDLE )
\ -11 GetStdHandle constant hSTD_INPUT_HANDLE        \ #define STD_OUTPUT_HANDLE   ((DWORD)-11)
\ .( after hSTD_INPUT_HANDLE )
\ -12 GetStdHandle constant hSTD_ERROR_HANDLE        \ #define STD_ERROR_HANDLE    ((DWORD)-12)
\ .( after hSTD_ERROR_HANDLE )
\ : newgotoxy             &vm->stack hSTD_OUTPUT_HANDLE 2 0
\               42 c-function-entry call-c-function ;  \ [ &coord -- error ] Set cursor position. VS 2005's function SetConsoleCursorPosition
\ .( after gotoxy )
\ : GetConsoleScreenBufferInfo 43 c-function-entry call-c-function ;
\ : SetConsoleTextAttribute    44 c-function-entry call-c-function ;
\ : CloseHandle                46 c-function-entry call-c-function ;
\ See http://www.cplusplus.com/forum/beginner/4234/ for example C programs for gotoxy() and wherexy()

: WORDS   ( -- )              \ dump all words' names, plaint version
          &vm->nword @ 0 do
              s" %s " &vm->vectors 52 i * + 1 printf  \ vm->vectors = 41 characters + 4 xt + 4 immediate = 49 ==> [49 , 4n] = 52
          loop cr
          ." Total " &vm->nword @ . ." words " cr
          ;

: off     ( addr -- )         \ make it FALSE
          0 swap !
          ;

: on      ( addr -- )         \ make it TRUE
          -1 swap !
          ;

alias     &vm->debugmode  debugmode
                              \ 'debugmode on' enables a lot of un-safe warnings but it does not enter internel debug shell
                              \ it's a good idea to set debugmode on and off in your program for run time un-safe checkings

: g       ( -- )              \ 'g' command of debug.exe, exits the KsanaVM internal debug shell entered from *debug* or *bp*
          debugmode off       \ it just turns off the debugmode. When my internal debug shell finds the mode is off, it exits
          ;                   \ the shell and so resumes the program.

: .ss      ( -- )              \ 更豪華版的 .s splendor version
          ." Indx     Decimal   Hexdecimal      Decimal   Hexdecimal" cr
          &vm->sp @           \ [sp0]               \ Get sp immediately before pushing anything in.
          &vm->rp @ over over \ [sp0 rp0 sp0 rp0]
          max                 \ [sp0 rp0 max  ]
          -1 swap do          \ [sp0 rp0      ] vm->sp = -1 when it's empty。
              i -1 <= if
                  s"         Data stack 0x%08x   Return stack 0x%08x "
                  &vm->rstack &vm->stack 2 printf cr
              else
                  s" %4d" i 1 printf                     \ [sp0 rp0]
                  over i                                 \ [sp0 rp0 sp0 i]
                  >= if
                     s" %12d   0x%08X"
                     &vm->stack i CELLSIZE * + @         \ read the stack cell
                     dup
                     2 printf
                  else
                     s"                          " 0 printf
                  then
                  dup i                                  \ [sp0 rp0 rp0 i]
                  >= if
                     s"  %12d   0x%08X"
                     &vm->rstack i CELLSIZE * + @        \ read the rstack cell
                     dup
                     2 printf
                  then
                  cr
              then
          -1 +loop                                       \ [sp0 rp0]
          drop drop
          ;

: ?bye    ( yes -- )
          if bye then
          ;

variable  _words.column
: words   ( -- )              \ dump all words' names, even more beautiful version
          _words.column off                      \ start from column 0
          &vm->nword @ 0 do
              &vm->vectors 52 i * +             \ [ strz ] this word's name field, stringz. 52 is sizeof(vm->vectors[])
              dup strlen                        \ [ strz len ] format stringz strlen(stringz)
              dup 14 max 14 =                   \ [ strz 14? ]
              if
                 drop s" %15s"                  \ [ strz " %15s" ]
                 _words.column dup @ 15 + dup rot !
                 75 > if cr 15 _words.column ! then
              else
                 dup 29 max 29 =
                 if
                    drop s" %30s"               \ %30s
                    _words.column dup @ 30 + dup rot !
                    75 > if cr 30 _words.column ! then
                 else
                    drop s" %45s"               \ %45s
                    _words.column dup @ 45 + dup rot !
                    75 > if cr 45 _words.column ! then
                 then
              then
              swap
              1 printf
          loop
          cr ." Total " &vm->nword @ . ." words " cr
          ;

.( define .b print 8-bits hex )
: .b   ( byte -- )
    $ff and s" %02x" swap 1 printf
;
.( define .w print 16-bits hex )
: .w   ( 16-bits -- )
    $ffff and s" %04x" swap 1 printf
;
.( define .d print 32-bits hex )
: .d   ( 32-bits -- )
    s" %08x" swap 1 printf
;

\ WINAPI words

.( loadlibrary kernel32.dll )
.( init winapi 'sleep' command word )
loadlibrary kernel32.dll ?exit          \ 'loadlibrary' is from Sir yap's demo in KsanaVM forth tutorial.
kernel32.dll @ 1 0 winapi Sleep ?exit     
: sleep Sleep drop ;                    \ 'sleep' [ milliseconds -- ] 

.( init winapi 'GetStdHandle' command word )
kernel32.dll @
1 0 winapi GetStdHandle ?exit       \ [ nStdHandle -- HANDLE ]
\ nStdHandle can be one of belows :
\ 	STD_INPUT_HANDLE  (DWORD)-10
\ 	STD_OUTPUT_HANDLE (DWORD)-11
\ 	STD_ERROR_HANDLE  (DWORD)-12

.( init winapi 'ReadConsoleInput' command word, improved to 'waitkey' [ -- keycode ] comamnd word )
\ Kernel32.lib http://msdn.microsoft.com/en-us/library/ms684961%28v=VS.85%29.aspx
\ Reads data from a console input buffer and removes it from the buffer.
\ BOOL WINAPI ReadConsoleInput(
\   __in   HANDLE hConsoleInput,         \ -10 GetStdHandle
\   __out  PINPUT_RECORD lpBuffer,       \ I found it stores 5 dwords for every make-code or break-code of a key  buffer[1]=1 down, buffer[2]=
\   __in   DWORD nLength,                \ lpBuffer's array elements
\   __out  LPDWORD lpNumberOfEventsRead  \ dword container
\ );
kernel32.dll @
4 0 winapi ReadConsoleInputA ?exit         \ [ (-10 GetStdHandle) buffer length &_waitkey_NumberOfEventsRead -- OK ]
create _waitkey_buffer 256 allot
variable &_waitkey_NumberOfEventsRead
: waitkey    ( -- keycode )              \ Wait for press a key, return dword keycode.
    1000 0 do
	   -10 GetStdHandle                  \ arg1
	   _waitkey_buffer                   \ arg2
	   10                                \ arg3
	   &_waitkey_NumberOfEventsRead      \ arg4
	   ReadConsoleInputA drop 			 \ I don't think ReadConsoleInputA can be failed
	   _waitkey_buffer 1 CELLS + @
	   not if r> r> drop dup >r >r ( break ) then
	loop
	_waitkey_buffer 2 CELLS + @
;

1 constant common.f08
.( common.f08 done )
message_switch &vm->greetingswitch ! \ restore .( messages ) switch

