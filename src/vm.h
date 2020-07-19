#ifndef KSANA4TH_VM
#define KSANA4TH_VM

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

FILE* g_pLog;  // debug log.txt 

struct KsanaVm;
typedef struct KsanaVm KsanaVm;
#define MAXNAMELEN 43  // WF06 consider winapi structures may need very long word names
// lesson 3
typedef void (*KVMXT)(KsanaVm *vm);  // the execution vector


typedef struct {
	char name[MAXNAMELEN+1];  // the name
	KVMXT xt;                             // the execuate vector

	//lesson 8
	int immediate;                       // execute immediately regardless of state
} KsanaWord ;   // Word is the execution unit in ksana virtual machine


/* main structure of Virtual Machine */
struct KsanaVm {
	int debugmode;         //  -hcchen5600 2008/11/04 19:38
//Lesson 1
	char *tib;             //terminal input buffer, hold the text to be interpreted.
	int ntib;                //consumed index
	char token[1024];  //current token
	int terminate;       // true if VM is terminating 

//Lesson 2
#define STACK_DEPTH 256
	int sp ;                                  // the stack pointer
	int datastack [STACK_DEPTH]; // the stack

//Lesson 5
	int rp ;                                       // the stack pointer
	int returnstack [STACK_DEPTH];    // the stack

// wf06 hcchen5600 2010/5/31 20:30 , trying to make KVMEval() 'eval' workable in colon words. That requires KVMEval() to
// save-restore vm->ip , vm->abortexec, and this new thing vm->eval-rp-base floating threashold
  int eval_rp_base;

//Lesson 3
#define KVM_MAXWORD 1024         // was 512 , since wf06 become 1024. 1024 is very big for Forth. h.c. chen 2010/05/29
	int nword;                                   //number of words
	KsanaWord vectors[KVM_MAXWORD];    // array to hold name and execution vector

//Lesson 6
	int abortexec;                             // abort execution
	int ip;                                        // the instruction pointer, point to the next xt to be executed

//Lesson 8
	int compiling;                             // true if in compile state
	KsanaWord newword;                 // the high level word under construction

//Lesson 4
#define DICTSIZE 64*1024             // was 16k, since wf06 64k is very big for Forth. h.c. chen 2010/05/29
#define CELLSIZE (sizeof(int))
	int here;                                     // write from here
	char dictionary[DICTSIZE];           // holds the compiled words and data
	int greetingswitch;              // switch to control start up greeting messages and ".( string )". 'cold' command always turn it on. autoexec.f not found turns it off.
	int stopinclude;              // stop including the recent .f file
} ;


//macro for getting the Top-Of-Stack
#define KVMTOS(vm)  ((vm)->datastack[(vm)->sp])
#define KVMRTOS(vm)  ((vm)->returnstack[(vm)->rp])

//lesson 1
KsanaVm *KVMCreate();
KVMFree(KsanaVm *vm);
KVMSetTib(KsanaVm *vm, char *tib);
int KVMNextToken(KsanaVm *vm);


//lesson 2
void KVMDumpStack(KsanaVm *vm);
void KVMAdd(KsanaVm *vm);
void KVMMultiply(KsanaVm *vm);
void KVMPush(KsanaVm *vm, int v);
int KVMPop(KsanaVm *vm);

// lesson 3
// KVMXT KVMFindWord(KsanaVm *vm,char *name);
KsanaWord *KVMGetWord(KsanaVm *vm,char *name);

void KVMAddWord(KsanaVm *vm, char *name, KVMXT xt);

void KVMMinus(KsanaVm *vm);
void KVMDivide(KsanaVm *vm);
void KVMDup(KsanaVm *vm);
void KVMDrop(KsanaVm *vm);
void KVMSwap(KsanaVm *vm);
void KVMOver(KsanaVm *vm);
void KVMBye(KsanaVm *vm);

// lesson 4
int inDictionary(KsanaVm *vm, int address); //see if an address within the scope of dictionary
void DictCompile(KsanaVm *vm, int i);  // compile an integer to the dictionary

// Lesson 5
void KVMRPush(KsanaVm *vm, int v);
int KVMRPop(KsanaVm *vm);
void KVMCall(KsanaVm *vm, int startaddr); // invoke a high level call
void KVMRet(KsanaVm *vm);                    //return to caller

int do_square(KsanaVm *vm);

// Lesson 6 

// Lesson 7
void KVMExecute(KsanaVm *vm, void *xt);

//Lesson 9
void Eval(KsanaVm *vm, char *s);
// void addbasicword(KsanaVm *vm);  replaced by my add_c_code_words()  h.c. chen 2008/10/26 14:44:06

// h.c. chen's retail version 2008/10/26 14:43:17
void add_c_code_words(KsanaVm *vm, int argc, char **argv, int version);
void KVMdotGreeting(KsanaVm *vm);
#endif