/*
Ksana Minimal Virtual Machine
yap 2007.12.17
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vm.h"
#include "compiler.h"
#include "progflow.h"
#include "io.h"
#include "var.h"
#include "KVMstring.h"
#include "winapi.h"
#include "basic.h"

// *debug*     -hcchen5600 2008/11/04 19:41  vm->debugmode = 0 ends debug mode. This doesn't need a code word
KVMbp(KsanaVm *vm)
{ char cmd [1024];         // buffer to hold input command

  // int save_abortexec, save_ip;
  // save_abortexec = vm->abortexec;
  // save_ip = vm->ip;
  vm->debugmode = 1;
  Eval(vm,"cr .s");   // hcchen5600 2010/06/02 16:48:44 auto .s
  while (vm->debugmode) {
    printf("debug>");      // the command prompt
    gets(cmd);          // get the command from console
    // KVMSetTib(vm,cmd);  // set it as Vm's terminal input buffer

    Eval(vm,cmd);   // hcchen5600 2010/5/31 23:22 now Eval() does save-restore of vm-ip, vm->abortexec, and vm->eval_rp_base adjustment.
  }
  // vm->abortexec = save_abortexec;
  // vm->ip = save_ip;
}

// *bp* [ n --]  n is the break point ID    -hcchen5600 2008/11/04 19:41   vm->debugmode = 0 ends debug mode. This doesn't need a code word
KVMnbp(KsanaVm *vm)
{       char cmd [1024];         // buffer to hold input command
        int  n;
    // int save_abortexec, save_ip;
    // save_abortexec = vm->abortexec;
    // save_ip = vm->ip;

    n = KVMPop(vm);  // break point ID
    vm->debugmode = 1;
    Eval(vm,"cr .s");   // hcchen5600 2010/06/02 16:48:44 auto .s
    while (vm->debugmode) {
      printf("[%d] debug>", n);      // the command prompt
      gets(cmd);          // get the command from console
      // KVMSetTib(vm,cmd);  // set it as Vm's terminal input buffer

      Eval(vm,cmd);
    }
    // vm->abortexec = save_abortexec;
    // vm->ip = save_ip;
}

// *int3*   -hcchen5600 2008/11/07 08:56:26 AM  from my Google Document https://docs.google.com/Doc?id=dgzzwq68_538ddkhc9gj&hl=zh_TW or a copy at http://sites.google.com/site/guitardingdong/ksanavm-forth/wostudycreate-does
// When debugger is avaiable this command triggers the breakpoint
KVMInt3(KsanaVm *vm)
{
  // _asm int 3
}
                

/* --------------------------Lesson 1 ------------------------*/
// Cold start the KVM
void KVMcold(KsanaVm *vm)
{ 
	int argc, version; char **argv;
  Eval(vm,"argc argv version");
  version = KVMPop(vm); 
  argv = KVMPop(vm); 
  argc = KVMPop(vm);
  
  memset(vm,0,sizeof(KsanaVm));
  vm->sp = vm->rp = vm->eval_rp_base = vm->greetingswitch = -1;
  add_c_code_words(vm, argc, argv, version);
  Eval(vm,".greeting");
}                                                                           

//constructor
KsanaVm *KVMCreate()
{
  KsanaVm *vm;
  vm=(KsanaVm*)(malloc(sizeof(KsanaVm)));
  memset(vm,0,sizeof(KsanaVm));
  vm->sp=-1;
  vm->rp= vm->eval_rp_base -1;
  return vm;
}                 

//destructor
KVMFree(KsanaVm *vm)
{
  free(vm);
}

KVMSetTib(KsanaVm *vm, char *tib)
{
  vm->tib=tib;
  vm->ntib=0;
}

/* get a token from tib, delimiter by space character (ascii <=32) */
int KVMNextToken(KsanaVm *vm)
{
  unsigned char *ptib,*ptoken;
  if (!vm->tib) return 0; // nothing to process
  ptib=vm->tib+vm->ntib;
  while (*ptib && *ptib <=' ') {
    ptib++;  // skip the leading delimiter
  }
  ptoken=vm->token;
  while (*ptib && *ptib>' ') {
    *ptoken++ = *ptib++;
    if ((ptoken-vm->token)>=sizeof(vm->token)-1) break;  // check if exceed maximum token
  }
  *ptoken=0;  // ending null byte
  vm->ntib = ptib - vm->tib;   //update the consumed index
  return *(vm->token);  // return true if token length>0 ( not null string)
}


/* --------------------------Lesson 2 ------------------------*/

void KVMPush(KsanaVm *vm, int v)
{
  if ( (vm->sp)  >= STACK_DEPTH) {
    puts("Stack Overflow");
    return;
  }
  vm->sp++;
  vm->datastack[vm->sp]=v;
}
int KVMPop(KsanaVm *vm)
{
  int v;
  if ( vm->sp  < 0 ) {
    puts("Stack under flow");
    return 0;
  }

  v= vm->datastack[vm->sp];
  vm->sp--;
  return v;
}


/* --------------------------Lesson 3 ------------------------*/

/* add a new word  */
void KVMAddWord(KsanaVm *vm, char *name, KVMXT xt)
{
  if( vm->nword >= KVM_MAXWORD) {  
    printf("Error! vm->nword %d over its limit %d", vm->nword, KVM_MAXWORD);  // h.c. chen 2008/11/07 20:57:51
    return;
  }

  strncpy( vm->vectors[vm->nword].name, name, MAXNAMELEN);
  vm->vectors[vm->nword].xt = xt;

  vm->nword++;
}

// is this 'tick' ? find a word's entry point
KVMXT FindWord(KsanaVm *vm,char *name)
{
  KsanaWord *w;
  w=KVMGetWord(vm,name);
  if (w) return w->xt;
  return NULL;
}

// h.c. chen 2010/05/30   [ stringz -- KsanaWord->xt ] 
void KVMsFindWord(KsanaVm *vm)
{
  KVMPush(vm, (int) FindWord(vm,KVMPop(vm)));
}

// h.c. chen 2010/05/30   [ -- KsanaWord->xt ] find next-token's xt, return 0 if not found
void KVMFindWord(KsanaVm *vm)
{
  KVMNextToken(vm);
  KVMPush(vm, (int) FindWord(vm,vm->token));
}

/* given a name , return the word , return NULL if not found*/
KsanaWord *KVMGetWord(KsanaVm *vm,char *name)
{
  int i;
  KsanaWord *word;
  for (i=vm->nword;i>=0;i--) {  // scan from bottom, newly added word.
    word = &(vm->vectors[i]);
    if (strcmp(word->name, name)==0) return word;
  }
  return NULL;
}


/* --------------------------Lesson 4 ------------------------*/
int inDictionary(KsanaVm *vm, int address)
{
  return (address >= vm->dictionary) && (address <= vm->dictionary+DICTSIZE);
}


// invoke a high level word,  the forth "innerloop"
void KVMCall(KsanaVm *vm, int startaddr)
{
  int addr;
  vm->abortexec=0;
  vm->ip=startaddr;
  do {
    // vm->ip and here always point to somewhere in the vm->dictionary space in data segment.
    // While actual code address, addr=*((int*)vm->ip), is in code segment. Be clear with this.
    addr=*((int*)vm->ip);        // fetch the address from ip
    vm->ip+=CELLSIZE;          //advance to next CELL
    if (inDictionary(vm,addr)) {
      KVMRPush(vm,vm->ip);  // save the next xt for returning from caller , see  *1*
      KVMCall(vm,addr);         // jmp to the addr , recursive call
    } else {
      (*(KVMXT)(addr))(vm);   // call as a C function  ( code-word )
    }
  } while (!vm->abortexec) ;      // abort if not more item on return stack  *2*
}

void KVMRet(KsanaVm *vm)            // return to caller
{
  if (vm->rp <= vm->eval_rp_base) {   // to allow Eval() used in colon word, I use vm->eval_rp_base.  h.c.chen 2010/5/31 wf06
    vm->abortexec=1;  // quit innerloop if no more item on return stack  *2*
    return;
  }
  vm->ip=KVMRPop(vm);                // recover the ip from return stack   *1*
}

void KVMQret(KsanaVm *vm)            // conditional return to caller
{
  if (KVMPop(vm)) KVMRet(vm);
}

/* ------------- lesson 9 ---------------*/
void Eval(KsanaVm *vm, char *s)
{
  KsanaWord *w;
  int v, ishex, isdecimal;
  char *cmd=vm->tib; //save current tib Eval 可以是程式直接送進來的，本來未完成的 console input stream global variables 要保留好，等會兒要回來繼續。
  int ntib=vm->ntib; // save oritinal vm->ntib value, 理由同上。
  
  // wf06 h.c. chen 2010/5/31   I found all my new words are unstable (KVMEval(), KVMbp(), KVMnbp(), s_include() ... ) because they uses Eval() !!
  // Eval() needs to also save-restore vm->ip, vm->abortexec, and vm->eval_rp_base.
  int vm_ip = vm->ip;
  int vm_abortexec = vm->abortexec;
  int vm_eval_rp_base = vm->eval_rp_base;
  vm->eval_rp_base = vm->rp;
  
  KVMSetTib(vm,s);  // 把接下來的 input command stream 導向 s the input argument.

  //the main loop need some changes...
  while ( KVMNextToken(vm) ) {
	w = KVMGetWord(vm,vm->token);
    if (w) {
      if (vm->compiling && !w->immediate) { // immediate word must be run immediately regardless of state
        DictCompile(vm, w->xt);         //!!  compile the xt to dictionary , instead of running it
      } else {
        KVMExecute(vm, w->xt);       // not in compiling state (interpreting state)   , simply execute
      }
    } else {                                // the rest are same as lesson 3
      ishex = isdecimal = v = 0;
      if (vm->token[0]=='$'){  // follow F language's convention, $ffffffff means hex
        ishex = sscanf(&vm->token[1],"%x",&v);  // if it is a hex
      }else if (vm->token[0]=='0' && (vm->token[1]=='x' || vm->token[1]=='X')){
        ishex = sscanf(&vm->token[2],"%x",&v);  // if it is a hex
      } else {
        isdecimal = sscanf(vm->token,"%d",&v); // see if it is a numeric value
      }
      if (isdecimal != 1 && ishex != 1) {
        printf("unknown command => %s\n",vm->token);
        vm->compiling = 0;  //  -hcchen5600 2008/11/03 20:09 http://mfgkm-wks.wistron.com/RD/Software/Basic%20extension%20words.aspx 16:53
      } else {
        // got a valid numeric value, push the value into the stack
        if (vm->compiling) {                        // compiling mode
          DictCompile(vm, KVMdoLiteral);  //!! compile a literal
          DictCompile(vm, v);                    // at runtime, the v will be push to stack
        } else {
          KVMPush(vm,v);                        // interpreter mode, simply push on stack
        }
      }
    }
  }

  KVMSetTib(vm,cmd); // restore
  vm->ntib=ntib;        // restore ntib

  // h.c. chen 2010/5/31  for  KVMEval KVMbp KVMnbp and KVMsInclude 
  vm->ip           = vm_ip;           
  vm->abortexec    = vm_abortexec;    
  vm->eval_rp_base = vm_eval_rp_base; 
}

// lesson 13
void KVMAlias(KsanaVm *vm)
{
        KsanaWord *word;
        KVMNextToken(vm);
        word=KVMGetWord(vm,vm->token);

        //
        if (!word) {
                printf("unknown command => %s\n",vm->token);
                return;
        }

        KVMNextToken(vm);
        KVMAddWord(vm,vm->token, word->xt);
        if (word->immediate) KVMSetImmediate(vm);
}

//lesson 15   h.c. chen 2008/11/08 08:50:25 move from winapi.c to vm.c
int s_include(KsanaVm *vm, char *filename)   // return Error, this error is dedicated for file opening and reading issues. Not for the .f file's programming problems.
{
        FILE *f;
        char buffer[1024];
        f=fopen(filename,"r");
        if (!f) {
                // printf("%s not found\n",vm->token);  改成有 return 值，無需再自己亂印 error message 了 hcchen5600 2010/06/03 11:08:50 
                return(-1);  // return -1 means FAILED
        }
        vm->stopinclude = 0;
        while (!feof(f)) {
                if (fgets(buffer,1024,f)){ 
                	Eval(vm,buffer);
					if (vm->stopinclude) {
						if (vm->greetingswitch) puts("vm->stopinclude turned on, stop including.");
						break;   // I need a method to stop including the recent .f file. hcchen5600 2010/06/08 19:30:48 
						// BTW, to stop including the recent line, "0 vm->tib !" works fine.
					}
                }                                 
        }
        fclose(f);
        return(0);  // 0 == OK
}

void KVMInclude(KsanaVm *vm)  // [ -- error ] this error is dedicated for file opening and reading issues. Not for the .f file's programming problems.
{
        KVMNextToken(vm);
        KVMPush(vm, s_include(vm, vm->token));
}

void KVMsInclude(KsanaVm *vm)  // [ -- error ] this error is dedicated for file opening and reading issues. Not for the .f file's programming problems.
{       
    //  int save_abortexec, save_ip;
  	//	save_abortexec = vm->abortexec;
  	//	save_ip = vm->ip;
        KVMPush(vm, s_include(vm, KVMPop(vm)));
  	//	vm->abortexec = save_abortexec;
  	//	vm->ip = save_ip;
}

// h.c. chen 2010/05/29 allow WF05 to execute a string
// [ stringz -- ... ] Eval(stringz)
void KVMEval(KsanaVm *vm)
{   
  // int save_abortexec, save_ip;
  // save_abortexec = vm->abortexec;
  // save_ip = vm->ip;
  Eval(vm, KVMPop(vm));
  // vm->abortexec = save_abortexec;
  // vm->ip = save_ip;
}

// [ errorlevel -- ] exit the program, returns TOS as errorlevel
void KVMExit(KsanaVm *vm)                 
{   
	exit(vm->sp >= 0 ? KVMPop(vm) : 0);
}                                                              

void add_c_code_words(KsanaVm *vm, int argc, char **argv, int version)
{
    add_basic_words(vm);
    fprintf(g_pLog, "add_basic_words(vm)\n"); fflush(g_pLog);
    addioword(vm);
    addstringword(vm);
    addflowword(vm);
    addvarword(vm);
    addwinapiword(vm);
    adddictword(vm);
    addcompilerword(vm);
    fprintf(g_pLog, "addcompilerword(vm)\n"); fflush(g_pLog);
    KVMAddWord(vm, "cold", KVMcold);
    KVMAddWord(vm, "include", KVMInclude);
    KVMAddWord(vm, "sinclude", KVMsInclude);
    KVMAddWord(vm, "alias", KVMAlias);
    KVMAddWord(vm, "return", KVMRet);
    KVMAddWord(vm, "?return", KVMQret);
    KVMAddWord(vm, "*debug*", KVMbp);
    KVMAddWord(vm, "*bp*", KVMnbp);   
//  KVMAddWord(vm, "debugoff", KVMbpoff);  // vm->debugmode = 0; doesn't need a code word
    KVMAddWord(vm, "*int3*", KVMInt3);
    KVMAddWord(vm, "eval", KVMEval);
    KVMAddWord(vm, "findword", KVMFindWord);   // [ -- KsanaWord->xt ] get KsanaWord->xt entry point of next word. For interactive use.
    KVMAddWord(vm, "sfindword", KVMsFindWord); // [ stringz -- KsanaWord->xt ] get KsanaWord->xt entry point . For colon definition use
    KVMAddWord(vm, "exit", KVMExit);
    KVMAddWord(vm, ".greeting", KVMdotGreeting);
    fprintf(g_pLog, "KVMAddWord(vm, .greeting, KVMdotGreeting)\n"); fflush(g_pLog);

//  Eval(vm,": ? @ . ; ");  // <========= demo a in line colon word definition
//  Eval(vm,"alias mod %"); // <========= demo alias

	Eval(vm,": constant create , does> r> @ ;");
    fprintf(g_pLog, ": constant create , does> r> @ ;\n"); fflush(g_pLog);
	KVMPush(vm, version); Eval(vm,"constant version");
    fprintf(g_pLog, "constant version\n"); fflush(g_pLog);
	KVMPush(vm, argc); Eval(vm,"constant argc");
    fprintf(g_pLog, "constant argc\n"); fflush(g_pLog);
	KVMPush(vm, argv); Eval(vm,"constant argv");
    fprintf(g_pLog, "constant argv\n"); fflush(g_pLog);
	KVMPush(vm, stdin); Eval(vm,"constant stdin");
    fprintf(g_pLog, "constant stdin\n"); fflush(g_pLog);
    KVMPush(vm, stdout); Eval(vm,"constant stdout");
    fprintf(g_pLog, "constant stdout\n"); fflush(g_pLog);
    KVMPush(vm, stderr); Eval(vm,"constant stderr");
    fprintf(g_pLog, "constant stderr\n"); fflush(g_pLog);
    KVMPush(vm, EOF); Eval(vm,"constant EOF");
    fprintf(g_pLog, "constant EOF\n"); fflush(g_pLog);
    KVMPush(vm, (int) &errno); Eval(vm,"constant &errno");
    fprintf(g_pLog, "constant &errno\n"); fflush(g_pLog);
    
    // KsanaVM system constants
    KVMPush(vm, (int) &vm->debugmode      ); Eval(vm, "constant &vm->debugmode     ");
    KVMPush(vm, (int) &vm->tib            ); Eval(vm, "constant &vm->tib           ");
    KVMPush(vm, (int) &vm->ntib           ); Eval(vm, "constant &vm->ntib          ");
    KVMPush(vm, (int) &vm->token          ); Eval(vm, "constant &vm->token         ");
    KVMPush(vm, (int) &vm->terminate      ); Eval(vm, "constant &vm->terminate     ");
    KVMPush(vm, (int) &vm->sp             ); Eval(vm, "constant &vm->sp            ");
    KVMPush(vm, (int) &vm->datastack      ); Eval(vm, "constant &vm->stack         ");
    KVMPush(vm, (int) &vm->rp             ); Eval(vm, "constant &vm->rp            ");
    KVMPush(vm, (int) &vm->returnstack    ); Eval(vm, "constant &vm->rstack        ");
    KVMPush(vm, (int) &vm->nword          ); Eval(vm, "constant &vm->nword         ");
    KVMPush(vm, (int) &vm->vectors        ); Eval(vm, "constant &vm->vectors       ");
    KVMPush(vm, (int) &vm->abortexec      ); Eval(vm, "constant &vm->abortexec     ");
    KVMPush(vm, (int) &vm->ip             ); Eval(vm, "constant &vm->ip            ");
    KVMPush(vm, (int) &vm->compiling      ); Eval(vm, "constant &vm->compiling     ");
    KVMPush(vm, (int) &vm->newword        ); Eval(vm, "constant &vm->newword       ");
    KVMPush(vm, (int) &vm->here           ); Eval(vm, "constant &vm->here          ");
    KVMPush(vm, (int) &vm->dictionary     ); Eval(vm, "constant &vm->dictionary    ");
    KVMPush(vm, (int) &vm->greetingswitch ); Eval(vm, "constant &vm->greetingswitch");
    KVMPush(vm, (int) &vm->stopinclude    ); Eval(vm, "constant &vm->stopinclude   ");
}
