#ifndef _EDIT_H
  #define _EDIT_H

#include "defines.h"
#include "timedate.h"

typedef void TEditCallback(TWord current);

// EDIT_Bool - edit boolean
extern TBool EDIT_Bool(char __flash *title);
// EDIT_Integer - edit integer
extern TBool EDIT_Integer(char __flash *title, char __flash *format, TWord *value,
  TWord lo, TWord hi, TEditCallback *callback);
// EDIT_Time - edit time in minutes
extern TBool EDIT_Time(char __flash *title, TWord *minutes);
// EDIT_Date - edit date (day and month)
extern TBool EDIT_Date(TTimeDate *tmpDt);
// EDIT_Show - just show text
extern TBool EDIT_Show(char *title, char *text, TBool blink);
// EDIT_Block - block keyboard
extern void EDIT_Block(void);
// EDIT_Advanced - advanced edit (serial number and password)
extern TBool EDIT_Advanced(char *title, char *tStr, TByte len, TBool sound);

// √‹¬Î ‰»Î£¨v2.10, Gavin£¨2020-09-03 –«∆⁄Àƒ 14:38:07
extern TBool EDIT_AdvancedPW(char *title, char *tStr, TByte len, TBool sound);

// EDIT_Hex - edit HEX-Value
extern TBool EDIT_Hex(char *title, char *tStr, TByte len);
// EDIT_Start - show logo and device id
extern void EDIT_Start(void);

#endif
