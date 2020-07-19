#             PTC             File
# -----------------------------------------------------------------------------
# File Name
MAKEFILENAME = makefile

# Pro/ENGINEER Source
PROTOOL_SRC = D:\ptc\Creo 2.0

# Pro/ENGINEER Datacode
PROTOOL_DATECODE = M230

# Application names
APP_NAME = weforth

# Machine Type
PRO_MACHINE_TYPE = x86e_win64

# Executable names
APP_EXE = ../$(APP_NAME).exe
APP_DLL = ../$(APP_NAME).dll

# Pro/Toolkit Source & Machine Loadpoint
PROTOOL_SYS = $(PROTOOL_SRC)/COMMON FILES/$(PROTOOL_DATECODE)/protoolkit/$(PRO_MACHINE_TYPE)

# Include File Paths
INCS = -I. -I"$(PROTOOL_SYS)/../includes"

# Compiler Flags
CC = cl
MACH = -DPRO_MACHINE=36 -DPRO_OS=4
CCFLAGS = -c -GS -fp:precise -D_WSTDIO_DEFINED
CFLAGS = $(CCFLAGS) $(INCS) $(MACH)

# Libraries
PTCLIBS = "$(PROTOOL_SYS)/obj/protoolkit.lib"
PTCLIBS_DLL = "$(PROTOOL_SYS)/obj/protk_dll.lib"
LIBS = libcmt.lib kernel32.lib user32.lib wsock32.lib advapi32.lib mpr.lib winspool.lib netapi32.lib psapi.lib gdi32.lib shell32.lib comdlg32.lib ole32.lib ws2_32.lib

# Object files
OBJS = $(APP_NAME).obj basic.obj compiler.obj progflow.obj string.obj io.obj var.obj vm.obj winapi.obj
 
# Linker
LINK = link

$(APP_EXE) :  $(OBJS) $(PTCLIBS)
	@echo Start at `date`
	@echo PROTOOL_SRC = $(PROTOOL_SRC)
        @echo PROTOOL_SYS = $(PROTOOL_SYS)
#Executable applications compiled using WinMain() instead of main() should set the subsystem to windows instead of console.
	$(LINK) /subsystem:console -out:$(APP_EXE) /debug:none /machine:amd64 @<<longline.list
$(OBJS) $(PTCLIBS) $(LIBS)
<<

	@echo Finished at `date`
	del $(OBJS)

# Dll target
dll: $(APP_DLL)

$(APP_DLL) :  $(OBJS) $(PTCLIBS_DLL)
	@echo Start at `date`
	@echo PROTOOL_SRC = $(PROTOOL_SRC)
        @echo PROTOOL_SYS = $(PROTOOL_SYS)
	$(LINK) /subsystem:console -out:$(APP_DLL) /debug:none /machine:amd64 /dll @<<longline.list
$(OBJS) $(PTCLIBS_DLL) $(LIBS)
<<

	@echo Finished at `date`
	del $(OBJS)

# object dependencies

$(APP_NAME).obj:  $(APP_NAME).c vm.h compiler.h
	$(CC) $(CFLAGS) $(APP_NAME).c

basic.obj: basic.c vm.h KVMstring.h
	$(CC) $(CFLAGS) basic.c

compiler.obj: compiler.c compiler.h progflow.h vm.h
	$(CC) $(CFLAGS) compiler.c

progflow.obj: progflow.c basic.h progflow.h
	$(CC) $(CFLAGS) progflow.c

string.obj: string.c vm.h progflow.h
	$(CC) $(CFLAGS) string.c

io.obj: io.c vm.h
	$(CC) $(CFLAGS) io.c

var.obj: var.c compiler.h var.h vm.h
	$(CC) $(CFLAGS) var.c

vm.obj: vm.c vm.h compiler.h progflow.h io.h var.h KVMstring.h winapi.h basic.h
	$(CC) $(CFLAGS) vm.c

winapi.obj: winapi.c vm.h var.h
	$(CC) $(CFLAGS) winapi.c

clean :
	del $(OBJS)
	del $(APP_EXE)
	nmake -f $(MAKEFILENAME)

clean_dll :
	del $(OBJS)
	del $(APP_DLL)
	nmake -f $(MAKEFILENAME) dll
