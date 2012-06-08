#===================================================================================================
#
#   This is the make include file for the CandyBarz project.  This make include file contains all
#		of the definitions that are common to the WHOLE CandyBarz project.
#
#   Please note the following :
#			1.	This include file requires GNU make version 3.76.1 or later.
#			2.	This include file was written with the tab expansion set to 2.  It will probably look
#					best (comments aligned correctly, etc.) with that tab setting.
#			3.	This makefile requires the following environment variable be defined :
#
#	 				  CANDY should be set to the root directory of the CandyBarZ source tree.  I.e. the
#						CANDY BASE DIRECTORY.
#
#           E.g.		set CANDY=D:\DEVELOPMENT\PROJECTS\CANDY\SRC
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
UTILBASE=$(CANDY)\plugins\pluginUtils
UTILODIR=$(UTILBASE)\o
UTILHDIR=$(UTILBASE)\h

CC                =	icc.exe
RES						=	rc.exe

# These are the compiler flags that are common to all C compiles in the project.
CCFLAGS	=	/Ss /Q /Gm /G4 /I$(HDIR) /I$(UTILHDIR) /Ge- $(DEBUG_FLAGS)

#	These are the linker flags that are common to all Linker calls in the project
LD_FLAGS       =	/B"/pmtype:pm" /B"/nologo" $(DEBUG_LD)
VPATH       =	$(CDIR)
PLUGIN_DEF=$(RDIR)\plugin.def
PLUGIN_RES	 =  $(RDIR)\plugin.res
RESFLAGS =	-i $(HDIR) -i $(RDIR) -i $(UTILHDIR)

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
	$(CC) $(CCFLAGS) /B"/dll" $(LD_FLAGS) /B"/e:1" /B"/noe" /Fe $@ $(OBJS) $(PLUGIN_DEF) MMPM2.LIB $(UTILODIR)\cbzutil.lib
	$(RES) $(PLUGIN_RES) $(OUTPUT)
ifdef NO_DEBUG

else
	mapsym $(basename $(OUTPUT)) 	
endif


$(PLUGIN_RES):  $(RDIR)\plugin.rc $(RDIR)\plugin.dlg
	cd $(RDIR) && $(RES) $(RESFLAGS) -r plugin.rc

