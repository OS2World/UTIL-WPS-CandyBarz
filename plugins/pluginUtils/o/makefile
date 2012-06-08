# $Id: makefile,v 1.2 2000/10/29 12:26:15 cwohlgemuth Exp $ 

#===================================================================================================
#
#	 This Makefile makes the Generic Plugin DLL (plugin.dll) for the CandyBarZ project.
#
#	 Please note the following :
#			1. This makefile requires GNU make version 3.76.1 or later.
#			2. This makefile was written with the tab expansion set to 2.	It will probably look
#					best (comments aligned correctly, etc.) with that tab setting.
#
#===================================================================================================
# Always clear the SUFFIXES dependency before resetting it.
.SUFFIXES:
# Now set the SUFFIXES dependency
.SUFFIXES: .c .dll .ipf .hlp .lib .obj .rc .res
# Assumes the makefile is in $(BASE)\o
BASE			=	..
# This assumes a common tree structure for each project version and
# subpart.
CDIR=$(BASE)\c
HDIR=$(BASE)\h
ODIR=$(BASE)\o
RDIR=$(BASE)\res
CC                =	icc.exe
RES						=	rc.exe
# These are the compiler flags that are common to all C compiles in the project.
CCFLAGS	=	/Ss /Q /Gm /G4 /I$(HDIR) /Ge- $(DEBUG_FLAGS)
#	These are the linker flags that are common to all Linker calls in the project
LD_FLAGS       =	/B"/pmtype:pm" /B"/nologo" $(DEBUG_LD)
OBJS			= pluginUtils.obj ColorChangeDlgProc.obj
OUTPUT		=	cbzutil.dll
VPATH       =	$(CDIR)
PLUGIN_DEF=$(RDIR)\pluginUtils.def
PLUGIN_RES	 =  $(RDIR)\pluginUtils.res
RESFLAGS =	-i $(HDIR) -i $(RDIR)
#	Conditional Variable, if the makefile command line includes NO_DEBUG=1 then the debugging flags
#	will be disabled.
ifdef NO_DEBUG
	DEBUG_FLAGS    = /Ti- /O+
	DEBUG_LD       =
else
	DEBUG_FLAGS    = /Ti+ /O-
	DEBUG_LD       = /B"/de" /B"/MAP" /B"/LINENUMBERS"
endif

# Default Dependencies
%.obj: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<

ALL: $(OUTPUT)
.PHONY:	clean
clean:
	- del $(OUTPUT) $(OBJS)

$(OUTPUT): $(OBJS) $(PLUGIN_DEF) $(PLUGIN_RES)
	@echo " Link::Linker "
	@echo " Bind::Resource Bind "
	$(CC) $(CCFLAGS) /B"/dll" $(LD_FLAGS) /B"/e:1" /B"/noe" /Fe $@ $(OBJS) $(PLUGIN_DEF)
	$(RES) $(PLUGIN_RES) $(OUTPUT)
	implib cbzutil.lib cbzutil.dll
ifdef NO_DEBUG

else
	mapsym $(basename $(OUTPUT)) 	
endif

$(PLUGIN_RES):  $(RDIR)\pluginUtils.rc $(RDIR)\pluginUtils.dlg
	cd $(RDIR) && $(RES) $(RESFLAGS) -r pluginUtils.rc
