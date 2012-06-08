#===================================================================================================
#
#   This Makefile makes the CBSetup Executable (CBSetup.exe) for the CandyBarZ project.
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
BASE			=	..
DLL_FLAG	=	 /Ge+
include  ..\theme.inc

OBJS1 		= themeBuilder.obj themeBuilderUtil.obj InfoDlgProc.obj WindowClassDlgProc.obj \
			CandyBarZParser.obj CandyBarZScanner.obj \
			CandyBarZPSRT.obj

OUTPUT1 	=	CBBuild.exe
#OUTPUT2 	=	themeBuilder.hlp
SETUP_DEF	=	$(RDIR)\themeBuilder.def
SETUP_RES	=	$(ODIR)\themeBuilder.res

VPATH			=	$(CDIR);$(RDIR);$(CDIR2)


RESFLAGS	=	-i $(HDIR) -i $(RDIR) -i $(CANDY)\h -i $(CANDY)\res
ALL: $(OUTPUT1) #$(OUTPUT2)

.PHONY:	clean
clean:
	- del $(OUTPUT1) $(OUTPUT2) $(OBJS1) $(SETUP_RES)

CandyBarZParser.obj:	$(CDIR2)\CandyBarZParser.c
	$(CC) $(CCFLAGS) -c -o $@ $<
$CandyBarZScanner.obj:	$(CDIR2)\CandyBarZScanner.c
	$(CC) $(CCFLAGS) -c -o $@ $<
CandyBarZPSRT.obj:	$(CDIR2)\CandyBarZPSRT.c
	$(CC) $(CCFLAGS) -c -o $@ $<
	

#  Please note we had to specify the path to use to link in $(LNK_CMN).c, whereas we do not have to
#  when linking the DLL.  This is because $(SETUP.RES) is a dependency, but not a linkd object in
#  The output, so we cannot use the $^ as the list of objects to link, but have to specify them so
#  that $(SETUP_RES) does not get linked!
$(OUTPUT1):	$(OBJS1) $(SETUP_RES)
	$(CC) $(CCFLAGS) $(LD_FLAGS) /B"/e:1" /B"/noe" /Fe $@ $(OBJS1) $(SETUP_DEF) 
	rc $(SETUP_RES) $(OUTPUT1)

$(SETUP_RES):	$(RDIR)\themeBuilder.rc $(RDIR)\themeBuilder.dlg
	cd $(RDIR) && $(RES) $(RESFLAGS) -r themeBuilder.rc $(SETUP_RES)

	
#===================================================================================================
#
#
#   $Id: themeBuilder.mak,v 1.1 1999/06/15 20:48:30 ktk Exp $
#
#
#   $Log: themeBuilder.mak,v $
#   Revision 1.1  1999/06/15 20:48:30  ktk
#   Import
#
#   Revision 1.2  1998/10/17 21:44:37  pgarner
#   Created macros for the target names CandyBar.DLL, CBSetup.exe and CBSetup.hlp
#   makefiles see the same target names.  I also moved all of the compile flags
#   into candy.inc and just have a DLL_FLAG switch (Ge+/-) to differentiate the
#   compile for the DLL versus the executable.
#
#   Revision 1.1  1998/10/17 19:52:36  pgarner
#   Initial revision
#
#
#
#
#===================================================================================================
