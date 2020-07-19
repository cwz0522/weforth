
.( load user32.dll dynamic library )
loadlibrary user32.dll ?exit            \ 'loadlibrary' is from Sir yap's demo in KsanaVM forth tutorial.
                                        \ user32.dll at this moment is a string of filename.
                                        \ after loadlibrary will successfully do its job, 'user32.dll' become a new command word.
                                        \ As a command word, 'user32.dll' leaves the pointer to DLL user32.dll.

.( get 'MessageBoxA' function from user32.dll )
user32.dll @                            \ Get 'user32.dll' address
4 0 winapi MessageBoxA ?exit            \ 'winapi' [ DLL-address n -- error ] gets an export function from the given DLL.
                                        \ in this example, 'winapi' gets the exported function 'MessageBoxA' from user32.dll
                                        \ and assign the save name 'MessageBoxA' to the function.
                                        \ Where n is 'MessageBoxA' function's input argument number. In this exampke, it needs
                                        \ 4 arguments as shown below C++ function prototype,
                                        \
                                        \    int WINAPI MessageBoxA(
                                        \      __in_opt  HWND hWnd,             
                                        \      __in_opt  LPCTSTR lpText,        
                                        \      __in_opt  LPCTSTR lpCaption,     
                                        \      __in      UINT uType             
                                        \    );
                                        \
                                        \ After 'winapi' done. 'MessageBoxA'

.( Test using it . . . )
: test 0 s" hello, world!" s" WinApi Test" 0 MessageBoxA ;    \ see, there are 4 arguments before calling MessageBoxA
test .ss                                                      \ MessageBoxA returns an error code let's check it out in the stack.
