#===================================================================================================
#
#   This Makefile makes the CandyBarZ DLL (CandyBar.dll) for the CandyBarZ project.
#
#   Please note the following :
#			1.	This makefile requires GNU make version 3.76.1 or later.
#			2.	This makefile was written with the tab expansion set to 2.  It will probably look
#					best (comments aligned correctly, etc.) with that tab setting.
#			3.	This makefile requires the following environment variable be defined :
#
#	 				  CANDY should be set to the root directory of the CandyBarZ source tree.  I.e. the
#						CANDY BASE DIRECTORY.
#
#           E.g.		set CANDY=D:\DEVELOPMENT\PROJECTS\CANDY\SRC
#
#===================================================================================================
# Include the common defines for the CandyBarZ project
include  $(CANDY)\candy.inc
BASE			=	..
DLL_FLAG	=	 /Ge-
OBJS	 		= CandyBarZ.obj CandyBarZTBPaintProc.obj CandyBarZTBProc.obj CandyBarZUtil.obj \
			  CandyBarZFrameProc.obj CandyBarZFramePaintProc.obj \
			  CandyBarZStaticProc.obj CandyBarZContainerProc.obj  CandyBarZScrollBarProc.obj\
		   	  CandyBarZButtonProc.obj CandyBarZNotebookProc.obj \
		   	  CandyBarZMenuProc.obj CandyBarZMenuPaintProc.obj \
			  CandyBarZCommon.obj debug.obj except.obj

# CandyBarZButtonPaintProc.obj

OUTPUT		=	$(CANDY_DLL)
VPATH			=	$(CDIR)
ALL: $(OUTPUT)
.PHONY:	clean
clean:
	- del $(LNK_CMN).obj $(OUTPUT) $(OBJS)
$(OUTPUT): $(OBJS) $(RDIR)\CandyBarZ.def
	@echo " Link::Linker "
	@echo " Bind::Resource Bind "
	$(CC) $(CCFLAGS) /B"/dll" $(LD_FLAGS) /B"/e:1" /B"/noe" /Fe $@ $^
#MMPM2.LIB
	
#===================================================================================================
#
#
#   $Id: candydll.mak,v 1.3 2000/10/05 15:09:52 cwohlgemuth Exp $
#
#
#   $Log: candydll.mak,v $
#   Revision 1.3  2000/10/05 15:09:52  cwohlgemuth
#   Added rules for new procedures recently added to tree.
#
#   Revision 1.2  1999/12/29 19:38:11  enorman
#   Resync'ed with my local tree
#
#   Revision 1.1  1999/06/15 20:48:02  ktk
#   Import
#
#   Revision 1.2  1998/10/17 21:42:29  pgarner
#   Created macros for the target names CandyBar.DLL, CBSetup.exe and CBSetup.hlp
#   I put these macros in candy.inc.  This helps to assure that all of the
#   makefiles see the same target names.  I also moved all of the compile flags
#   into candy.inc and just have a DLL_FLAG switch (Ge+/-) to differentiate the
#   compile for the DLL versus the executable.
#
#   Revision 1.1  1998/10/17 19:52:17  pgarner
#   Initial revision
#
#
#
#
#===================================================================================================
