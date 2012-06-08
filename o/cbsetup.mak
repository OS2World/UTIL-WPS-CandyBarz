#===================================================================================================
#
#   This Makefile makes the CBSetup Executable (CBSetup.exe) for the CandyBarZ project.
#
#   Please note the following :
#        1. This makefile requires GNU make version 3.76.1 or later.
#        2. This makefile was written with the tab expansion set to 2.  It will probably look
#              best (comments aligned correctly, etc.) with that tab setting.
#        3. This makefile requires the following environment variable be defined :
#
#                CANDY should be set to the root directory of the CandyBarZ source tree.  I.e. the
#                 CANDY BASE DIRECTORY.
#
#           E.g.     set CANDY=D:\DEVELOPMENT\PROJECTS\CANDY\SRC
#
#===================================================================================================

# Include the common defines for the CandyBarZ project
include  $(CANDY)\candy.inc

BASE        =  ..

DLL_FLAG =   /Ge+

OBJS1       = ColorDlgProc.obj CustomizeDlgProc.obj InstallDlgProc.obj Setup.obj SetupUtil.obj \
      CandyBarZParser.obj CandyBarZScanner.obj PreviewDlgProc.obj      
#     ButtonDecorDlgProc.obj  ButtonDecorDlgProc2.obj readbitmap.obj \
#     themesmaindlgproc.obj
#GlobalsDlgProc.obj

OUTPUT1  =  $(CBSETUP)
OUTPUT2  =  $(CB_HELP)
SETUP_DEF   =  $(RDIR)\Setup.def
SETUP_RES   =  $(ODIR)\setup.res

VPATH       =  $(CDIR);$(RDIR)


RESFLAGS =  -i $(HDIR) -i $(RDIR)
ALL: $(OUTPUT1) $(OUTPUT2)

.PHONY:  clean
clean:
	- del $(LNK_CMN).obj $(OUTPUT1) $(OUTPUT2) $(OBJS1) $(SETUP_RES)


#  Please note we had to specify the path to use to link in $(LNK_CMN).c, whereas we do not have to
#  when linking the DLL.  This is because $(SETUP.RES) is a dependency, but not a linkd object in
#  The output, so we cannot use the $^ as the list of objects to link, but have to specify them so
#  that $(SETUP_RES) does not get linked!
$(OUTPUT1): $(CDIR)\$(LNK_CMN).c $(OBJS1) $(SETUP_RES)
	$(CC) $(CCFLAGS) $(LD_FLAGS) /B"/e:1" /B"/noe" /Fe $@ $(CDIR)\$(LNK_CMN).c $(OBJS1) $(SETUP_DEF) 
#MMPM2.LIB
	rc $(SETUP_RES) $(OUTPUT1)

$(OUTPUT2): $(RDIR)\Setup.ipf $(HDIR)\Setup.h
	ipfc $(RDIR)\Setup.ipf $@ /W3
$(SETUP_RES):  $(RDIR)\setup.rc $(RDIR)\setup.dlg
	cd $(RDIR) && $(RES) $(RESFLAGS) -r setup.rc $(SETUP_RES)


#===================================================================================================
#
#
#   $Id: cbsetup.mak,v 1.1 1999/06/15 20:48:03 ktk Exp $
#
#
#   $Log: cbsetup.mak,v $
#   Revision 1.1  1999/06/15 20:48:03  ktk
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
