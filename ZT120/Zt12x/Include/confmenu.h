#ifndef _CONFMENU_H
#define _CONFMENU_H

#include "const.h"
#include "defines.h"
#include "menu.h"

extern TMenuItemStr CONF_LIST_VOLUME[VOLUME_MODE_LAST];
extern TMenuItemStr CONF_LIST_DEPTH[DEPTH_MODE_LAST];
extern TMenuItemStr CONF_LIST_STIR[MOTOR_MODE_LAST];
extern TMenuItemStr CONF_LIST_COUNT[TESTCNT_MODE_LAST];
extern TMenuItemStr CONF_LIST_ALARM[ALARM_MODE_LAST];
extern TMenuItemStr CONF_LIST_SOUND[SOUND_MODE_LAST];
extern TMenuItemStr CONF_LIST_DFORMAT[DFORMAT_MODE_LAST];
extern TMenuItemStr CONF_LIST_PRINT[PRINTER_MODE_LAST];

// CONFIG_Menu - run configuration menu
extern void CONFIG_Menu(void);

#endif
