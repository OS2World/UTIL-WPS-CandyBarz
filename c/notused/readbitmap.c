/* $Id: readbitmap.c,v 1.1 1999/06/15 20:48:00 ktk Exp $ */


#include "setup.h"

/******** Loads a bitmap-file into memory ***********************************/
/*                                                                          */
/*    C.W. 20.09.98                                                         */
/*                                                                          */
/****************************************************************************/
HBITMAP ReadBitmap(PSZ pszFileName,HPS hpsBitmapFile)
{
	APIRET     rc;                                         /* API return code */
	BOOL       fRet = FALSE;                         /* Function return code. */
	FILESTATUS fsts;

	ULONG  cbRead;                        /* Number of bytes read by DosRead. */
	HFILE hfile;
	ULONG ulResult;
	HPS hps;
	HBITMAP hbm=NULLHANDLE;
	PBYTE pFileBegin;
	BITMAPINFOHEADER2 bmp2FrameBitmapFile;
  PBITMAPFILEHEADER2 pbfh2;                   /* can address any file types */
  PBITMAPINFOHEADER2 pbmp2;                     /* address any info headers */
	
	/*
	 *   Use Loop to avoid duplicate cleanup code.  If any errors, a break
	 *   statement will jump directly to error cleanup code.
	 */
	
	pFileBegin=NULLHANDLE;
	do
		{
			if(DosOpen(pszFileName, &hfile, &ulResult, 0UL, FILE_NORMAL, OPEN_ACTION_OPEN_IF_EXISTS | OPEN_ACTION_FAIL_IF_NEW,
								 OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL))
				break;
			
			rc =	DosQueryFileInfo(hfile, 1, &fsts, sizeof(fsts));
      if (rc)
				break;                        /* jump to error code outside of loop */			
      rc =  DosAllocMem(
												(PPVOID) &pFileBegin,
												(ULONG)  fsts.cbFile,
												(ULONG) PAG_READ | PAG_WRITE | PAG_COMMIT);
			if (rc)
				break;                        /* jump to error code outside of loop */
			if (DosRead( hfile, (PVOID)pFileBegin, fsts.cbFile, &cbRead)) {
				DosFreeMem(pFileBegin);
				break;                        /* jump to error code outside of loop */				
			}
			
			/*
       *   If it's a bitmap-array, point to common file header.  Otherwise,
       *   point to beginning of file.
       */
      pbfh2 = (PBITMAPFILEHEADER2) pFileBegin;
      pbmp2 = NULL;                   /* only set this when we validate type */		
      switch (pbfh2->usType)
				{
				case BFT_BITMAPARRAY:
					/*
					 *   If this is a Bitmap-Array, adjust pointer to the normal
					 *   file header.  We'll just use the first bitmap in the
					 *   array and ignore other device forms.
					 */
					pbfh2 = &(((PBITMAPARRAYFILEHEADER2) pFileBegin)->bfh2);
					pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
					break;
				case BFT_BMAP:
					pbmp2 = &pbfh2->bmp2;    /* pointer to info header (readability) */
					break;
				default:      /* these formats aren't supported; don't set any ptrs */
				case BFT_ICON:
				case BFT_POINTER:
				case BFT_COLORICON:
				case BFT_COLORPOINTER:
					break;
				}   /* end switch (pbfh2->usType) */
      if (pbmp2 == NULL)
				break;        /* File format NOT SUPPORTED: break out to error code */
      /*
       *   Check to see if BMP file has an old structure, a new structure, or
       *   Windows structure.  Capture the common data and treat all bitmaps
       *   generically with pointer to new format.  API's will determine format
       *   using cbFixed field.
       *
       *   Windows bitmaps have the new format, but with less data fields
       *   than PM.  The old strucuture has some different size fields,
       *   though the field names are the same.
       *
       *
       *   NOTE: bitmap data is located by offsetting the beginning of the file
       *         by the offset contained in pbfh2->offBits.  This value is in
       *         the same relatie location for different format bitmap files.
       */

		
      memcpy(                      /* copy bitmap info into global structure */
						 (PVOID) &bmp2FrameBitmapFile,
						 (PVOID) pbmp2,
						 pbmp2->cbFix);      /* only copy specified size (varies per format) */
					
			hbm = GpiCreateBitmap(
														hpsBitmapFile,                         /* presentation-space handle */
														&bmp2FrameBitmapFile,            /* address of structure for format data */
														CBM_INIT,                                                      /* options */
														(PBYTE)pbfh2 + pbfh2->offBits,                            /* address of buffer of image data */
														(PBITMAPINFO2)(PVOID)&pbfh2->bmp2);                 /* address of structure for color and format */
						
 

			if (!hbm)
				{
					break;                        /* jump to error code outside of loop */
				}

      DosClose( hfile);
			DosFreeMem(pFileBegin);
			
      return hbm;                                    /* function successful */
		} while (FALSE); /* fall through loop first time */
	
	/*
	 *   Close the file, free the buffer space and leave.  This is an error exit
	 *   point from the function.  Cleanup code is here to avoid duplicate code
	 *   after each operation.
	 */
	if (pFileBegin != NULL)
		DosFreeMem( pFileBegin);
	if(hfile)	DosClose( hfile);
	
	return 0;                                         /* function failed */
}   /* end ReadBitmap() */
