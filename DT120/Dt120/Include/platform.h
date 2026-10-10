#ifndef _PLATFORM_H
  #define _PLATFORM_H

#include <iom128.h>
#include <inavr.h>
#include <pgmspace.h>

#define XTAL	(14745600L)

#define RS_BaudRate(baud)	(XTAL/16L/baud-1)

#define PCBID_ADDR  (0x0ff0)

#endif
