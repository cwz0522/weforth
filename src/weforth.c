//main loop of Ksana Minimal Virtual Machine
//release under GPL 3.0
//yap 2007.12.20


#include "vm.h"
//#include "lesson.h"
#include "compiler.h"

void KVMdotGreeting(KsanaVm *vm)
{
    if(vm->greetingswitch) {
		    //0        1         2         3         4         5         6         7         8
		    //12345678901234567890123456789012345678901234567890123456789012345678901234567890
		puts("WF06 (Win32) by  H.C. Chen 2010/06/10 12:44:59 ");
		puts("Based on yap's KsanaVM forth machine http://tutor.ksana.tw");
		puts("http://mfgkm-wks.wistron.com/RD/Software/Wistron EFI Forth Release Notes.aspx  ");
	}
}


int main(int argc, char **argv)
{
        char cmd [1024];         // buffer to hold input command
        KsanaVm *vm;             // the instance of virtual machine

        g_pLog = fopen("log.txt", "w");
        fprintf(g_pLog, "weforth start.\n"); fflush(g_pLog);
        
        vm=KVMCreate();          // create the virtual machine
        fprintf(g_pLog, "KVMCreate\n"); fflush(g_pLog);
        memset(cmd,0,sizeof(cmd)); // initial all values to zero
        fprintf(g_pLog, "memset\n"); fflush(g_pLog);

        add_c_code_words(vm, argc, argv, 6);  // the last element is WFxx version
        fprintf(g_pLog, "add_c_code_words\n"); fflush(g_pLog);
        Eval(vm,"include autoexec.f &vm->greetingswitch ! .greeting");
        fprintf(g_pLog, "Eval(vm,include autoexec.f &vm->greetingswitch ! .greeting);\n"); fflush(g_pLog);
        while (! vm->terminate) {
                printf("\nok>");      // the command prompt
                gets(cmd);          // get the command from console

				// KVMSetTib(vm,cmd);  // set it as Vm's terminal input buffer
                Eval(vm,cmd);
        }

        KVMFree(vm);   // free the virtual machine
        return(vm->sp >= 0 ? KVMPop(vm) : 0);
}