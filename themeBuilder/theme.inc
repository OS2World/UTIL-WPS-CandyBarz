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


# This assumes a common tree structure for each project version and
# subpart.  The BASE variable is defined in the makefile itself
CDIR=$(BASE)\c
HDIR=$(BASE)\h
ODIR=$(BASE)\o
RDIR=$(BASE)\res
HDIR2=$(CANDY)\h
CDIR2=$(CANDY)\c
CANDYOBJ	=	$(CANDY)\o

CC						=	icc.exe
RES						=	rc.exe


# These are the compiler flags that are common to all C compiles in the project.  Note that the
# DLL flag is set to /Ge+ or /Ge- by the individual makefile itself.
CCFLAGS	=	 /Ss /Q /Gm /G4 /I$(HDIR) /I$(HDIR2) $(DLL_FLAG) $(DEBUG_FLAGS) /Wuse


#  These are the linker flags that are common to all Linker calls in the project
LD_FLAGS			=	/B"/pmtype:pm" /B"/nologo" $(DEBUG_LD)


#  Conditional Variable, if the makefile command line includes NO_DEBUG=1 then the debugging flags
#  will be disabled.
ifdef NO_DEBUG
	DEBUG_FLAGS		= /Ti- /O+
	DEBUG_LD			=
else
	DEBUG_FLAGS		= /Ti+ /O-
	DEBUG_LD			= /B"/de"	
endif


# Always clear the SUFFIXES dependency before resetting it.
.SUFFIXES:

# Now set the SUFFIXES dependency
.SUFFIXES: .c .dll .ipf .hlp .lib .obj .rc .res


# Default Dependencies
%.obj: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<


#=============================================================================================================
#
#
#   $Id: theme.inc,v 1.1 1999/06/15 20:48:29 ktk Exp $
#
#
#   $Log: theme.inc,v $
#   Revision 1.1  1999/06/15 20:48:29  ktk
#   Import
#
#   Revision 1.6  1998/10/17 21:40:29  pgarner
#   Created macros for the target names CandyBar.DLL, CBSetup.exe and CBSetup.hlp
#   This assures that all of the various makefiles and dependencies have the
#   same target names
#
#   Revision 1.5  1998/10/17 19:42:57  pgarner
#   Changed this considerably.  We have eliminated psrt.dll, we have made
#   ALL the directories dependant upon a single environment variable, and we
#   have converted the project to GNU make.
#
#   Revision 1.4  1998/06/29 15:22:20  pgarner
#   Changed the make variable that contains the base directory from common to
#   candy to prevent conflicts with another project.
#
#   Revision 1.3  1998/06/29 08:11:06  pgarner
#   Changed the drive from E: to D:  I probably need to revamp this make include
#   so that the common path in a env variable.
#
#   Revision 1.2  1998/06/12 17:24:23  pgarner
#   For some reason I had left out the RCS comment block!  This includes it!
#
#   Revision 1.1  1998/06/08 14:18:58  pgarner
#   Initial Checkin
#
#
#
#=============================================================================================================

