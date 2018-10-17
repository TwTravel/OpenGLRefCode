/********************************************************************

	pct.h

	----------------------------------------------------------------
    Èí¼þÐí¿ÉÖ¤ £­ GPL
	°æÈ¨ËùÓÐ (C) 2002 VCHelp coPathway ISee workgroup.
	----------------------------------------------------------------
	ÕâÒ»³ÌÐòÊÇ×ÔÓÉÈí¼þ£¬Äã¿ÉÒÔ×ñÕÕ×ÔÓÉÈí¼þ»ù½ð»á³ö°æµÄGNU Í¨ÓÃ¹«¹²Ðí
	¿ÉÖ¤Ìõ¿îÀ´ÐÞ¸ÄºÍÖØÐÂ·¢²¼ÕâÒ»³ÌÐò¡£»òÕßÓÃÐí¿ÉÖ¤µÄµÚ¶þ°æ£¬»òÕß£¨¸ù
	¾ÝÄãµÄÑ¡Ôñ£©ÓÃÈÎºÎ¸üÐÂµÄ°æ±¾¡£

    ·¢²¼ÕâÒ»³ÌÐòµÄÄ¿µÄÊÇÏ£ÍûËüÓÐÓÃ£¬µ«Ã»ÓÐÈÎºÎµ£±£¡£ÉõÖÁÃ»ÓÐÊÊºÏÌØ¶¨
	Ä¿µØµÄÒþº¬µÄµ£±£¡£¸üÏêÏ¸µÄÇé¿öÇë²ÎÔÄGNUÍ¨ÓÃ¹«¹²Ðí¿ÉÖ¤¡£

    ÄãÓ¦¸ÃÒÑ¾­ºÍ³ÌÐòÒ»ÆðÊÕµ½Ò»·ÝGNUÍ¨ÓÃ¹«¹²Ðí¿ÉÖ¤µÄ¸±±¾£¨±¾Ä¿Â¼
	GPL.txtÎÄ¼þ£©¡£Èç¹û»¹Ã»ÓÐ£¬Ð´ÐÅ¸ø£º
    The Free Software Foundation, Inc.,  675  Mass Ave,  Cambridge,
    MA02139,  USA
	----------------------------------------------------------------
	Èç¹ûÄãÔÚÊ¹ÓÃ±¾Èí¼þÊ±ÓÐÊ²Ã´ÎÊÌâ»ò½¨Òé£¬ÇëÓÃÒÔÏÂµØÖ·ÓëÎÒÃÇÈ¡µÃÁªÏµ£º

			http://isee.126.com
			http://cosoft.org.cn/projects/iseeexplorer

	»ò·¢ÐÅµ½£º

			yzfree##sina.com
	----------------------------------------------------------------
	±¾ÎÄ¼þÓÃÍ¾£º	ISeeÍ¼Ïñä¯ÀÀÆ÷¡ªApple PICTÍ¼Ïñ¶ÁÐ´Ä£¿é¶¨ÒåÎÄ¼þ
	
					¶ÁÈ¡¹¦ÄÜ£º¿É¶ÁÈ¡8¡¢32Î»PCTÍ¼Ïó£¬µ«ÆäÖÐµÄÊ¸Á¿³É·Ý½«±»ºöÂÔ
							  
					±£´æ¹¦ÄÜ£ºÔÝ²»Ö§³Ö
	  
	±¾ÎÄ¼þ±àÐ´ÈË£º	
					YZ			yzfree##yeah.net
		
	±¾ÎÄ¼þ°æ±¾£º	20516
	×îºóÐÞ¸ÄÓÚ£º	2002-5-16
		  
	×¢£ºÒÔÉÏE-MailµØÖ·ÖÐµÄ##ÇëÓÃ@Ìæ»»£¬ÕâÑù×öÊÇÎªÁËµÖÖÆ¶ñÒâµÄE-Mail
	µØÖ·ÊÕ¼¯Èí¼þ¡£
	----------------------------------------------------------------
	ÐÞÕýÀúÊ·£º

		2002-5		µÚÒ»´Î·¢²¼£¨ÐÂ°æ£©

********************************************************************/



#ifndef __PCT_MODULE_INC__
#define __PCT_MODULE_INC__

#ifndef __ISEEIO_INC__
#include "iseeio.h"						/* Êý¾Ý´æ´¢Éè±¸½Ó¿Ú¶¨ÒåÎÄ¼þ */
#endif


/* ¶¨ÒåÄ£¿é°æ±¾¡¢Ãû³Æ¡¢º¯ÊýÇ°×º¡¢Ä¬ÈÏµÄÎÄ¼þÃûºó×º */
#define	MODULE_BUILDID					20
#define MODULE_NAME						("PCT²å¼þ")
#define MODULE_FUNC_PREFIX				("pct_")
#define MODULE_FILE_POSTFIX				("pct")

/* ÎÄ¼þÅÐ±ð±êÖ¾ */


#ifdef WIN32	/* Windows */

#	ifdef  PCT_EXPORTS
#	define PCT_API __declspec(dllexport)
#	else
#	define PCT_API __declspec(dllimport)
#	endif
#	define CALLAGREEMENT	__cdecl

#	ifndef _DEUBG
#	define NDEBUG
#	endif

#else			/* Linux */

#	define PCT_API
#	define CALLAGREEMENT

#	ifndef NDEBUG
#	define _DEUBG
#	endif

#endif	/*WIN32*/


/* PICT Î¢²Ù×÷Âë¶¨Òå£¨ÊÊÓÃÓÚ°æ±¾2¡¢À©Õ¹°æ±¾2£© */
#define PCTDEF_NOP			0x0000			/* No operation 0 */
#define PCTDEF_CLIP			0x0001			/* Clipping region Region size */
#define PCTDEF_BKPAT		0x0002			/* BkPat Background pattern 8 */
#define PCTDEF_TXFONT		0x0003			/* TxFont Font number for text (Integer) 2 */
#define PCTDEF_TXFACE		0x0004			/* TxFace Text's font style (0..255) 1 */
#define PCTDEF_TXMODE		0x0005			/* TxMode Source mode (Integer) 2 */
#define PCTDEF_SPEXTRA		0x0006			/* SpExtra Extra space (Fixed) 4 */
#define PCTDEF_PNSIZE		0x0007			/* PnSize Pen size (Point) 4 */
#define PCTDEF_PNMODE		0x0008			/* PnMode Pen mode (Integer) 2 */
#define PCTDEF_PNPAT		0x0009			/* PnPat Pen pattern 8 */
#define PCTDEF_FILLPAT		0x000A			/* FillPat Fill pattern 8 */
#define PCTDEF_OVSIZE		0x000B			/* OvSize Oval size (Point) 4 */
#define PCTDEF_ORIGIN		0x000C			/* Origin dh, dv (Integer) 4 */
#define PCTDEF_TXSIZE		0x000D			/* TxSize Text size (Integer) 2 */
#define PCTDEF_FGCOLOR		0x000E			/* FgColor Foreground color (Long) 4 */
#define PCTDEF_BKCOLOR		0x000F			/* BkColor Background color (Long) 4 */
#define PCTDEF_TXRATIO		0x0010			/* TxRatio Numerator (Point), denominator (Point) 8 */
#define PCTDEF_VERSIONOP	0x0011			/* VersionOp Version (0..255) 1 */
#define PCTDEF_BKPIXPAT		0x0012			/* BkPixPat Background pixel pattern Variable; see Listing A-1 on page A-17 */
#define PCTDEF_PNPIXPAT		0x0013			/* PnPixPat Pen pixel pattern Variable; see Listing A-1 on page A-17 */
#define PCTDEF_FILLPIXPAT	0x0014			/* FillPixPat Fill pixel pattern Variable; see Listing A-1 on page A-17 */
#define PCTDEF_PNLOCHFRAC	0x0015			/* PnLocHFrac Fractional pen position (Integer—low word of Fixed); 2*/
#define PCTDEF_CHEXTRA		0x0016			/* ChExtra Added width for nonspace characters (Integer) 2*/
#define PCTDEF_REV0017		0x0017			/* Reserved for Apple use Not determined */
#define PCTDEF_REV0018		0x0018			/* Reserved for Apple use Not determined */
#define PCTDEF_REV0019		0x0019			/* Reserved for Apple use Not determined */
#define PCTDEF_RGBFGCOL		0x001A			/* RGBFgCol Foreground color (RGBColor) 6 */
#define PCTDEF_RGBBKCOL		0x001B			/* RGBBkCol Background color (RGBColor) 6 */
#define PCTDEF_HILITEMODE	0x001C			/* HiliteMode Highlight mode flag: no data; this opcode is sent before a drawing operation that uses the highlight mode 0 */
#define PCTDEF_HILITECOLOR	0x001D			/* HiliteColor Highlight color (RGBColor) 6 */
#define PCTDEF_DEFHILITE	0x001E			/* DefHilite Use default highlight color; no data; set highlight to default (from low memory) 0 */
#define PCTDEF_OPCOLOR		0x001F			/* OpColor Opcolor (RGBColor) 6 */
#define PCTDEF_LINE			0x0020			/* Line pnLoc (Point), newPt (Point) 8 */
#define PCTDEF_LINEFROM		0x0021			/* LineFrom newPt (Point) 4 */
#define PCTDEF_SHORTLINE	0x0022			/* ShortLine pnLoc (Point), dh (–128..127), dv (–128..127) 6 */
#define PCTDEF_SHORTLINEFR	0x0023			/* ShortLineFrom dh (–128..127), dv (–128..127) 2 */
#define PCTDEF_REV0024		0x0024			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0025		0x0025			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0026		0x0026			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0027		0x0027			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_LONGTEXT		0x0028			/* LongText txLoc (Point), count (0..255), text 5 + text */
#define PCTDEF_DHTEXT		0x0029			/* DHText dh (0..255), count (0..255), text 2 + text */
#define PCTDEF_DVTEXT		0x002A			/* DVText dv (0..255), count (0..255), text 2 + text */
#define PCTDEF_DHDVTEXT		0x002B			/* DHDVText dh (0..255), dv (0..255), count (0..255), text 3 + text */
#define PCTDEF_FONTNAME		0x002C			/* fontName Data length (Integer), old font ID (Integer), name length (0..255), font name* 5 + name length */
#define PCTDEF_LINEJUSTIFY	0x002D			/* lineJustify Operand data length (Integer), intercharacter spacing (Fixed), total extra space for justification (Fixed) 10 */
#define PCTDEF_GLYPHSTATE	0x002E			/* glyphState Data length (word), followed by these 1-byte Boolean values: outline preferred, preserve glyph, fractional widths, scaling disabled 8 */
#define PCTDEF_REV002F		0x002F			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_FRAMERECT	0x0030			/* frameRect Rectangle (Rect) 8 */
#define PCTDEF_PAINTRECT	0x0031			/* paintRect Rectangle (Rect) 8 */
#define PCTDEF_ERASERECT	0x0032			/* eraseRect Rectangle (Rect) 8 */
#define PCTDEF_INVERTRECT	0x0033			/* invertRect Rectangle (Rect) 8 */
#define PCTDEF_FILLRECT		0x0034			/* fillRect Rectangle (Rect) 8 */
#define PCTDEF_REV0035		0x0035			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0036		0x0036			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0037		0x0037			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_FRAMESAMERT	0x0038			/* frameSameRect Rectangle (Rect) 0 */
#define PCTDEF_PAINTSAMERT	0x0039			/* paintSameRect Rectangle (Rect) 0 */
#define PCTDEF_ERASESAMERT	0x003A			/* eraseSameRect Rectangle (Rect) 0 */
#define PCTDEF_INVERTSAMERT	0x003B			/* invertSameRect Rectangle (Rect) 0 */
#define PCTDEF_FILLSAMERECT	0x003C			/* fillSameRect Rectangle (Rect) 0 */
#define PCTDEF_REV003D		0x003D			/* Reserved for Apple use 0 */
#define PCTDEF_REV003E		0x003E			/* Reserved for Apple use 0 */
#define PCTDEF_REV003F		0x003F			/* Reserved for Apple use 0 */
#define PCTDEF_FRAMERRECT	0x0040			/* frameRRect Rectangle (Rect)‡ 8 */
#define PCTDEF_PAINTRRECT	0x0041			/* paintRRect Rectangle (Rect)‡ 8 */
#define PCTDEF_ERASERRECT	0x0042			/* eraseRRect Rectangle (Rect)‡ 8 */
#define PCTDEF_INVERTRRECT	0x0043			/* invertRRect Rectangle (Rect)‡ 8 */
#define PCTDEF_FILLRRECT	0x0044			/* fillRRect Rectangle (Rect)‡ 8 */
#define PCTDEF_REV0045		0x0045			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0046		0x0046			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0047		0x0047			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_FRAMESAMERRT	0x0048			/* frameSameRRect Rectangle (Rect) 0 */
#define PCTDEF_PAINTSAMERRT	0x0049			/* paintSameRRect Rectangle (Rect) 0 */
#define PCTDEF_ERASESAMERRT	0x004A			/* eraseSameRRect Rectangle (Rect) 0 */
#define PCTDEF_INVSAMERRT	0x004B			/* invertSameRRect Rectangle (Rect) 0 */
#define PCTDEF_FILLSAMERRT	0x004C			/* fillSameRRect Rectangle (Rect) 0 */
#define PCTDEF_REV004D		0x004D			/* Reserved for Apple use 0 */
#define PCTDEF_REV004E		0x004E			/* Reserved for Apple use 0 */
#define PCTDEF_REV004F		0x004F			/* Reserved for Apple use 0 */
#define PCTDEF_FRAMEOVAL	0x0050			/* frameOval Rectangle (Rect) 8 */
#define PCTDEF_PAINTOVAL	0x0051			/* paintOval Rectangle (Rect) 8 */
#define PCTDEF_ERASEOVAL	0x0052			/* eraseOval Rectangle (Rect) 8 */
#define PCTDEF_INVERTOVAL	0x0053			/* invertOval Rectangle (Rect) 8 */
#define PCTDEF_FILLOVAL		0x0054			/* fillOval Rectangle (Rect) 8 */
#define PCTDEF_REV0055		0x0055			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0056		0x0056			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_REV0057		0x0057			/* Reserved for Apple use 8 bytes of data 8 */
#define PCTDEF_FRAMESAMEOV	0x0058			/* frameSameOval Rectangle (Rect) 0 */
#define PCTDEF_PAINTSAMEOV	0x0059			/* paintSameOval Rectangle (Rect) 0 */
#define PCTDEF_ERASESAMEOV	0x005A			/* eraseSameOval Rectangle (Rect) 0 */
#define PCTDEF_INVERTSAMEOV	0x005B			/* invertSameOval Rectangle (Rect) 0 */
#define PCTDEF_FILLSAMEOV	0x005C			/* fillSameOval Rectangle (Rect) 0 */
#define PCTDEF_REV005D		0x005D			/* Reserved for Apple use 0 */
#define PCTDEF_REV005E		0x005E			/* Reserved for Apple use 0 */
#define PCTDEF_REV005F		0x005F			/* Reserved for Apple use 0 */
#define PCTDEF_FRAMEARC		0x0060			/* frameArc Rectangle (Rect), startAngle, arcAngle 12 */
#define PCTDEF_PAINTARC		0x0061			/* paintArc Rectangle (Rect), startAngle, arcAngle 12 */
#define PCTDEF_ERASEARC		0x0062			/* eraseArc Rectangle (Rect), startAngle, arcAngle 12 */
#define PCTDEF_INVERARC		0x0063			/* invertArc Rectangle (Rect), startAngle,arcAngle 12 */
#define PCTDEF_FILLARC		0x0064			/* fillArc Rectangle (Rect), startAngle, arcAngle 12 */
#define PCTDEF_REV0065		0x0065			/* Reserved for Apple use 12 bytes of data 12 */
#define PCTDEF_REV0066		0x0066			/* Reserved for Apple use 12 bytes of data 12 */
#define PCTDEF_REV0067		0x0067			/* Reserved for Apple use 12 bytes of data 12 */
#define PCTDEF_FRAMESAMEARC	0x0068			/* frameSameArc Rectangle (Rect) 4 */
#define PCTDEF_PAINTSAMEARC	0x0069			/* paintSameArc Rectangle (Rect) 4 */
#define PCTDEF_ERASESAMEARC	0x006A			/* eraseSameArc Rectangle (Rect) 4 */
#define PCTDEF_INVSAMEARC	0x006B			/* invertSameArc Rectangle (Rect) 4 */
#define PCTDEF_FILLSAMEARC	0x006C			/* fillSameArc Rectangle (Rect) 4 */
#define PCTDEF_REV006D		0x006D			/* Reserved for Apple use 4 bytes of data 4 */
#define PCTDEF_REV006E		0x006E			/* Reserved for Apple use 4 bytes of data 4 */
#define PCTDEF_REV006F		0x006F			/* Reserved for Apple use 4 bytes of data 4 */
#define PCTDEF_FRAMEPOLY	0x0070			/* framePoly Polygon (Poly) Polygon size */
#define PCTDEF_PAINTPOLY	0x0071			/* paintPoly Polygon (Poly) Polygon size */
#define PCTDEF_ERASEPOLY	0x0072			/* erasePoly Polygon (Poly) Polygon size */
#define PCTDEF_INVERTPOLY	0x0073			/* invertPoly Polygon (Poly) Polygon size */
#define PCTDEF_FILLPOLY		0x0074			/* fillPoly Polygon (Poly) Polygon size */
#define PCTDEF_REV0075		0x0075			/* Reserved for Apple use Polygon (Poly) Polygon size */
#define PCTDEF_REV0076		0x0076			/* Reserved for Apple use Polygon (Poly) Polygon size */
#define PCTDEF_REV0077		0x0077			/* Reserved for Apple use Polygon (Poly) Polygon size */
#define PCTDEF_FRAMESAMEPL	0x0078			/* frameSamePoly (Not yet implemented) 0 */
#define PCTDEF_PAINTSAMEPL	0x0079			/* paintSamePoly (Not yet implemented) 0 */
#define PCTDEF_ERASESAMEPL	0x007A			/* eraseSamePoly (Not yet implemented) 0 */
#define PCTDEF_INVERTSAMEPL	0x007B			/* invertSamePoly (Not yet implemented) 0 */
#define PCTDEF_FILLSAMEPL	0x007C			/* fillSamePoly (Not yet implemented) 0 */
#define PCTDEF_REV007D		0x007D			/* Reserved for Apple use 0 */
#define PCTDEF_REV007E		0x007E			/* Reserved for Apple use 0 */
#define PCTDEF_REV007F		0x007F			/* Reserved for Apple use 0 */
#define PCTDEF_FRAMERGN		0x0080			/* frameRgn Region (Rgn) Region size */
#define PCTDEF_PAINTRGN		0x0081			/* paintRgn Region (Rgn) Region size */
#define PCTDEF_ERASERGN		0x0082			/* eraseRgn Region (Rgn) Region size */
#define PCTDEF_INVERTRGN	0x0083			/* invertRgn Region (Rgn) Region size */
#define PCTDEF_FILLRGN		0x0084			/* fillRgn Region (Rgn) Region size */
#define PCTDEF_REV0085		0x0085			/* Reserved for Apple use Region (Rgn) Region size */
#define PCTDEF_REV0086		0x0086			/* Reserved for Apple use Region (Rgn) Region size */
#define PCTDEF_REV0087		0x0087			/* Reserved for Apple use Region (Rgn) Region size */
#define PCTDEF_FRAMESAMERGN	0x0088			/* frameSameRgn (Not yet implemented) 0 */
#define PCTDEF_PAINTSAMERGN	0x0089			/* paintSameRgn (Not yet implemented) 0 */
#define PCTDEF_ERASESAMERGN	0x008A			/* eraseSameRgn (Not yet implemented) 0 */
#define PCTDEF_INVSAMERGN	0x008B			/* invertSameRgn (Not yet implemented) 0 */
#define PCTDEF_FILLSAMERGN	0x008C			/* fillSameRgn (Not yet implemented) 0 */
#define PCTDEF_REV008D		0x008D			/* Reserved for Apple use 0 */
#define PCTDEF_REV008E		0x008E			/* Reserved for Apple use 0 */
#define PCTDEF_REV008F		0x008F			/* Reserved for Apple use 0 */
#define PCTDEF_BITSRECT		0x0090			/* BitsRect CopyBits with clipped rectangle Variable§¶; see Listing A-2 on page A-17 */
#define PCTDEF_BITSRGN		0x0091			/* BitsRgn CopyBits with clipped region Variable§¶; see Listing A-3 on page A-18 */
#define PCTDEF_REV0092		0x0092			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0093		0x0093			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0094		0x0094			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0095		0x0095			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0096		0x0096			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV0097		0x0097			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_PACKBITSRECT	0x0098			/* PackBitsRect Packed CopyBits with clipped rectangle Variable§; see Listing A-2 on page A-17 */
#define PCTDEF_PACKBITSRGN	0x0099			/* PackBitsRgn Packed CopyBits with clipped rectangle Variable§; see Listing A-3 on page A-18 */
#define PCTDEF_DIRBITSRT	0x009A			/* DirectBitsRect PixMap, srcRect, dstRect, mode (Integer), PixData Variable */
#define PCTDEF_DIRBITSRGN	0x009B			/* DirectBitsRgn PixMap, srcRect, dstRect, mode (Integer), maskRgn, PixData Variable */
#define PCTDEF_REV009C		0x009C			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV009D		0x009D			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV009E		0x009E			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV009F		0x009F			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_SHORTCOMMENT	0x00A0			/* ShortComment Kind (Integer) 2 */
#define PCTDEF_LONGCOMMENT	0x00A1			/* LongComment Kind (Integer), size (Integer), data 4 + data */
#define PCTDEF_REV00A2		0x00A2			/* Reserved for Apple use Data length (Integer), data 2 + data length */
#define PCTDEF_REV00A3		0x00A3
#define PCTDEF_REV00A4		0x00A4
#define PCTDEF_REV00A5		0x00A5
#define PCTDEF_REV00A6		0x00A6
#define PCTDEF_REV00A7		0x00A7
#define PCTDEF_REV00A8		0x00A8
#define PCTDEF_REV00A9		0x00A9
#define PCTDEF_REV00AA		0x00AA
#define PCTDEF_REV00AB		0x00AB
#define PCTDEF_REV00AC		0x00AC
#define PCTDEF_REV00AD		0x00AD
#define PCTDEF_REV00AE		0x00AE
#define PCTDEF_REV00AF		0x00AF
#define PCTDEF_REV00B0		0x00B0			/* Reserved for Apple use 0 */
/*
. . . .
. . . .	===== Reserved for Apple use 0 
. . . .
*/
#define PCTDEF_REV00CF		0x00CF
#define PCTDEF_REV00D0		0x00D0			/* Reserved for Apple use Data length (Long), data 4 + data length */
/*
. . . .
. . . . ===== Reserved for Apple use Data length (Long), data 4 + data length
. . . .
*/
#define PCTDEF_REV00FE		0x00FE
#define PCTDEF_OPENDPIC		0x00FF			/* OpEndPic End of picture 2 */
#define PCTDEF_REV0100		0x0100			/* Reserved for Apple use 2 bytes of data 2 */
/*
. . . .
. . . . ===== Reserved for Apple use 2 bytes of data 2
. . . .
*/
#define PCTDEF_REV01FF		0x01FF			/* Reserved for Apple use 2 bytes of data 2 */
#define PCTDEF_REV0200		0x0200			/* Reserved for Apple use 4 bytes of data 4 */
#define PCTDEF_VERSION		0x02FF			/* Version Version number of picture 2 */
/* 
. . . .
. . . . ===== 0xnnXX	COUNT = nn*2
. . . .
*/
#define PCTDEF_REV0BFF		0x0BFF			/* Reserved for Apple use 22 bytes of data 22 */
#define PCTDEF_HEADEROP		0x0C00			/* HeaderOp For extended version 2: version (Integer), reserved (Integer), hRes, vRes (Fixed), srcRect, reserved (Long); for version 2: opcode 24 */
#define PCTDEF_REV0C01		0x0C01			/* Reserved for Apple use 24 bytes of data 24 */
/* 
. . . .
. . . . Reserved for Apple use 24 bytes of data 24, 0xnnXX	COUNT = nn*2
. . . .
*/
#define PCTDEF_REV7F00		0x7F00			/* Reserved for Apple use 254 bytes of data 254 */
/*
. . . .
. . . . Reserved for Apple use 254 bytes of data 254
. . . .
*/
#define PCTDEF_REV7FFF		0x7FFF			/* Reserved for Apple use 254 bytes of data 254 */
#define PCTDEF_REV8000		0x8000			/* Reserved for Apple use 0 */
/* 
. . . .
. . . . Reserved for Apple use 0
. . . .
*/
#define PCTDEF_REV80FF		0x80FF			/* Reserved for Apple use 0 */
#define PCTDEF_REV8100		0x8100			/* Reserved for Apple use Data length (Long), data 4 + data length */
/*
. . . .
. . . . Reserved for Apple use Data length (Long), data 4 + data length
. . . .
*/
#define PCTDEF_COMPQT		0x8200			/* CompressedQuickTime Data length (Long), data (private to	QuickTime) 4 + data length */
#define PCTDEF_UNCOMPQT		0x8201			/* UncompressedQuickTime Data length (Long), data (private to QuickTime) 4 + data length */
#define PCTDEF_REVFFFF		0xFFFF			/* Reserved for Apple use Data length (Long), data 4 + data length */



/* PICT Í· */
typedef struct
{
	short	size;
	short	top, left, bottom, right;
} PCT_MAGIC, *LPPCT_MAGIC;


/* PICT °æ±¾2 Í¼ÏóÍ·½á¹¹ */
typedef struct
{
	long	ver;						/* °æ±¾ºÅ£¬×ÜÊÇ-1£¨0xffff£©*/
	short	top, rev0, left, rev1, bottom, rev2, right, rev3;
	short	rev4, rev5;
} PCT_HEADER, *LPPCT_HEADER;


/* PICT À©Õ¹°æ±¾2 Í¼ÏóÍ·½á¹¹ */
typedef struct 
{
	short	ver;						/* °æ±¾ºÅ£¬×ÜÊÇ-2£¨0xfffe£©*/
	short	rev0;
	short	hdpi;						/* ×î¼Ñ·Ö±æÂÊÊý¾Ý£¨·ÖË®Æ½¡¢´¹Ö±£© */
	short	rev1;
	short	vdpi;
	short	rev2;
	short	top, left, bottom, right;	/* ¿ÉÑ¡µÄÔ´¾ØÐÎ×ø±ê */
	short	rev3, rev4;
} PCT_EX_HEADER, *LPPCT_EX_HEADER;


/* PICT »ù±¾»æÍ¼ÔªËØ½á¹¹¶¨Òå */
typedef struct
{
	short	y, x;				/* µã×ø±êÊý¾Ý */
} PCT_POINT, *LPPCT_POINT;


typedef struct
{
	short	top, left, bottom, right;
} PCT_RECT, *LPPCT_RECT;


typedef struct
{
	short		size;			/* ÇøÓòÊý¾Ý³ß´ç£¨×Ö½Úµ¥Î»£©*/
	PCT_RECT	enrect;			/* ÇøÓò±ß½ç */
} PCT_REGION, *LPPCT_REGION;


typedef struct
{
	long		pos;			/* Î»Êý¾ÝµØÖ· */
	short		width;			/* ÐÐ¿í¶È£¨×Ö½Úµ¥Î»£¬·ÇÏóËØµ¥Î»£© */
	PCT_RECT	bound;			/* Í¼Ïó±ß½ç */
} PCT_BITMAP, *LPPCT_BITMAP;


typedef struct 
{
	long		baseAddr;
	short		rowBytes;		/* ÐÐ¿í¶È */
	PCT_RECT	bounds;			/* ±ß½ç×ø±ê */
	short		pmVersion;		/* PixMap °æ±¾ºÅ */
	short		packType;		/* Ñ¹ËõÀàÐÍ */
	long		packSize;		/* Ñ¹ËõºóµÄ³ß´ç£¨Õâ¸öÖµÃ»ÓÐ±»Ê¹ÓÃ£¬±ØÐëÎª0£© */
	long		hRes;			/* Ë®Æ½·Ö±æÂÊ */
	long		vRes;			/* ´¹Ö±·Ö±æÂÊ */
	short		pixelType;		/* ÏóËØ¸ñÊ½£¨0£­Ë÷ÒýÍ¼£¬16£­RGBÍ¼£© */
	short		pixelSize;		/* Ã¿ÏóËØµÄÎïÀíÎ»Êý */
	short		cmpCount;		/* Ã¿ÏóËØµÄÂß¼­×é·Ö */
	short		cmpSize;		/* Ã¿×é·ÖµÄÂß¼­Î»Êý */
	long		planeBytes;		/* ÏÂÒ»¸öÃæ°åµÄÆ«ÒÆ */
	long		pmTable;		/* ²ÊÉ«±í½á¹¹µÄ¾ä±ú */
	long		pmReserved;		/* ±£Áô£¬±ØÐëÎª0 */
} PCT_PIXMAP, *LPPCT_PIXMAP;


typedef struct
{
	unsigned short red;
	unsigned short green;
	unsigned short blue;
} PCT_RGBCOLOR, *LPPCT_RGBCOLOR;

typedef struct
{
	short			value;			/* Ë÷ÒýÖµ */
	PCT_RGBCOLOR	rgb;			/* Õæ²ÊÊý¾Ý */
} PCT_COLORSPEC, *LPPCT_COLORSPEC;


typedef struct
{
	long			ctSeed;			/* ±íµÄÎ¨Ò»±êÊ¶ */
	short			ctFlags;		/* ¸ßÎ»£º0 = PixMap; 1 = device */
	short			ctSize;			/* ÏÂÒ»¸öÓòÖÐÏîµÄ¸öÊý */
	PCT_COLORSPEC	ctTable[1];		/* ÑÕÉ«¿Õ¼äÏî */
} PCT_COLORTABLE, *LPPCT_COLORTABLE;


typedef struct
{
	unsigned short	red;
	unsigned short	green;
	unsigned short	blue;
	long			matchData;
} PCT_MATCHRECT, *LPPCT_MATCHRECT;


typedef struct
{
	short			patType;
	long			patMap;
	long			patData;
	long			patXData;
	short			patXValid;
	long			patXMap;
	long			pat1Data;
} PCT_PIXPAT, *LPPCT_PIXPAT;


typedef struct
{
	short			polySize;		/* ×Ö½ÚÊý */
	PCT_RECT		polyBBox;		/* ±ß½ç×ø±ê */
	PCT_POINT		polyPoints[1];	/* ¶¥µã×ø±ê¼¯ */
} PCT_POLYGON, *LPPCT_POLYGON;


typedef struct
{
	unsigned char	pat[8];
} PCT_PATTERN, *LPPCT_PATTERN;


typedef struct
{
	PCT_POINT		pnLoc;			/* Î»ÖÃ */
	PCT_POINT		pnSize;			/* ±Ê³ß´ç */
	short			pnMode;			/* ±ÊµÄÑù°åÄ£Ê½ */
	PCT_PATTERN		pnPat;			/* ±ÊÑù°å */
} PCT_PENSTATE, *LPPCT_PENSTATE;


typedef struct
{
	PCT_PIXMAP		pixmap;
	PCT_RECT		src_rect;		/* Ô´¾ØÐÎ×ø±ê */
	PCT_RECT		dec_rect;		/* Ä¿±ê¾ØÐÎ×ø±ê */
	short			mode;			/* ×ª»»Ä£Ê½ */
} PCT_DIRECT_BITS_RECT, *LPPCT_DIRECT_BITS_RECT;


typedef struct
{
	PCT_PIXMAP		pixmap;
	PCT_RECT		src_rect;		/* Ô´¾ØÐÎ×ø±ê */
	PCT_RECT		dec_rect;		/* Ä¿±ê¾ØÐÎ×ø±ê */
	short			mode;			/* ×ª»»Ä£Ê½ */
	PCT_REGION		mask_rgn;		/* ÑÚÂëÇøÓò */
} PCT_DIRECT_BITS_RGB, *LPPCT_DIRECT_BITS_RGN;





#ifndef __ISEE_IRW_INC__
#include "iseeirw.h"					/* µ÷ÓÃÕßÊý¾Ý½Ó¿Ú¶¨ÒåÎÄ¼þ */
#endif

#if (defined(__cplusplus) || defined(c_plusplus))
extern  "C" {
#endif

#ifdef WIN32
extern PCT_API LPIRWP_INFO	CALLAGREEMENT is_irw_plugin(void);
#else	/* Linux */
extern PCT_API LPIRWP_INFO	CALLAGREEMENT pct_get_plugin_info(void);
extern PCT_API int			CALLAGREEMENT pct_init_plugin(void);
extern PCT_API int			CALLAGREEMENT pct_detach_plugin(void);
#endif	/* WIN32 */

extern PCT_API int	CALLAGREEMENT pct_get_image_info(PISADDR psct, LPINFOSTR pinfo_str);
extern PCT_API int	CALLAGREEMENT pct_load_image(PISADDR psct, LPINFOSTR pinfo_str);
extern PCT_API int	CALLAGREEMENT pct_save_image(PISADDR psct, LPINFOSTR pinfo_str, LPSAVESTR lpsave);

#if (defined(__cplusplus) || defined(c_plusplus))
}
#endif


#endif /* __PCT_MODULE_INC__ */
