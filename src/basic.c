/*
Wistron EFIF Project 0.04  H.C. Chen 2008/11/01 14:17:52
Build most basic 23 words mentioned in eforth 《易符真經》[第一章 單晶系統的挑戰] http://www.eforth.com.tw/academy-n/sutra/sutra_c.htm
My modification log : http://sites.google.com/site/guitardingdong/ksanavm-forth/get-the-23-basic-words-into-my-ksanavm-forth
*/

#include <string.h>
#include <stdio.h>
#include "vm.h"
#include "KVMstring.h"

// pop two item , do multiplication and push back
// stack effect diagram ( n1 n2 -- n1+n2 )
void KVMAdd(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) + KVMPop(vm)); //  -hcchen5600 2008/11/03 22:45
}

//  -hcchen5600 2008/11/03 21:11
void KVMMod(KsanaVm *vm)
{
        int n1,n2 ,r ;
        n2=KVMPop(vm);    // pop second operand
        n1=KVMPop(vm);    // pop first operand
        r= n1 % n2;              // do MOD
        KVMPush(vm,  r );   // push to stack
}

// pop two item , do multiplication and push back
// ( n1 n2 -- n1*n2 )
void KVMMultiply(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) * KVMPop(vm));  //  -hcchen5600 2008/11/03 22:44
}

// .s
void KVMDumpStack(KsanaVm *vm)
{
        int i;
        char buf[2048];
        char str[16];

        if (vm->sp<0) {
                puts("Empty stack");
                return;
        }
        memset(buf,0,sizeof(buf));                // string.h
        memset(str,0,sizeof(str));

        strncat(buf,"stack: ",sizeof(buf));
        for (i=0;i<=vm->sp;i++)
        {
                //itoa(vm->datastack[i],str,10);
                sprintf(str,"0x%x",vm->datastack[i]);  //aguai suggested   stdio.h
                strncat(buf,str,sizeof(buf));
                strncat(buf," ",sizeof(buf));
        }
        puts(buf);
}

/* --------------------------Lesson 3 ------------------------*/


// ( n -- n n )
void KVMDup(KsanaVm *vm)
{
        if ( (vm->sp)  >= STACK_DEPTH) {
                puts("Stack Overflow");
                return;
        }
        KVMPush(vm, KVMTOS(vm));
}
// ( n -- )
void KVMDrop(KsanaVm *vm)
{
        KVMPop(vm);
}
// ( n1 n2 -- n2 n1 )
void KVMSwap(KsanaVm *vm)
{
        int temp;
        if (vm->sp<1) {
                puts("need at least 2 items on stack");
                return;
        }

        temp=KVMTOS(vm);
        KVMTOS(vm)=((vm)->datastack[(vm)->sp-1]);
        ((vm)->datastack[(vm)->sp-1]) = temp;
}

// ( n1 n2 -- n1 n2 n1 )
void KVMOver(KsanaVm *vm)
{
        int temp;
        if (vm->sp<1) {
                puts("need at least 2 items on stack");              // stdio.h
                return;
        }

        temp=((vm)->datastack[(vm)->sp-1]);
        KVMPush(vm,temp);
}


void KVMMinus(KsanaVm *vm)
{
        int n1,n2 ,r ;
        n2=KVMPop(vm);    // pop second operand
        n1=KVMPop(vm);    // pop first operand
        r= n1-n2;              // do addition
        KVMPush(vm,  r );   // push to stack
}

// pop two item , do division and push back
// ( n1 n2 -- n1/n2 )
void KVMDivide(KsanaVm *vm)
{
        int n1,n2 ,r ;
        n2=KVMPop(vm);    // pop second operand
        n1=KVMPop(vm);    // pop first operand
        r= n1 / n2;       // do multiplication
        KVMPush(vm, r);   // push to stack
}

void KVMBye(KsanaVm *vm)
{
        vm->terminate=1;
}


/* --------------------------Lesson 5 ------------------------*/
//push given argument to return stack  , for KVM internal use
void KVMRPush(KsanaVm *vm, int v)
{
        if ( (vm->rp)  >= STACK_DEPTH) {
                puts("Return Stack Overflow");
                return;
        }
        vm->rp++;
        vm->returnstack[vm->rp]=v;
}
//  >r
void KVMrto(KsanaVm *vm)
{
        KVMRPush(vm, KVMPop(vm));   // push into return stack
}

//pop from return stack  , for KVM internal use
int KVMRPop(KsanaVm *vm)
{
        int v;
        if ( vm->rp  < 0 ) {
                puts("Return Stack under flow");
                return 0;
        }

        v= vm->returnstack[vm->rp];
        vm->rp--;
        return v;
}

// r>
void KVMrfrom(KsanaVm *vm)
{
        KVMPush(vm,KVMRPop(vm));
}


// Read from return stack  r@
void KVMRAt(KsanaVm *vm)
{
        int v;
        if ( vm->rp  < 0 ) {
                puts("Return Stack under flow");
                return;
        }

        v= vm->returnstack[vm->rp];
        KVMPush(vm, v);   // push to stack
}


/* --------------------------Lesson 7 ------------------------*/
void KVMExecute(KsanaVm *vm, void *xt)   // KsanaVM internal function. Is not the Forth word execute or perform [ addr --]
{
        if (inDictionary(vm, xt)) {         //** these two line is added to check if a high level word
                KVMCall(vm, xt);               //** execute high level word, set a break point here and type test
        } else {
                (*(KVMXT)(xt))(vm) ;       // cast to KVMXT and execute the code-word ( c function )
        }
}

// eforth calls it execute , ___ calls it perform
void KVMPerform(KsanaVm *vm)  // alias to 'execute' ,
{
        int n1;
        if (n1=KVMPop(vm)){  // 防呆機制 -hcchen5600 2008/11/03 22:35
                KVMExecute(vm, (void *) n1);
        }else{
        printf("Error! trying to execute a null pointer.\n");
        }
}

/* --------------------------Lesson 9 ------------------------*/

// [ n1 n2 -- n1>n2 ]  -hcchen5600 2008/11/03 22:31
void KVMGT(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) < KVMPop(vm) ? -1 : 0 );
}

// [ n1 n2 -- n1<n2 ]  -hcchen5600 2008/11/03 22:31
void KVMLT(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) > KVMPop(vm) ? -1 : 0 );
}

// ==
void KVMEqual(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) == KVMPop(vm) ? -1 : 0 );
}

/* ~~~~~~~~~~~ missing ones in eforth 23 basic words h.c. chen 2008/11/02 23:22:53  ~~~~~~~~~~~~~~~~ */
// [ n1 n2 -- bitwise AND]  -hcchen5600 2008/11/03 22:27
void KVMAnd (KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) & KVMPop(vm) );
}

// [ n1 n2 -- bit wise OR ]  -hcchen5600 2008/11/03 22:26
void KVMOr(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) | KVMPop(vm) );
}

// [ n1 n2 -- bit wise XOR ]
void KVMXor(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) ^ KVMPop(vm) );
}

// [ n1 -- NOT ]  become 0 or -1
void KVMNot(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) ? 0 : -1 );
}

// 0<  h.c. chen 2008/11/02 22:31:35
// ( n1 -- n<0 )
void KVMIsNegative(KsanaVm *vm)
{
        KVMPush(vm, KVMPop(vm) < 0 ? -1 : 0 );
}

void add_basic_words(KsanaVm *vm)
{
        // addbasicword(vm);
            KVMAddWord(vm, "+"          , KVMAdd                 );
            KVMAddWord(vm, "-"          , KVMMinus               );
            KVMAddWord(vm, "*"          , KVMMultiply            );
            KVMAddWord(vm, "/"          , KVMDivide              );
            KVMAddWord(vm, "mod"        , KVMMod                 );  // alias mod %
            KVMAddWord(vm, "drop"       , KVMDrop                );
            KVMAddWord(vm, "dup"        , KVMDup                 );
            KVMAddWord(vm, "swap"       , KVMSwap                );
            KVMAddWord(vm, "over"       , KVMOver                );
            KVMAddWord(vm, "bye"        , KVMBye                 );
            KVMAddWord(vm, ">"          , KVMGT                  );
            KVMAddWord(vm, "<"          , KVMLT                  );
            KVMAddWord(vm, "="          , KVMEqual               );
            KVMAddWord(vm, ".s"         , KVMDumpStack           );

            KVMAddWord(vm, ">r"         , KVMrto                 );
            KVMAddWord(vm, "r>"         , KVMrfrom               );
            KVMAddWord(vm, "r@"         , KVMRAt                 );
            KVMAddWord(vm, "execute"    , KVMPerform             );

            KVMAddWord(vm, "and"        , KVMAnd                 );
            KVMAddWord(vm, "or"         , KVMOr                  );
            KVMAddWord(vm, "xor"        , KVMXor                 );
            KVMAddWord(vm, "not"        , KVMNot                 );
            KVMAddWord(vm, "0<"         , KVMIsNegative          );
}
