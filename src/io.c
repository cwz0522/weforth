#include <stdio.h>
#include <windows.h>
#include "vm.h"

void KVMDot(KsanaVm *vm) // [int --]
{
	printf("%d ",KVMPop(vm));
}

void KVMCr(KsanaVm *vm)  // [ -- ]
{
	printf("\n");
}

// WF06 already supports call-c-function() so we don't need seperated KVMPrintf() and KVMScanf().
// H.C. Chen  -hcchen5600 2008/10/30 14:54
// [ string, v1 , v2, ... n -- int ]
// KVMprintf needs a given n at TOP, number of given values, can be 0, 1,2,... etc
// void KVMPrintf(KsanaVm *vm)
// {   int i, n, p, r;
//   n = KVMPop(vm);   // get argument count n
//   for (i=0; i<=n; i++){
//     p=KVMPop(vm);
//     _asm push p
//   }
//   n = CELLSIZE + n*CELLSIZE;  // stack bytes used by input arguments, need to drop later
//   _asm call dword ptr [printf]
//   _asm mov r,eax    // save function return value into variable r
//   _asm add esp,n    // drop input arguments from stack
//   KVMPush(vm,r);    // push variable r to forth data stack
// }

// H.C. Chen   -hcchen5600 2008/11/04 14:56
// [ string, v1 , v2, ... n -- int ]
// KVMScanf needs a given n at TOS, number of given values, can be 0, 1,2,... etc. But what does n=0 mean?
// Returns:    The number of fields that were successfully converted and
//             assigned. A return value of EOF means an attempt was made
//             to read at end¢wof¢wfile. A return value of 0 means no
//             field were assigned.
// void KVMScanf(KsanaVm *vm)
// {   int i, n, p, r;
//   n = KVMPop(vm);   // get argument count n
//   for (i=0; i<=n; i++){
//     p=KVMPop(vm);
//     _asm push p
//   }
//   n = CELLSIZE + n*CELLSIZE;  // stack bytes used by input arguments, need to drop later
//   _asm call dword ptr [scanf]
//   _asm mov r,eax    // save function return value into variable r
//   _asm add esp,n    // drop input arguments from stack
//   KVMPush(vm,r);    // push variable r to forth data stack
//   fflush(stdin);    // scanf is very ticky actually, this flush avoid all problems.
// }

// H.C. Chen   -hcchen5600 2010/05/29 -> 6/5 modified. input n-push and one more argument m-pop that indicates how many stack should be pop'ed
// after call-return. Of course usually m-pop must be same as n-push value , but I found some C functions are not working that way !! For example
// GetStdHandle() function has one imput but when it return that stack already balanced so we don't need to pop that stack. I don't know why, that
// is what I have found by VS 2005 debugger !!
// funcall [ ... n-push m-pop entry -- long ]
// Returns: eax , whatever it is.
// void KVMFuncall(KsanaVm *vm)
// {   
// 	int i, n, m, p, r, entry;  // int is actually 32 bits
// 	entry = KVMPop(vm);   // get function entry point
// 	m = KVMPop(vm);   // get argument count m-pop  , after calling how many stack should be pop'ed for balance?
// 	n = KVMPop(vm);   // get argument count n-push , before calling how many arguments were pushed into the stack?
// 	for (i=0; i<n; i++){
// 		p=KVMPop(vm);
// 		_asm push p
// 	}
// 	m = m * CELLSIZE;  // stack bytes used by input arguments, need to drop later
// 	_asm call dword ptr [entry]
// 	_asm mov r,eax    // save function return value into variable r
// 	_asm add esp,m    // drop input arguments from stack
// 	KVMPush(vm,r);    // push variable r to forth data stack
// }
                              
// [ X Y -- ] set console cursor position
void KVMgotoxy(KsanaVm *vm)
{
	COORD coord;
	coord.Y = KVMPop(vm);
	coord.X = KVMPop(vm);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}            

// [ -- X Y ]  -1 means failed to read cursor X,Y coords.
void KVMwherexy(KsanaVm *vm)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle( STD_OUTPUT_HANDLE ), &csbi)){
		KVMPush(vm,-1);   // when in trouble, return very very trouble values
		KVMPush(vm,-1);
	} else {
		KVMPush(vm,csbi.dwCursorPosition.X);
		KVMPush(vm,csbi.dwCursorPosition.Y);
	}
}


// [ attribute -- ] set console text attribute 
void KVMConsoleAttribute(KsanaVm *vm)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), KVMPop(vm));
}


// http://www.cplusplus.com/forum/articles/10515/
// [ -- ] Clear screen 
void KVMClearScreen(KsanaVm *vm)
{
	HANDLE                     hStdOut;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD                      count;
	DWORD                      cellCount;
	COORD                      homeCoords = { 0, 0 };

	hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
	if (hStdOut == INVALID_HANDLE_VALUE) return;  // do nothing leave

	/* Get the number of cells in the current buffer */
	if (!GetConsoleScreenBufferInfo( hStdOut, &csbi )) return;
	cellCount = csbi.dwSize.X *csbi.dwSize.Y;

	/* Fill the entire buffer with spaces */
	if (!FillConsoleOutputCharacter(
		hStdOut,
		(TCHAR) ' ',
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Fill the entire buffer with the current colors and attributes */
	if (!FillConsoleOutputAttribute(
		hStdOut,
		csbi.wAttributes,
		cellCount,
		homeCoords,
		&count
		)) return;

	/* Move the cursor home */
	SetConsoleCursorPosition( hStdOut, homeCoords );
}

/* label list from map file  -hcchen5600 2008/12/19 16:36  */
unsigned label_list[] = {
   (unsigned) printf     , // 00 
   (unsigned) fprintf    , // 01 
   (unsigned) sprintf    , // 02 
   (unsigned) vsprintf   , // 03 
   (unsigned) puts       , // 04 
   (unsigned) fputs      , // 05 
   (unsigned) fputc      , // 06 
   (unsigned) fputchar   , // 07 
   (unsigned) scanf      , // 08 
   (unsigned) fscanf     , // 09 
   (unsigned) sscanf     , // 10 
   (unsigned) fflush     , // 11 
   (unsigned) gets       , // 12 
   (unsigned) fgets      , // 13 
   (unsigned) fgetc      , // 14 
   (unsigned) fgetchar   , // 15 
   (unsigned) fopen      , // 16 
   (unsigned) fclose     , // 17 
   (unsigned) feof       , // 18
   (unsigned) ferror     , // 19
   (unsigned) strncpy    , // 20 
   (unsigned) strupr     , // 21 
   (unsigned) strlwr     , // 22 
   (unsigned) strcat     , // 23 
   (unsigned) strlen     , // 24 
   (unsigned) strcpy     , // 25 
   (unsigned) strcmp     , // 26 
   (unsigned) stricmp    , // 27 
   (unsigned) strerror   , // 28 The strerror function maps errnum to an error-message string, returning a pointer to the string */
   (unsigned) memcpy     , // 29 
   (unsigned) malloc     , // 30 
   (unsigned) itoa       , // 31 
   (unsigned) ltoa       , // 32 
   (unsigned) atoi       , // 33 
   (unsigned) atol       , // 34 
   (unsigned) ultoa      , // 35 
   (unsigned) atexit     , // 36 
   (unsigned) freopen    , // 37 
   (unsigned) fdopen     , // 38 
   (unsigned) fseek      , // 39 
   (unsigned) ftell      , // 40 Gets the current position of a file pointer. */
   (unsigned) ungetc     , // 41 Pushes a character back onto the stream.   */
};

// function list map , work with funcall to call C functions [ index -- entry ]
void KVMFuncmap(KsanaVm *vm)
{
	KVMPush(vm,(unsigned) label_list[KVMPop(vm)]);
}

void addioword(KsanaVm *vm)
{
	KVMAddWord(vm,".",KVMDot);
	KVMAddWord(vm,"cr",KVMCr);
	// KVMAddWord(vm,"printf",KVMPrintf);   WF06 has call-c-function aleady.
	// KVMAddWord(vm,"scanf" ,KVMScanf);    WF06 has call-c-function aleady.
	// KVMAddWord(vm,"call-c-function" ,KVMFuncall);
	KVMAddWord(vm,"c-function-entry" ,KVMFuncmap);
	KVMAddWord(vm,"gotoxy" , KVMgotoxy);       // [x y -- ]
	KVMAddWord(vm,"wherexy" , KVMwherexy);     // [ -- x y ]
	KVMAddWord(vm,"cls" , KVMClearScreen);     // [ -- ] clear screen
	KVMAddWord(vm,"SetScreenAttribute" , KVMConsoleAttribute);     // [ attrib -- ] set attribute                   
}