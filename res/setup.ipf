
:userdoc.

.*  Used res #s.  Please update when a new resource is added.
.*
.* 1002, 2001-2006, 3001-3014, 4001-4003, 5001, 6001-6006, 7001-7006, 8001
.*
.*------------------------------------------------------------------------------------------------------*.

.*------------------------------------------------------------------------------------------------------*.
.*    Copyright Netlabs, 1998, this code may not be used for any military purpose                                                                           *.
.*------------------------------------------------------------------------------------------------------*.

.*------------------------------------------------------------------------------------------------------*.
.*                                              Main help page                                          *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=1002 name=HLPIDX_SETUP.Welcome to CandyBarZ!
:i1 id=AboutCB.Welcome to CandyBarZ!

:p.CandyBarZ once was a titlebar replacement that provided gradient fill patterns. Now it brings
skin support to OS/2. You may select to gradient fill your frames, use images or transparency. You
may use images or custom rendering for the button controls, replace the frame controls and much more.
There are plenty of
options available, all accessible through the CBSetup program that you're currently using.  These
help pages will guide you through the installation and personalization of CandyBarZ.  Select one of
the following topics for more information.

:sl.
:li.&bul.:link reftype=hd res=2001.Installing CandyBarZ:elink.
:li.&bul.:link reftype=hd res=3001.Setting the default colors:elink.
:li.&bul.:link reftype=hd res=4001.Customizing individual programs:elink.
:li.&bul.:link reftype=hd res=7001.Known problems, limitations, and troubleshooting:elink.
:li.&bul.:link reftype=hd res=7002.Registration information:elink.
:li.&bul.:link reftype=hd res=7003.License and disclaimer:elink.
:li.&bul.:link reftype=hd res=7004.The authors:elink.
:li.&bul.:link reftype=hd res=8001.Future Plans:elink.
:li.&bul.:link reftype=hd res=9001.Version History:elink.
:esl.

:p.Thanks for using CandyBarZ!
:p.
:p.CandyBarZ Version 1.40.1, Copyright 1997-2000, OS/2 Netlabs.  Not to be used for military purposes.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Installation Help                                           *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2001 name=HLPIDX_INSTALL.Installing CandyBarZ
:i1 id=InstallCB.Installing CandyBarZ

:p.:hp7.New Installations:ehp7.

:p.Since you're reading this help screen, we assume that you've unzipped the archive and started the
CBSetup program.  Until CandyBarZ is installed, the CBSetup notebook contains only the install page.
For help with any of the controls on the Installation page, tab to on the control and press F1 or choose
one of the links below.

:sl.
:li.&bul.:link reftype=hd res=2002.Destination directory for CandyBar.dll:elink.
:li.&bul.:link reftype=hd res=2003.Destination directory for CandyBar.ini:elink.
:li.&bul.:link reftype=hd res=2004.Status Window:elink.
:li.&bul.:link reftype=hd res=2005.Install:elink.
:li.&bul.:link reftype=hd res=2006.Uninstall:elink.
:esl.

:note.Whack-A-Mole uses a previous version of PSRT.DLL.  However, CandyBarZ no longer uses PSRT.DLL.
If you DO NOT have Whack-A-Mole installed, but DO have a previous version of CandyBarZ installed,
you may remove the PSRT.DLL.

:note.CandyBar.dll MUST reside in a directory specified in your LIBPATH line in CONFIG.SYS.

:p.:hp7.Upgrade Installations:ehp7.

:p.:hp2.CAUTION!!:ehp2.  The installation process has changed for version 1.20.  :hp2.Do NOT:ehp2.
uninstall before upgrading.  If you uninstall, your CandyBar.INI :hp2.WILL BE DELETED:ehp2.!

:p.To upgrade your current CandyBarZ installation to the latest release, start the CBSetup program and
turn to the "Install/Uninstall" page.  Press the :link reftype=hd res=2005.Install:elink. button.
Reboot to load the new DLLs.

.*------------------------------------------------------------------------------------------------------*.
.*                                          DLL Dropdown                                                *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2002 name=HLPIDX_DLLDEST.Destination directory for CandyBar.dll

:p.This dropdown listbox contains the entries from your LIBPATH line in CONFIG.SYS.  CandyBar.DLL
:hp2.MUST:ehp2. reside in a directory referenced by your LIBPATH.  If you have a previous
version of CandyBarZ installed, CBSetup will attempt to find that installation.  Otherwise, it will
default to <boot drive>\OS2\DLL.

:p.If you would like to use a directory not contained in the list, take the following steps:

:ol.
:li.Exit the CBSetup program
:li.Create the directory
:li.Add the directory to the LIBPATH line in CONFIG.SYS
:li.Restart the CBSetup program
:eol.

:p.After performing these steps, the new directory should be present in the dropdown listbox.

.*------------------------------------------------------------------------------------------------------*.
.*                                          INI Entryfield                                              *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2003 name=HLPIDX_INIDEST.Destination directory for CandyBar.ini

:p.This entryfield allows you to choose the destination directory for CandyBar.ini, the CandyBarZ option
file.  This file is not contained in the archive, so don't be alarmed that you don't see it.  CandyBarZ
will create the profile with default values the first time it is loaded.

:p.By default, CBSetup will place CandyBar.ini in the same directory as the DLLs.  If you wish to change
this, either type in a new destination or press the :hp1.Browse...:ehp1. button.  A file dialog will
appear so that you can choose a destination.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Status Window                                               *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2004 name=HLPIDX_STATUS.Status Window

:p.The Status window allows you to see the progress of the installation/uninstallation process.  If any
errors occur, they will also be displayed there.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Install Button                                              *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2005 name=HLPIDX_INSTBUTTON.Install

:p.The :hp1.Install:ehp1. button does exactly that, begins the installation process.  CandyBar.dll will
be copied to the directory displayed in the :link reftype=hd res=2002.listbox:elink..  The
location chosen for CandyBar.ini (displayed in the :link reftype=hd res=2003.entryfield:elink.) will be
written to the OS2.INI file (application CandyBarZ, key Profile).  Additionally, the :hp1.SYS_DLLS:ehp1.
application in OS2.INI will be modified so that OS/2 loads CandyBar.dll at boot time.

:p.Note that the CBSetup.exe program and this help file (CBSetup.hlp) are not copied anywhere.  You
should place these in a directory of your choice (if not together, then CBSetup.hlp must reside in a
directory listed in your :hp1.SET HELP=:ehp1. line in CONFIG.SYS) and, optionally, create a program
object on your desktop.  CBSetup.exe is your key to total titlebar domination!  Use it wisely.

:p.When the installation is complete, you must reboot.  This will allow OS/2 to load CandyBar.dll.  You
can then start the CBSetup program again and begin :link reftype=hd res=3001.customizing:elink..

.*------------------------------------------------------------------------------------------------------*.
.*                                          Uninstall Button                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=2006 name=HLPIDX_UNINSTALL.Uninstall

:p.Should you have the poor taste to uninstall CandyBarZ, the :hp1.Uninstall:ehp1. will grudgingly allow
you to do so.  Simply press the button.  The uninstallation process will remove the modifications that
the installation made to the OS2.INI file and delete the files CandyBar.DLL, and CandyBar.INI.
CBSetup.exe and CBSetup.hlp must be removed manually.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Default Colors                                              *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3001 name=HLPIDX_PAGE1.Setting the Default Colors
:i1 id=Default.Setting the Default Colors

:p.The :hp1.Default Colors:ehp1. pages allow you to set the color scheme that CandyBarZ uses by default
(that is, for all windows that don't have a :link reftype=hd res=4001.custom color scheme:elink.).  On
these pages, you'll see previews of the active and inactive titlebars, as well as several buttons.
For help with any of the controls on these pages, tab to the control and press F1 or choose one of the
links below.

:sl.
:li.&bul.:link reftype=hd res=3002.Top... (Left):elink.
:li.&bul.:link reftype=hd res=3002.Bottom... (Right):elink.
:li.&bul.:link reftype=hd res=3006.Horizontal:elink.
:li.&bul.:link reftype=hd res=3008.Fill Up (Left):elink.
:li.&bul.:link reftype=hd res=3008.Fill Down (Right):elink.
:li.&bul.:link reftype=hd res=3011.3D Text:elink.
:li.&bul.:link reftype=hd res=3007.Draw Border:elink.
:li.&bul.:link reftype=hd res=3014.The Enable PM View Fix checkbox:elink.
:li.&bul.:link reftype=hd res=3012.The Enable S3 Fix checkbox:elink.
:li.&bul.:link reftype=hd res=3010.Use File...:elink.
:li.&bul.:link reftype=hd res=3010.Browse... (if a file is in use):elink.
:li.&bul.:link reftype=hd res=3003.The Apply and Save buttons:elink.
:li.&bul.:link reftype=hd res=3004.The Undo button:elink.
:li.&bul.:link reftype=hd res=3005.The Enable CandyBarZ checkbox:elink.
:esl.

.*------------------------------------------------------------------------------------------------------*.
.*                                      Top and Bottom Buttons                                          *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3002 name=HLPIDX_TOPBOTTOM.The Top... and Bottom... Buttons

:p.These allow you to :link reftype=hd res=5001.change the color:elink. used for the
topmost and bottommost (respectively, you know) rows of pixels in the titlebar.

.*------------------------------------------------------------------------------------------------------*.
.*                                      Apply and Save Buttons                                          *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3003 name=HLPIDX_APPLYSAVE.The Apply and Save Buttons

:p.The :hp1.Apply:ehp1. button broadcasts the new color set to all open windows.  The effect of this
button lasts only until the next reboot.  The :hp1.Save:ehp1. button broadcasts the change and writes
the changes to the CandyBarZ profile so they will be used until changed again.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Undo Buttons                                                *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3004 name=HLPIDX_UNDODEF.The Undo Button

:p.The :hp1.Undo:ehp1. button reverses any changes made since the previous use of
:hp1.:link reftype=hd res=3003.Apply:elink.:ehp1. and/or :link reftype=hd res=3003.:hp1.Save:elink.:ehp1.
(or since the start of the CBSetup program).

.*------------------------------------------------------------------------------------------------------*.
.*                                          Enable Checkbox                                             *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3005 name=HLPIDX_ENABLECBDEF.The Enable CandyBarZ Checkbox

:p.The :hp1.Enable CandyBarZ:ehp1. checkbox allows you to disable CandyBarZ altogether.  Though not
strictly necessary, we made a conscious decision to require a reboot upon disabling or enabling
CandyBarZ.

.*------------------------------------------------------------------------------------------------------*.
.*                                      Horizontal Checkbox                                             *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3006 name=HLPIDX_HORIZONTAL.The Horizontal Checkbox

:p.The :hp1.Horizontal:ehp1. checkbox instructs CandyBarZ to use a horizontal, rather than the default
vertical, gradient "like Win 95 does it."

.*------------------------------------------------------------------------------------------------------*.
.*                                      Draw Border Checkbox                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3007 name=HLPIDX_DRAWBORDER.The Draw Border Checkbox

:p.The :hp1.Draw Border:ehp1. checkbox instructs CandyBarZ to draw a 3D border highlight, ala Warp 4.
we haven't seen the effect of this on versions of OS/2 prior to Warp 4.  Note that the preview window
will not show this border.

.*------------------------------------------------------------------------------------------------------*.
.*                                          The Fill Buttons                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3008 name=HLPIDX_FILLBUTTONS.The Fill Buttons

:p.:hp1.Fill Up:ehp1. and :hp1.Fill Down:ehp1. cause CBSetup to choose one endpoint of the gradient
itself.  If two of three colors are less than half their maximum value, then all of the colors are
doubled in brightness across the gradient.  Similarly, if two of the three color components are greater
than half their maximum value, all of the components are halved in strength across the gradient.  For
example, if the top color is red=200, green=60, and blue=240, pressing the "Fill Down" button will cause
the bottom color to become red=100, green=30, and blue=120.  Simple, no?

:p.This scheme may not provide a steep enough gradient.  Lemme know at :hp1.jack.troughton@videotron.ca:ehp1..

.*------------------------------------------------------------------------------------------------------*.
.*                                          CandyBarZ Button                                            *.
.*------------------------------------------------------------------------------------------------------*.
.*
.*:h1 res=3009 name=HLPIDX_MENU.The CandyBarZ Button
.*
.*:p.Pressing the :hp1.CandyBarZ:ehp1. button causes a menu of options to appear.  This menu contains the
.*following choices:
.*:sl.
.*:li.&bul.:link reftype=hd res=3002.Top...:elink.
.*:li.&bul.:link reftype=hd res=3002.Bottom...:elink.
.*:li.&bul.:link reftype=hd res=3008.Fill Down (Right):elink.
.*:li.&bul.:link reftype=hd res=3008.Fill Up (Left):elink.
.*:li.&bul.:link reftype=hd res=3010.Use File...:elink.
.*:li.&bul.:link reftype=hd res=3010.Change File (if a file is in use):elink.
.*:li.&bul.:link reftype=hd res=3006.Horizontal:elink.
.*:li.&bul.:link reftype=hd res=3007.Draw Border:elink.
.*:esl.
.*

.*------------------------------------------------------------------------------------------------------*.
.*                                              Files                                                   *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3010 name=HLPIDX_FILES.CandyBarZ File Support

:p.:hp1.Use File:ehp1. allows you to choose an image to display in your titlebars.  To use a file, press
"Use File" and choose the desired image from the subsequent file dialog.  To choose a different image,
press the :hp1.Browse...:ehp1. button.

:p.Any image format supported by OS/2 is supported by CandyBarZ.  For Warp 4, this means that
you can use bitmap, JPG, GIF, TIFF, Targa, and more.  Even better, as support for new file formats is
added to OS/2, CandyBarZ will automatically support them as well.

.*------------------------------------------------------------------------------------------------------*.
.*                                             3D Text                                                  *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3011 name=HLPIDX_3D.3D Text
:p.The :hp1.3D Text:ehp1. option causes CandyBarZ to draw a text shadow slightly offset from the titlebar
text, giving the text a 3D appearance.  Pressing the color button displays a dialog which allows you to
choose the shadow color.  Note that the preview window, since it does not show text, will not show this
effect.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Enable S3 Fix                                               *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3012 name=HLPIDX_S3.The Enable S3 Fix checkbox
:p.The :hp1.Enable S3 Fix:ehp1. checkbox is a workaround for some S3 (and other) cards which fail to
properly display horizontal gradients.  Use this option only if horizontal gradients do not work on your
system, as there is some additional CPU overhead.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Active/Inactive                                             *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3013 name=HLPIDX_ACTIVEINACTIVE.The Active/Inactive radio buttons
:p.The :hp1.Active/Inactive Titlebar Options:ehp1. radio buttons let you switch between editing the
active and inactive settings for your titlebars.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Enable PM View Fix                                          *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=3014 name=HLPIDX_PMVIEW.The Enable PM View Fix checkbox
:p.the :hp1.Enable PM View Fix:ehp1. checkbox enables gradient colors for the program PM View.  Because
this workaround interferes with Object Desktop's titlebar buttons, it is recommended that this feature
be disabled when using Object Desktop.  This option can be enabled with PM View running but disabling
it requires that PM View be restarted for the change to take effect.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Custom Colors                                               *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=4001 name=HLPIDX_PAGE2.Using Custom Color Schemes
:i1 id=CustPage.Using Custom Color Schemes

:p.The :hp1.Custom Colors:ehp1. page allows you to customize the titlebars for individual programs.  The
customization is based upon the :hp2.executable name without extension:ehp2..  If you don't follow this
rule, custom colors :hp2.just won't work:ehp2..  The listbox lists (go figure) all of the existing custom
configurations.  For help with any of the controls on this page, tab to the control and press F1 or
choose one of the links below.

:sl.
:li.&bul.:link reftype=hd res=4002.The Add... and Edit... buttons:elink.
:li.&bul.:link reftype=hd res=4003.The Remove button:elink.
:esl.

.*------------------------------------------------------------------------------------------------------*.
.*                                      Add and Edit Buttons                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=4002 name=HLPIDX_ADDEDIT.The Add... and Edit... Buttons

:p.These buttons allow you to :link reftype=hd res=6001.create new color schemes or edit existing
ones:elink..  You may also edit an existing scheme by double-clicking its entry in the list.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Remove Button                                               *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=4003 name=HLPIDX_REMOVE.The Remove Button

:p.This button removes the currently selected scheme from the listbox and broadcasts the change to the
system.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Color change dialog                                         *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=5001 name=HLPIDX_CLRCHANGE.Using the Color Change Dialog
:i1 id=ClrChange.Using the Color Change Dialog

:p.The color change dialog is straightforward.  Three sliders allow you to adjust each of the Red, Green,
and Blue components of your color.  Spinbuttons next to the sliders allow somewhat finer control.  A
preview window allows you to see the current color.  Pressing the :hp1.Ok:ehp1. button accepts the color,
and pressing the :hp1.Cancel:ehp1. button returns you to the previous dialog without changing the color.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Customize Dialog                                            *.
.*------------------------------------------------------------------------------------------------------*.
:h1 res=6001 name=HLPIDX_CUSTOMIZE.The Customization Dialog
:i1 id=CustDlg.The Customization Dialog

:p.This dialog is very similar to the :hp1.:link reftype=hd res=3001.Default Colors:elink.:ehp1. page.
For help with any of the controls on this page, tab to the control and press F1 or choose one of the
links below.

:sl.
:li.&bul.:link reftype=hd res=6002.Title:elink.
:li.&bul.:link reftype=hd res=3002.Top... and Bottom...:elink.
:li.&bul.:link reftype=hd res=3006.Horizontal:elink.
:li.&bul.:link reftype=hd res=3008.The Fill buttons:elink.
:li.&bul.:link reftype=hd res=3011.3D Text:elink.
:li.&bul.:link reftype=hd res=3007.Draw Border:elink.
:li.&bul.:link reftype=hd res=3010.Use File...:elink.
:li.&bul.:link reftype=hd res=3010.Browse... (if a file is in use):elink.
:li.&bul.:link reftype=hd res=3013.Active/Inactive Titlebar Options:elink.
:li.&bul.:link reftype=hd res=6004.Enable CandyBarZ:elink.
:li.&bul.:link reftype=hd res=6004.Enable Custom Colors:elink.
:li.&bul.:link reftype=hd res=6003.Undo:elink.
:li.&bul.:link reftype=hd res=6005.Save and Cancel:elink.
:esl.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Title Entryfield                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=6002 name=HLPIDX_CUSTTITLE.The Title Entryfield

:p.This field lets you enter (or change) the name of the executable to customize.  :hp2.Do not:ehp2.
included the .exe extension.  Entering PMSHELL will give the custom scheme to folders, VIO windows, and
all other windows created by the system.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Undo buttons                                                *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=6003 name=HLPIDX_CUSTUNDO.The Undo Buttons

:p.These buttons allow you to undo any changes made since the dialog was invoked.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Checkboxes                                                  *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=6004 name=HLPIDX_CHKBOXES.The Checkboxes

:p.The :hp1.Enable CandyBarZ:ehp1. checkbox allows you to disable CandyBarZ on a per-executable basis.
Hopefully, this will resolve any problems you may experience with CandyBarZ and certain programs.  The
:hp1.Enable Custom Colors:ehp1. checkbox allows you to use the :hp1.:link reftype=hd res=3001.Default
Colors:elink.:ehp1. without completely removing the custom scheme.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Save and Cancel                                             *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=6005 name=HLPIDX_SAVECANCEL.The Save and Cancel Buttons

:p.The :hp1.Save:ehp1. button writes the custom configuration to the CandyBarZ profile and broadcasts the
change to the system.  The :hp1.Cancel:ehp1. button dismisses the dialog without making any changes.


.*------------------------------------------------------------------------------------------------------*.
.*                                          CandyBarZ Button                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=6006 name=HLPIDX_CUSTMENU.The CandyBarZ Button

:p.Pressing the :hp1.CandyBarZ:ehp1. button causes a menu of options to appear.  This menu contains the
following choices:

:sl.
:li.&bul.:link reftype=hd res=3002.Top... (Left):elink.
:li.&bul.:link reftype=hd res=3002.Bottom... (Right):elink.
:li.&bul.:link reftype=hd res=3008.Fill Down (Right):elink.
:li.&bul.:link reftype=hd res=3008.Fill Up (Left):elink.
:li.&bul.:link reftype=hd res=3010.Use File...:elink.
:li.&bul.:link reftype=hd res=3010.Browse... (if a file is in use):elink.
:li.&bul.:link reftype=hd res=3006.Horizontal:elink.
:li.&bul.:link reftype=hd res=3007.Draw Border:elink.
:li.&bul.:link reftype=hd res=3011.3D Text:elink.
:esl.

.*------------------------------------------------------------------------------------------------------*.
.*                                              Problems                                                *.
.*------------------------------------------------------------------------------------------------------*.
:h1 res=7001.Known Problems, Limitations, and Troubleshooting
:i1 id=Problems.Known Problems, Limitations, and Troubleshooting

:sl.
:li.&bul.When a change is applied or saved, the CBSetup program broadcasts the message to all windows in
the system.  On Warp 3, this could cause the message queue to become filled.  The only result seems to be
a failing return code, which prompts CBSetup to display a message box to that effect.  The changes do take
effect.  We have not yet experienced this problem on Warp 4.

:li.&bul.Hitting escape on FeelX titlebar popup menus causes the titlebar to draw in inactive colors.  It
seems that, when we get my redraw message, the menu still has the focus, so we consider the subject
window inactive.

:li.&bul.Folders do not display their view in the titlebar.  This text is, apparently, not returned when
we ask for the titlebar text.

:li.&bul.PMINews LEDs do not redraw when the window is resized horizontally.  We're not sure if this is
our problem or their problem, but we will contact Stardock to see if this can fixed.

:li.&bul.WordPerfect 6.2 for DOS requires that VIDEO_SWITCH_NOTIFICATION be set to ON.  Thanks to
Ron VanAbrahams for this tip.

:li.&bul.Some programs may defy us - i.e., CandyBarZ has no effect on them.  If you experience this,
please email us at :hp1.jack.troughton@videotron.ca:ehp1., and we will attempt to contact the author of the
program.

:li.&bul.A number of other problems have been reported.  We fixed many bugs that could cause system-wide
problems, and we have tested with a number of programs reported as being problematic.  However, it is not
possible for us to test CandyBarZ under every possible configuration.  If you previously reported a
problem, and this problem still exists using CandyBarZ 1.20 (and is not listed here), please let us know.

:esl.

:p.If you are experiencing a problem with CandyBarZ that is not listed here, please contact us at
:hp1.jack.troughton@videotron.ca:ehp1..  If the problem occurs only with some programs, you may also attempt
to :link reftype=hd res=7006.disable CandyBarZ:elink. when using that program.

.*------------------------------------------------------------------------------------------------------*.
.*                                              Registration                                            *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=7002.Registration Information
:i1 id=RegInfo.Registration Information
:p.Formerly shareware, CandyBarZ has been released to OS/2 Netlabs and, as a consequence, is now free.
:p.Enjoy!

.*------------------------------------------------------------------------------------------------------*.
.*                                              License                                                 *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=7003.License and Disclaimer
:i1 id=License.License and Disclaimer

:p.CandyBarZ Version 1.40.1, Copyright 1997-2000, OS/2 Netlabs

:p.We hope you will not use this software for any military purposes. But this is not enforced.

:p.CandyBarZ is not warranted to be free of bugs.  The authors and OS/2 Netlabs may not be held
responsible for data corruption, loss of productivity, or any other bad thing that results from
the use of this software.
:p.See file COPYING for further information.

.*------------------------------------------------------------------------------------------------------*.
.*                                                  Us                                                  *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=7004.The Authors
:i1 id=Author.The Authors

:p.:hp2.Eric Norman:ehp2. - I am a Senior Computer Science Major at Montana State University.
I'm looking forward to graduating with a degree in the Spring of 1999.

:p.You may contact me via email at :hp1.warped@cs.montana.edu:ehp1.

:p.I would like to thank Brenda Sondereger, John Paxton, and all the great Professors at MSU who have
given me a solid backround in many aspects of computer programming.  I would also like to acknowledge
Jay Dolan, Don Burkenpas and Brian Weiss for all their coding tips and Mountain Dew.


:p.:hp2.Matt Wagner:ehp2. - I received my BS in Mathematics from MIT in 1993, and my MS in Applied
Mathematics from the University of Arizona in 1995.  I am a contract C, C++, and SQR programmer on
Windows NT (hey, it's hard to find an OS/2 job in Atlanta).  I do not write comic books.

:p.You may contact me via email at :hp1.erlkonig@alum.mit.edu:ehp1.

:p.I would like to acknowledge the support of Michael Compton and Nick Lemke
at DocuCorp International, Inc. for their willingness to test the first versions of CandyBarZ.  Thanks
also go to IBM for producing OS/2 and its development tools (CandyBarZ was written in C and
built with VisualAge for C++), despite their apparent lack of OS/2 support,
and all of the OS/2 developers who have set for me a high standard of
excellence.

:p.:hp2.Chris Wohlgemuth:ehp2.

:p.:hp2.Peter Garner:ehp2.

:p.:hp2.Jack Troughton:ehp2.

.*------------------------------------------------------------------------------------------------------*.
.*                                          Disabling CandyBarZ                                         *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=7006.Selectively Disabling CandyBarZ
:i1 id=Disable.Selectively Disabling CandyBarZ

:p.CandyBarZ may be selectively disabled on a per executable basis.  To do so
:sl.
:li.&bul.Turn to the :link reftype=hd res=4001.Custom Colors:elink. page and press the
:link reftype=hd res=4002."Add...":elink. button.
:li.&bul.When the dialog appears, type the name of the executable (:hp2.without extension:ehp2.) in the
:link reftype=hd res=6002.entryfield:elink. and uncheck the
:link reftype=hd res=6004.Enable CandyBarZ checkbox:elink.
:li.&bul.Press the :link reftype=hd res=6005.Save:elink. button.
:esl.

.*------------------------------------------------------------------------------------------------------*.
.*                                              Future                                                  *.
.*------------------------------------------------------------------------------------------------------*.

:h1 res=8001.Future Plans
:i1.Future Plans

:p.Amazing what can be done with such a simple concept.  Here's what we hope to do.

:sl.
:li.&bul.Fix more bugs.  There are a couple of outstanding issues (see
:link reftype=hd res=7001.Known Problems:elink.) that won't cause problems but are cosmetic flaws.

:li.&bul.WPS integration is in the works.

:li.&bul.New gradient options.  For example, bilinear (fade from center outward) gradients are in
progress.

:li.&bul.Gradient text.  The capability is there now, but we haven't added the code to enable
it yet.  Gotta save something for later....

:li.&bul.Animations?  Depends on how resource economically we can do it, but we've
been toying with the idea.

:li.&bul.Additional windows.  Scrollbars, frames, buttons....

:li.&bul.Customizations based on window title.  This was the original intent, but
many windows apparently don't have a title when their titlebars are
created.  Even if we can't work around this, we may implement it as a hit
and miss type of thing.

:li.&bul.Preset schemes.  We'll try to come up with some nice combinations to save you
the hassle.

:li.&bul.Work on focus issues.  Determining when the active colors vs. inactive
colors should be used is pretty twisted.  We suspect that we've missed a
few criteria somewhere.  It's NOT simply an issue of "if a child of the
frame has the focus, then use active."  For instance, if a popup menu
(which is owned by the frame, but is not a child of the frame) has the
focus, then the titlebar should be colored active.  However, if a
message box has the focus (which is also owned by, but is not a child
of, the frame), then the titlebar should be colored inactive.

:li.&bul.Internationalization.

:esl.

:p.:hp2.If there is a feature you would like to see, let us know.:ehp2.

:h1 res=9001.Version History
:i1.Version History

:table cols='19 5 15' rules=none frame=none.
:row.
:c.24 July, 1997 :c.1.00 :c.Initial release
:row.
:c.02 August, 1997 :c.1.01 :c.Fix lots of bugs, probably introduce some others
:row.
:c.02 September, 1997 :c.1.10 :c.Add horizontal gradients, fill option, 3D border highlight, clean up
CBSetup some, fix bugs, some optimizations
:row.
:c.Put Date Here :c.1.20 :c.Fix bugs, of course.  Add 3D text, redesign CBSetup, add image support, add
PMView and S3 workarounds
:etable.

:euserdoc.



.*============================================================================================================


    $Id: setup.ipf,v 1.2 2000/09/24 21:20:46 cwohlgemuth Exp $


    $Log: setup.ipf,v $
    Revision 1.2  2000/09/24 21:20:46  cwohlgemuth
    Updating.

    Revision 1.1  1999/06/15 20:48:22  ktk
    Import

    Revision 1.13  1998/10/31 03:00:43  pgarner
    Fixed type in changing my name to jacks

    Revision 1.11  1998/10/17 20:35:35  pgarner
    Changed the help text to reflect that PSRT.DLL is no longer used

    Revision 1.10  1998/09/30 05:09:30  mwagner
    Made the PMView fix optional

    Revision 1.8  1998/09/26 02:05:50  enorman
    Added my about the author blurb!

    Revision 1.7  1998/09/24 19:24:43  pgarner
    Added my about the author blurb!  :-)

    Revision 1.6  1998/09/22 12:05:47  mwagner
    Updated help to reflect new beta features

    Revision 1.4  1998/09/11 03:02:29  pgarner
    Add the non-military use license

    Revision 1.3  1998/09/11 02:45:43  pgarner
    'Added

    Revision 1.2  1998/06/14 12:06:21  mwagner
    Added the documentation that WordPerfect 6.2 for DOS requires that VIDEO_SWITCH_NOTIFICATION be set to ON.

    Revision 1.1  1998/06/08 14:18:58  pgarner
    Initial Checkin



============================================================================================================*.

