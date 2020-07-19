#include <windows.h>
#include "vm.h"
#include "var.h"

// ( -dllname- -- something_wrong? )  create a new variable named dllname   -hcchen5600 2008/10/30 10:43 OK=0 Failed=1
void KVMLoadLib(KsanaVm *vm)
{
	int temptib;
	HMODULE WINAPI dll;
	temptib = vm->ntib;
	KVMNextToken(vm);
	dll=LoadLibraryA(vm->token);   // get the handle 
	if (dll) {
		vm->ntib = temptib;
		KVMcreate(vm);
		DictCompile(vm, (int) dll);                 
		KVMPush(vm,0);  // Return 0 something_wrong? No! good. -hcchen5600 2008/10/30 10:41 
	} else {
		KVMPush(vm,1);  // Return 1 something_wrong? Yes! too bad.  -hcchen5600 2008/10/30 10:41 
	}
}


void KVMdoWinapi(KsanaVm *vm)
{
	// int i,p,r, proc=*(int*)vm->ip;
	// int paracount = *(int*)(vm->ip+CELLSIZE);
	// int popcount  = *(int*)(vm->ip+CELLSIZE+CELLSIZE) * CELLSIZE;   // hcchen5600 2010/06/10 12:41:00 
    // 
	// for (i=0;i<paracount;i++)	{
	// 	p=KVMPop(vm);
	// 	_asm push p;
	// }
	// //invoke the function
	// _asm call proc ; 
	// //save function return value into variable r
	// _asm mov r,eax; 
	// _asm add esp,popcount    // drop input arguments from stack for _cdecl calling convention. For stdcall, it's 0.   hcchen5600 2010/06/10 12:41:07 
	// //push variable r to stack
	// KVMPush(vm,r);
	// //leave the high level defined by "CREATE" in Winapi
	// KVMRet(vm);
}


// ( dll n -name- -- something_wrong? )  -hcchen5600 2008/10/30 10:35 add return flag, success = OK = 0, failed = 1
void KVMWinapi(KsanaVm *vm)
{
	int temptib;

	FARPROC WINAPI proc;
	int popcount  = KVMPop(vm);    // TOP of stack. For cdecl calling convention it's same number as parameter count. For stdcall, it's 0.   hcchen5600 2010/06/10 12:40:49 
	int	paracount = KVMPop(vm);
	HMODULE WINAPI dll = (HMODULE WINAPI) KVMPop(vm);

	temptib = vm->ntib;
	KVMNextToken(vm);
	proc = GetProcAddress(dll,vm->token);             // windows API
	if (proc) {
		vm->ntib = temptib;
		KVMcreate(vm);                                    // create a new word with same name
		ChangeCreateCodeField(vm,KVMdoWinapi);		      // change code field
		DictCompile(vm, (int) proc);                      // entry point of API
		DictCompile(vm, paracount);                       // number of parameter
		DictCompile(vm, popcount);                       // number of stack cells to clean up hcchen5600 2010/06/10 12:40:38 
		KVMPush(vm,0);  // Return 0 something_wrong? No! good. -hcchen5600 2008/10/30 10:41 
	} else {
		KVMPush(vm,1);  // Return 1 something_wrong? Yes! too bad.  -hcchen5600 2008/10/30 10:41 
	}
}


void addwinapiword(KsanaVm *vm)
{
	//lesson 9
	KVMAddWord(vm, "loadlibrary", KVMLoadLib);
	KVMAddWord(vm, "winapi", KVMWinapi);

}