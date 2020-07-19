#include "vm.h"
#include "progflow.h"
	
// ( -- str )  �ڧ� length �����F�A�]�� stdio ���� C Language �C�� string ���n�O�o drop �@�U�ӷФF�C
//             �ߤ@�ݭn�o�� string length �� 'type' �]�S���ΡC �u���n string length �ۤv�h 4(CELLSIZE) �h���N���F�C hcchen5600 2010/06/03 09:18:08 
void KVMdoStrQ(KsanaVm *vm)
{
   int len=*(int*)vm->ip;
   int align;
   KVMPush(vm,vm->ip+CELLSIZE);
   // KVMPush(vm,len);             // ����, �Q�n string length �ۤv�h 4 �hŪ�N���F�C hcchen5600 2010/06/03 09:14:39 
   align= (vm->here+len ) % 4;
   if (align) len+=4-align;

   vm->ip+=CELLSIZE+len;  //skip the str len and str body
}

//string compiler
void  KVMStrQ(KsanaVm *vm)
{
	char *ending;
	char *starting= vm->tib+vm->ntib+1 ;
	int n,align;

	if (!vm->compiling ) {
		printf("%s => S\"\n", must_be_in_compile_state);
		return;
	}

	ending=strchr(starting,'"');
	if (!ending) ending=starting+strlen(starting);
	
	n=ending-starting;
	vm->ntib += n+2;

	DictCompile(vm, KVMdoStrQ); // runtime code
    DictCompile(vm, n+1 ) ;         // length of the string

	strncpy(vm->dictionary+vm->here  , starting, n );
	*(char*)(vm->dictionary+vm->here +n)='\0';//ending null byte
	n++; 

	align=((vm->here+n) % 4);
	if (align)	n += 4-align;  //alignment to 4

	vm->here += n;
}

void KVMType(KsanaVm *vm)
{
	int sz=KVMPop(vm);
	char *s=KVMPop(vm);
	printf("%s",s);
}

// ( -- ) 
void KVMdoDotQ(KsanaVm *vm)
{
   int len=*(int*)vm->ip;
   int align;
   KVMPush(vm,vm->ip+CELLSIZE);
   KVMPush(vm,len);
   align= (vm->here+len ) % 4;
   if (align) len+=4-align;
   vm->ip+=CELLSIZE+len;  //skip the str len and str body
   KVMType(vm);  // print the string
}

//string compiler ."
void  KVMDotQ(KsanaVm *vm)
{
	char *ending;
	char *starting= vm->tib+vm->ntib+1 ;
	int n,align;

	if (!vm->compiling ) {
		printf("%s => .\"\n", must_be_in_compile_state);
		return;
	}

	ending=strchr(starting,'"');
	if (!ending) ending=starting+strlen(starting);
	
	n=ending-starting;
	vm->ntib += n+2;

	DictCompile(vm, KVMdoDotQ); // runtime code
    DictCompile(vm, n+1 ) ;         // length of the string

	strncpy(vm->dictionary+vm->here  , starting, n );
	*(char*)(vm->dictionary+vm->here +n)='\0';//ending null byte
	n++; 

	align=((vm->here+n) % 4);
	if (align)	n += 4-align;  //alignment to 4

	vm->here += n;
}

void KVMBracketComment(KsanaVm *vm)
{
	char *starting= vm->tib+vm->ntib+1 ;
	char *ending;

	ending=strchr(starting,')');
	if (!ending) ending=starting+strlen(starting);

	vm->ntib += ending-starting+2;
}

void KVMComment(KsanaVm *vm)
{
	char *starting= vm->tib+vm->ntib;  // I believe this +1 is incorrect, should be removed <== YES!!  -hcchen5600 2008/11/03 16:41 
	vm->ntib += strlen(starting);
}

void KVMDotBracket(KsanaVm *vm)
{
	char *ending,*starting= vm->tib+vm->ntib+1 ;
	char temp;
	int n;
	ending=strchr(starting,')');
	if (!ending) ending=starting+strlen(starting);
	
	n=ending-starting;
	vm->ntib += n+2;
	temp=*(starting+n);
	*(starting+n)=0;
    if (vm->greetingswitch) printf("%s\n",starting); // new feature. WF starting greeting and .( string ) �s�ʡC hcchen5600 2010/06/03 11:35:03 
	*(starting+n)=temp;      
}


void addstringword(KsanaVm *vm)
{
	KVMAddWord(vm, "s\"", KVMStrQ); KVMSetImmediate(vm);
	KVMAddWord(vm, ".\"", KVMDotQ); KVMSetImmediate(vm);
	KVMAddWord(vm,"type",KVMType); 

	//lesson 15
	KVMAddWord(vm, "(", KVMBracketComment); KVMSetImmediate(vm);
	KVMAddWord(vm, "\\", KVMComment); KVMSetImmediate(vm);
	KVMAddWord(vm,".(",KVMDotBracket); KVMSetImmediate(vm);
}