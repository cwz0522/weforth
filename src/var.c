#include "compiler.h"
#include "var.h"
#include "vm.h"

// #define KVM_warning_dangerous_memory_address if (vm->debugmode && !(p>=vm->dictionary && p<=vm->dictionary +vm->here)) \
//					     printf(KVM_warning_dangerous_address, p)
// const char KVM_warning_dangerous_address[] = "KVM warning: dangerous address 0x%x\n";
#define KVM_warning_dangerous_memory_address /* do nothing */

//lesson 11
void  KVMdoVariable(KsanaVm *vm)
{
        KVMPush(vm,vm->ip);
        KVMRet(vm);
}
// ( -name- )
void  KVMVariable(KsanaVm *vm)
{
        KVMNextToken(vm);
        strncpy(vm->newword.name,vm->token,sizeof(vm->newword));
        vm->newword.xt = vm->dictionary + vm->here;

        DictCompile(vm, KVMdoVariable);  // must add this for high level word
        DictCompile(vm,0);                      //initial value
        KVMAddWord(vm,vm->newword.name,vm->newword.xt);
}

// ( v -- n )
void  KVMFetch(KsanaVm *vm)
{
        int *p, i=0;
        p=KVMPop(vm);
        KVM_warning_dangerous_memory_address;
//      if (p>=vm->dictionary && p<=vm->dictionary +vm->here) {
                i= *p;
                KVMPush (vm, i);
//      } else {
//              printf("invalid address 0x%x\n", p);
//      }
}

// ( n v  --  )
void  KVMStore(KsanaVm *vm)
{
        int *p, i;
        p=KVMPop(vm);
        i=KVMPop(vm);
        KVM_warning_dangerous_memory_address;
//      if (p>=vm->dictionary && p<=vm->dictionary +vm->here) {   // 2008/11/06 20:22:14  h.c. chen
                *p=i;
//      } else {
//              printf("invalid address 0x%x\n", p);
//      }
}

// ( v -- n )
void  KVMCFetch(KsanaVm *vm)
{
        unsigned char *p, i;
        p = KVMPop(vm);
        KVM_warning_dangerous_memory_address;
//      if (p>=vm->dictionary && p<=vm->dictionary +vm->here) {   // h.c. chen 2008/11/06 21:03:22
                i = *p;
                KVMPush (vm, i);
//      } else {
//              printf("invalid address 0x%x\n", p);
//      }
}
// ( n v  --  )
void  KVMCStore(KsanaVm *vm)
{
        unsigned char *p, i;
        p = KVMPop(vm);
        i = (unsigned char) KVMPop(vm);
        KVM_warning_dangerous_memory_address;
//      if (p>=vm->dictionary && p<=vm->dictionary +vm->here) {  // h.c. chen 2008/11/06 21:04:44
                *p = i;
//      } else {
//              printf("invalid address 0x%x\n", p);
//      }
}
// ( address count c -- )
void  KVMFill(KsanaVm *vm)
{
        unsigned char c = KVMPop(vm);
        int count = KVMPop(vm);
        unsigned char *p = KVMPop(vm);
        int i;

        if (vm->debugmode && !((p>=vm->dictionary && p<=vm->dictionary +vm->here)       //   KVM_warning_dangerous_memory_address; h.c. chen 2008/11/06 21:08:17
          &&((p+count)>=vm->dictionary && (p+count)<=vm->dictionary +vm->here))){
                printf("KVM warning: dangerous filling address 0x%x count 0x%x\n", p, count);
        }
        for (i=0; i<count; i++)  {
                *p++ = c;
        }
}

//lesson 12
void  KVMcreate(KsanaVm *vm)
{       KsanaWord *w;
	
        KVMNextToken(vm);
        strncpy(vm->newword.name,vm->token,sizeof(vm->newword));
        vm->newword.xt = vm->dictionary + vm->here;

        DictCompile(vm, KVMdoVariable);  // must add this for high level word

        w = KVMGetWord(vm, vm->newword.name);  // h.c. chen 2008/11/07 22:58:23  improve the programming reliability
        if (w) printf("%s is not unique\n", vm->newword.name);

        KVMAddWord(vm,vm->newword.name,vm->newword.xt);
}
//lesson 17
void  KVMAllot(KsanaVm *vm)
{
        vm->here += KVMPop(vm);
}

void ChangeCreateCodeField(KsanaVm *vm,KVMXT xt)
{
        *(int*)vm->newword.xt = xt;
}
void  KVMDoes(KsanaVm *vm)
{
        int code=vm->ip;
        //change doCreate to code after does>
        ChangeCreateCodeField(vm,code);
        //stop here, code following does> are not executed.
        KVMRet(vm);
}

void addvarword(KsanaVm *vm)
{
        //lesson 12
        KVMAddWord(vm,"variable",KVMVariable);
        KVMAddWord(vm, "@", KVMFetch);
        KVMAddWord(vm, "!", KVMStore);
        //lesson 17
        KVMAddWord(vm, "create", KVMcreate);
        KVMAddWord(vm, "allot", KVMAllot);
        KVMAddWord(vm, "c@", KVMCFetch);
        KVMAddWord(vm, "c!", KVMCStore);
        KVMAddWord(vm, "fill", KVMFill);

        KVMAddWord(vm, "does>", KVMDoes);
}
