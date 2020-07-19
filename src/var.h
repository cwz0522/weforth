
#ifndef KVM_VAR
#define  KVM_VAR
const char KVM_warning_dangerous_address[];

void  KVMdoVariable(KsanaVm *vm);
void  KVMVariable(KsanaVm *vm);
void  KVMFetch(KsanaVm *vm);
void  KVMStore(KsanaVm *vm);
void  KVMCFetch(KsanaVm *vm);
void  KVMCStore(KsanaVm *vm);
void  KVMFill(KsanaVm *vm);
void  KVMcreate(KsanaVm *vm);
void  KVMAllot(KsanaVm *vm);
void  ChangeCreateCodeField(KsanaVm *vm,KVMXT xt);
void  KVMDoes(KsanaVm *vm);
void  addvarword(KsanaVm *vm);

#endif