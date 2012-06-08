#===================================================================================================
#
#   This Makefile makes the entire CandyBarZ project
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


SHIP_FILE   = cbarz1_40_1.zip

ALL: $(CANDY_DLL) $(CBSETUP) $(THEMEMAKER) plugins


#  I have created phony dependencies that have the same name as the target of
#  each individual makefile.  This way one can be in the CANDY home directory
#  and simply type make <target name> if one wishes to make only one target.

.PHONY:	clean $(CANDY_DLL) $(CBSETUP) ship plugins

clean:
	- cd $(CANDY)\o && make -f candydll.mak clean
	- cd $(CANDY)\o && make -f cbsetup.mak clean
	- cd themeBuilder\o && make -f themeBuilder.mak clean

	- cd $(CANDY)\plugins\pluginUtils\o && make clean
	- cd $(CANDY)\plugins\brdrimg\o && make clean
	- cd $(CANDY)\plugins\cbzbrdr\o && make clean
	- cd $(CANDY)\plugins\cbzimage\o && make clean
	- cd $(CANDY)\plugins\cbzlines\o && make clean
	- cd $(CANDY)\plugins\cbztext\o && make clean
	- cd $(CANDY)\plugins\chkimage\o && make clean
	- cd $(CANDY)\plugins\gradient\o && make clean
	- cd $(CANDY)\plugins\minmax\o && make clean
	- cd $(CANDY)\plugins\szeimage\o && make clean
	- cd $(CANDY)\plugins\transimg\o && make clean
	- cd $(CANDY)\plugins\syscolor\o && make clean
	- cd $(CANDY)\plugins\sysbackg\o && make clean

	- cd $(CANDY)\ship && del $(SHIP_FILE) $(CANDY_DLL) $(CB_HELP) $(CBSETUP) $(THEMEMAKER)

plugins:
	cd $(CANDY)\plugins\pluginUtils\o && make
	cd $(CANDY)\plugins\brdrimg\o && make
	cd $(CANDY)\plugins\cbzbrdr\o && make
	cd $(CANDY)\plugins\cbzimage\o && make
	cd $(CANDY)\plugins\cbzlines\o && make
	cd $(CANDY)\plugins\cbztext\o && make
	cd $(CANDY)\plugins\chkimage\o && make
	cd $(CANDY)\plugins\gradient\o && make
	cd $(CANDY)\plugins\minmax\o && make
	cd $(CANDY)\plugins\szeimage\o && make
	cd $(CANDY)\plugins\transimg\o && make
	cd $(CANDY)\plugins\syscolor\o && make 
	cd $(CANDY)\plugins\sysbackg\o && make

$(CANDY_DLL):
	cd $(CANDY)\o && make -f candydll.mak
	

$(CBSETUP):
	cd $(CANDY)\o && make -f cbsetup.mak

$(THEMEMAKER):
	cd themeBuilder\o && make -f themeBuilder.mak

ship:
	copy $(CANDY)\o\$(CANDY_DLL) $(CANDY)\ship
	copy $(CANDY)\o\$(CBSETUP) $(CANDY)\ship
	copy $(CANDY)\o\$(CB_HELP) $(CANDY)\ship
	copy themeBuilder\o\$(THEMEMAKER) $(CANDY)\ship
	- del $(CANDY)\ship\$(SHIP_FILE)
	- mkdir $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\pluginUtils\o\cbzutil.dll $(CANDY)\ship
	copy $(CANDY)\plugins\BRDRIMG\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\CBZBRDR\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\CBZIMAGE\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\CBZLINES\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\CBZTEXT\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\CHKIMAGE\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\GRADIENT\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\MINMAX\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\SZEIMAGE\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\TRANSIMG\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\SYSCOLOR\o\*.dll $(CANDY)\ship\plugins
	copy $(CANDY)\plugins\SYSBACKG\o\*.dll $(CANDY)\ship\plugins

	cd $(CANDY)\ship && lxlite *
	cd $(CANDY)\ship\plugins && lxlite *
	cd $(CANDY)\ship && zip -r $(SHIP_FILE) *

	
#===================================================================================================
#
#
#   $Id: makefile,v 1.2 2000/09/24 20:35:30 cwohlgemuth Exp $
#
#
#   $Log: makefile,v $
#   Revision 1.2  2000/09/24 20:35:30  cwohlgemuth
#   Added SYSCOLOR and SYBACKG plugins.
#
#   Revision 1.1  1999/06/15 20:47:54  ktk
#   Import
#
#   Revision 1.2  1998/10/17 21:39:39  pgarner
#   Added a phony ship dependency to zip up and create a "release" package.
#
#   Revision 1.1  1998/10/17 19:47:51  pgarner
#   Initial revision
#
#
#
#
#===================================================================================================

