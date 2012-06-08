/* $Id: colorchangedlgproc.c,v 1.2 2000/04/15 15:06:16 cwohlgemuth Exp $ */

#include "pluginUtils.h"

// for changing colors
/*MRESULT EXPENTRY ColorChangeDlgProc(HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
  {
  ULONG *plRgb;
  HWND hwndControl;
  ULONG lColor;
  CLRCHANGE *pClrCh;
  ULONG ulRgb;
  
  switch (msg)
  {
  case WM_INITDLG:
  {
  // where old color is passed in and new color will be stored
  if ((pClrCh = (CLRCHANGE *) mp2) == NULL)
  {
  WinDismissDlg(hwnd, DID_ERROR);
  }
  
  WinSetWindowPtr(hwnd, QWL_USER, &(pClrCh->ulRgb));
  ulRgb = pClrCh->ulRgb;
  
  // set controls based on color passed in
  hwndControl = WinWindowFromID(hwnd, SLID_RED);
  WinSendMsg(hwndControl,
  SLM_SETSLIDERINFO,
  MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
  MPFROMSHORT(((ulRgb) & 0xFF0000) >> 16));
  hwndControl = WinWindowFromID(hwnd, SPID_RED);
  WinSendMsg(hwndControl, SPBM_SETLIMITS, (MPARAM) 255, (MPARAM) 0);
  WinSendMsg(hwndControl,
  SPBM_SETCURRENTVALUE,
  MPFROMSHORT(((ulRgb) & 0xFF0000) >> 16),
  (MPARAM) 0);
  hwndControl = WinWindowFromID(hwnd, SLID_GREEN);
  WinSendMsg(hwndControl,
  SLM_SETSLIDERINFO,
  MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
  MPFROMSHORT(((ulRgb) & 0x00FF00) >> 8));
  hwndControl = WinWindowFromID(hwnd, SPID_GREEN);
  WinSendMsg(hwndControl, SPBM_SETLIMITS, (MPARAM) 255, (MPARAM) 0);
  WinSendMsg(hwndControl,
  SPBM_SETCURRENTVALUE,
  MPFROMSHORT(((ulRgb) & 0x00FF00) >> 8),
  (MPARAM) 0);
  hwndControl = WinWindowFromID(hwnd, SLID_BLUE);
  WinSendMsg(hwndControl,
  SLM_SETSLIDERINFO,
  MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
  MPFROMSHORT((ulRgb) & 0xFF));
  hwndControl = WinWindowFromID(hwnd, SPID_BLUE);
  WinSendMsg(hwndControl, SPBM_SETLIMITS, (MPARAM) 255, (MPARAM) 0);
  WinSendMsg(hwndControl,
  SPBM_SETCURRENTVALUE,
  MPFROMSHORT(((ulRgb) & 0xFF)),
  (MPARAM) 0);
  // set preview
  hwndControl = WinWindowFromID(hwnd, VSID_COLORPREVIEW);
  WinSendMsg(hwndControl,
  VM_SETITEMATTR,
  MPFROM2SHORT(1, 1),
  MPFROM2SHORT(VIA_DROPONABLE, TRUE));
  WinSendMsg(hwndControl, VM_SETITEM, MPFROM2SHORT(1, 1), MPFROMLONG(ulRgb));
  }
  break;
  
  case WM_CONTROL:
  {
  if ((plRgb = WinQueryWindowPtr(hwnd, QWL_USER)) == NULL)
  {
  WinDismissDlg(hwnd, DID_ERROR);
  }

  switch (SHORT1FROMMP(mp1))
  {
  case SLID_RED:     // slider changed
  
  case SLID_GREEN:
  case SLID_BLUE:
  {
  // get component value
  hwndControl = WinWindowFromID(hwnd, SHORT1FROMMP(mp1));
  lColor = (ULONG) WinSendMsg(hwndControl,
  SLM_QUERYSLIDERINFO,
  MPFROM2SHORT(SMA_SLIDERARMPOSITION,
  SMA_INCREMENTVALUE),
  (MPARAM) 0L);
  
  switch (SHORT1FROMMP(mp1))
  {
  case SLID_RED:
  {
  // update color
  *plRgb = (lColor * 65536) | ((*plRgb) & 0x00FFFF);
  hwndControl = WinWindowFromID(hwnd, SPID_RED);
  }
  break;
  
  case SLID_GREEN:
  {
  // update color
  *plRgb = (lColor * 256) | ((*plRgb) & 0xFF00FF);
  hwndControl = WinWindowFromID(hwnd, SPID_GREEN);
  }
  break;
  
  case SLID_BLUE:
  {
  // update color
  *plRgb = (lColor) | ((*plRgb) & 0xFFFF00);
  hwndControl = WinWindowFromID(hwnd, SPID_BLUE);
  }
  break;
  }
  // update spinbutton
  WinSendMsg(hwndControl, SPBM_SETCURRENTVALUE, MPFROMLONG(lColor), (MPARAM) 0);
  // update preview
  hwndControl = WinWindowFromID(hwnd, VSID_COLORPREVIEW);
  WinSendMsg(hwndControl, VM_SETITEM, MPFROM2SHORT(1, 1), MPFROMLONG(*plRgb));
  }
  break;
  
  case SPID_RED:     // spinbutton changed
  
  case SPID_GREEN:
  case SPID_BLUE:
  {
  // get new value
  hwndControl = WinWindowFromID(hwnd, SHORT1FROMMP(mp1));
  WinSendMsg(hwndControl,
  SPBM_QUERYVALUE,
  MPFROMP(&lColor),
  MPFROM2SHORT(0, SPBQ_UPDATEIFVALID));
  if (lColor > 255)  // make sure it's in bounds
  
  {
  lColor = 255;
  }
  switch (SHORT1FROMMP(mp1))
  {
  case SPID_RED:
  {
  // update color
  *plRgb = (lColor * 65536) | ((*plRgb) & 0x00FFFF);
  hwndControl = WinWindowFromID(hwnd, SLID_RED);
  }
  break;
  
  case SPID_GREEN:
  {
  // update color
  *plRgb = (lColor * 256) | ((*plRgb) & 0xFF00FF);
  hwndControl = WinWindowFromID(hwnd, SLID_GREEN);
  }
  break;
  
  case SPID_BLUE:
  {
  // update color
  *plRgb = (lColor) | ((*plRgb) & 0xFFFF00);
  hwndControl = WinWindowFromID(hwnd, SLID_BLUE);
  }
  break;
  }
  // update corresponding slider
  WinSendMsg(hwndControl,
  SLM_SETSLIDERINFO,
  MPFROM2SHORT(SMA_SLIDERARMPOSITION, SMA_INCREMENTVALUE),
  MPFROMLONG(lColor));
  // update preview
  hwndControl = WinWindowFromID(hwnd, VSID_COLORPREVIEW);
  WinSendMsg(hwndControl, VM_SETITEM, MPFROM2SHORT(1, 1), MPFROMLONG(*plRgb));
  }
  break;
  }
  }
  break;
  
  case WM_COMMAND:
  {
  switch (COMMANDMSG(&msg)->cmd)
  {
  case PBID_COLOROK:
  {
  WinDismissDlg(hwnd, DID_OK);
  }
  break;
  
  case PBID_COLORCANCEL:
  {
  WinDismissDlg(hwnd, DID_CANCEL);
  }
  }
  }
  break;
  
  return (WinDefDlgProc(hwnd, msg, mp1, mp2));
  }*/







