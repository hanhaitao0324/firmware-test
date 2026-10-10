#ifndef _MENU_H
#define _MENU_H

#include "defines.h"

typedef TBool TItemHandler(void);

typedef TItemHandler *ptrTItemHandler;

typedef char __flash TMenuItemStr[5];

typedef struct
  {
    TByte numItems;
    char __flash *title;
    TMenuItemStr *items;
    ptrTItemHandler *handler;
  } TMenu;

typedef struct
  {
    TByte numItems;
    TByte current;
    char __flash *title;
    TMenuItemStr *items;
  } TList;

extern void MENU_Run(TMenu *menu);
extern signed char MENU_RunList(TList *list);

extern TMenuItemStr CHOOSE_LIST[TIME_UNIT_LAST];

#endif
