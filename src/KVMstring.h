#ifndef KVM_STRING
#define KVM_STRING

void KVMdoStrQ(KsanaVm *vm);
void  KVMStrQ(KsanaVm *vm);
void KVMType(KsanaVm *vm);
void KVMBracketComment(KsanaVm *vm);
void KVMComment(KsanaVm *vm);
void KVMDotBracket(KsanaVm *vm);
void addstringword(KsanaVm *vm);

#endif