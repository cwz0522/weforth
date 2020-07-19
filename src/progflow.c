/*
Ksana Minimal Virtual Machine
yap 2007.12.21
*/

#include "basic.h"
#include "progflow.h"

extern const char must_be_in_compile_state[] = "Error! must be in compile state";

/* --------------------------Lesson 9 ------------------------*/
void branch(KsanaVm *vm)
{
        vm->ip=*(int*)(vm->ip);   // 下一格裡放的就是要 branch 過去的地方。 KVMLoop() KVMThen() 等會 compile 進去。  -hcchen5600 2008/11/08 15:36
}          
                    
// zbranch KVM 2008-Jan 版本來沒有。我現在瞭解了，它其實就是 doIf()。 -hcchen5600 2008/11/09 09:57
void doIf(KsanaVm *vm)
{
        if (0==KVMPop(vm)) branch(vm);
        else vm->ip+=CELLSIZE;
}

// zbranch 的反邏輯 qbranch 就是 ?break, 用於 do-loop 裡面。可以在 compile zbranch(i.e. doIf) 之前加 compile 一個 KVMNot 來取代。
// void qbranch(KsanaVm *vm)
// {
//         if (KVMPop(vm)) branch(vm);
//         else vm->ip+=CELLSIZE;
// }

/*
 FORTH:  xxx if true_clause then yyy

        xxx
        doIf
        [address]           //jump to address if not true
       true_clause
address: 
    yyy
    
FORTH:   xxx if true_clause else false_clause then yyy

    xxx
    doIf
        [addr1]              // jump to addr1 if not true
           code for "true"

    branch
        [addr2]   -->      // address after "then"
addr1:
           code for "false"
addr2:
    yyy

*/
void KVMIf(KsanaVm *vm)
{
        if (!vm->compiling ) {
                printf("%s => if\n", must_be_in_compile_state);
                return;
        }
        DictCompile(vm,doIf);
        KVMPush(vm,vm->dictionary +vm->here); //current dictionary pointer
        DictCompile(vm,0xFFFFFFFF) ; //jump address
}
void KVMThen(KsanaVm *vm)
{
        if (!vm->compiling ) {
                printf("%s => then\n", must_be_in_compile_state);
                return;
        }
        *(int*)(KVMPop(vm)) = vm->dictionary +vm->here ;
}
void KVMElse(KsanaVm *vm)
{
        int jmp;
        if (!vm->compiling ) {
                printf("%s => else\n", must_be_in_compile_state);
                return;
        }

        jmp=KVMPop(vm);
        DictCompile(vm,branch);
        KVMPush(vm,vm->dictionary +vm->here);
        DictCompile(vm,0xEEEEEEEE);
        *(int *)(jmp) = vm->dictionary +vm->here;
}       


/* --------------------------Lesson 11 ------------------------*/
/*

FORTH:  xxx DO to_do LOOP yyy

xxx
doDo
address:
    to_do
doLoop
[address]
yyy

DO       ( limit index -- )
LOOP
+LOOP   add top of stack to index
I    put the index on data stack
*/

//runtime code of DO
void doDo(KsanaVm *vm)
{
        int index= KVMPop(vm);
        int limit= KVMPop(vm);

        //push limit and index on return stack
        KVMRPush(vm,limit);
        KVMRPush(vm, index);
}

//runtime code of LOOP
void doLoop(KsanaVm *vm)
{
        int index,limit;
        index=KVMRPop(vm)+1;
        limit=KVMRPop(vm);
        if (index>=limit) {
                vm->ip+=CELLSIZE;  //  -hcchen5600 2008/11/08 15:24　這就是 break ?break 的線索。 
        } else {
                KVMRPush(vm,limit);
                KVMRPush(vm,index);
                branch(vm);                // jump to code after DO
        }
}

//runtime code of +LOOP
void doPlusLoop(KsanaVm *vm)
{
        int index,limit, offset;
        offset = KVMPop(vm);
        index  = KVMRPop(vm) + offset;
        limit  = KVMRPop(vm);

	// for positive +loop, exit condition is limit <= count.
	// for nagetive +loop, exit contition is limit > count. (not >= just for ANSI compliance)
	// to avoid the inconsistancy, I use ==. But it's wrong also. http://groups.google.com/group/ksanavm/msg/260eb0038ba29767
	// 應該根據 +loop 的正負來機動調整。   -hcchen5600 2008/11/09 19:39

        if (
        	(offset>0) && (limit <= index) 
        	||
        	(offset<0) && (limit > index) // change >= to > for ANSI compliance. 依陳爽老師指正 http://groups.google.com/group/ksanavm/msg/8cb630923f850f36?hl=zh-TW
        ){      // leaving the loop
                vm->ip+=CELLSIZE;
        } else {
        	// continue the loop
                KVMRPush(vm,limit);
                KVMRPush(vm,index);
                vm->ip =*(int*)(vm->ip);  // loop back
        }
}


void KVMDo(KsanaVm *vm)
{
        if (!vm->compiling ) {
                printf("%s => do\n", must_be_in_compile_state);
                return;
        }

        DictCompile(vm,doDo);           // runtime of do
        KVMPush(vm,vm->dictionary +vm->here); // save HERE to stack
        KVMPush(vm,0); // 原先 DO 只 push 自己的 here 進去，現在要加 push 一個 count 即 0 進 data stack  -hcchen5600 2008/11/09 10:53  https://docs.google.com/Doc?id=dgzzwq68_540c3fd97vt&hl=zh_TW
}

void KVMLoop(KsanaVm *vm)
{
        if (!vm->compiling ) {
                printf("%s => loop\n", must_be_in_compile_state);
                return;
        }

        DictCompile(vm,doLoop);          // the runtime of loop
        // Compile 到 KVMLoop() KVMPlusloop() 時，閉著眼睛把當時的 here 一一回填到小 list 裡每一格裡去 https://docs.google.com/Doc?id=dgzzwq68_540c3fd97vt&hl=zh_TW
        while (KVMPop(vm)) {
            *(int *) KVMPop(vm) = vm->dictionary + vm->here + CELLSIZE; // store the way out to each breaks' parameter , one more CELLSIZE to skip the below 'begin address of Do'
        }
        DictCompile(vm, KVMPop(vm)); // begin address of Do
}

void KVMPlusLoop(KsanaVm *vm)
{
        if (!vm->compiling ) {
                printf("%s => +loop\n", must_be_in_compile_state);
                return;
        }
        DictCompile(vm,doPlusLoop);    // the runtime of +loop
        // Compile 到 KVMLoop() KVMPlusloop() 時，閉著眼睛把當時的 here 一一回填到小 list 裡每一格裡去 https://docs.google.com/Doc?id=dgzzwq68_540c3fd97vt&hl=zh_TW
        while (KVMPop(vm)) {
            *(int *) KVMPop(vm) = vm->dictionary + vm->here + CELLSIZE; // store the way out to each breaks' parameter , one more CELLSIZE to skip the below 'begin address of Do'
        }
        DictCompile(vm, KVMPop(vm)); // begin address of Do
}

void KVMDoBreak (KsanaVm *vm) // 先用 code word 改寫，成功了再來想如何改寫成 colon word 。
{
		branch(vm);
		KVMRPop(vm);
		KVMRPop(vm);
}

// IMMEDIATE [x] https://docs.google.com/Doc?id=dgzzwq68_540c3fd97vt&hl=zh_TW
void KVMBreak (KsanaVm *vm) // 先用 code word 改寫，成功了再來想如何改寫成 colon word 。
{       int count;
        DictCompile(vm,KVMDoBreak);      // the runtime of break is actually simply a branch
        count = KVMPop(vm); // was from DO or previous break ?break
        KVMPush(vm, count); // 不要把舊 count 用掉
        KVMPush(vm,vm->dictionary +vm->here); // do break ?break 大家都一次 push 兩個 cell ， 先 here 後 count。
        KVMPush(vm, count + 1); // ... 現在要加 push 一個 count ... 每個 break ?break 都把 count + 1 比照辦理。
        DictCompile(vm,0xFFFFFFFF) ; //jump address
}         
                   
void KVMDoQbreak (KsanaVm *vm) // 先用 code word 改寫，成功了再來想如何改寫成 colon word 。
{
	if (KVMPop(vm)) {
		branch(vm);
		KVMRPop(vm);
		KVMRPop(vm);
	} else{
		vm->ip+=CELLSIZE;
	}
}

// IMMEDIATE [x] https://docs.google.com/Doc?id=dgzzwq68_540c3fd97vt&hl=zh_TW
void KVMQbreak (KsanaVm *vm)   // 先用 code word 改寫，成功了在來想如何改寫成 colon word 。
{       int count;
 
        DictCompile(vm,KVMDoQbreak);  
        count = KVMPop(vm); // was from DO or previous break ?break
        KVMPush(vm, count); // 不要把舊 count 用掉
        KVMPush(vm,vm->dictionary +vm->here); // do break ?break 大家都一次 push 兩個 cell ， 先 here 後 count。
        KVMPush(vm, count + 1); // ... 現在要加 push 一個 count ... 每個 break ?break 都把 count + 1 比照辦理。
        DictCompile(vm,0xFFFFFFFF) ; //jump address
}         

void KVMI(KsanaVm *vm)
{
        KVMPush( vm, KVMRTOS(vm) );  //get index
}

/* --------------------------Lesson 16 ------------------------*/
void KVMRecurse(KsanaVm *vm)
{
        DictCompile(vm,vm->newword.xt);
}

void addflowword(KsanaVm *vm)
{
        //lesson 10
        KVMAddWord(vm, "branch",  branch);      //  -hcchen5600 2008/11/03 20:49
        KVMAddWord(vm, "zbranch", doIf);        //   -hcchen5600 2008/11/09 11:00
        KVMAddWord(vm, "if",      KVMIf);       KVMSetImmediate(vm);  
        KVMAddWord(vm, "then",    KVMThen);     KVMSetImmediate(vm);
        KVMAddWord(vm, "else",    KVMElse);     KVMSetImmediate(vm);
                                                
        //lesson 11                             
        KVMAddWord(vm, "do",      KVMDo);       KVMSetImmediate(vm);  
        KVMAddWord(vm, "loop",    KVMLoop);     KVMSetImmediate(vm);
        KVMAddWord(vm, "+loop",   KVMPlusLoop); KVMSetImmediate(vm);
        KVMAddWord(vm, "i",       KVMI); 
        KVMAddWord(vm, "break",   KVMBreak);    KVMSetImmediate(vm);    // -hcchen5600 2008/11/09 11:06 
        KVMAddWord(vm, "?break",  KVMQbreak);   KVMSetImmediate(vm);    // -hcchen5600 2008/11/09 11:06 
                                                
        //lesson 15
        KVMAddWord(vm, "recurse", KVMRecurse);  KVMSetImmediate(vm);
}

/*
http://claymore.engineer.gvsu.edu/~steriana/Software/pfavr/design.html
*/