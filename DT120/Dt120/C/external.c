#include "defines.h"
#include "eeprom.h"
#include "external.h"
#include "global.h"
#include "usb.h"
#include "parser.h"

TBool EXTERNAL_Process(void)
{
  /*switch (globalDeviceStatus.portMode)
  {
    case PORT_MODE_USB:
      USB_Process();
      if (globalDeviceStatus.gotUSBPackage)
      {
        //got a USB package
        globalDeviceStatus.gotUSBPackage=false;
        PARSER_Comm(PORT_MODE_USB);
        return true;
      }
      break;
    case PORT_MODE_COM: 
      if (globalDeviceStatus.gotRS232Package)
      {
        //got RS232 package
        globalDeviceStatus.gotRS232Package=false;
        PARSER_Comm(PORT_MODE_COM);
        return true;
      }  
      break;
  }*/
  if (globalDeviceStatus.portMode==PORT_MODE_COM)
  {
    if (globalDeviceStatus.gotRS232Package)
    {
      //got RS232 package
      globalDeviceStatus.gotRS232Package=false;
      PARSER_Comm(PORT_MODE_COM);
      return true;
    }  
  }  
  USB_Process();
  if (globalDeviceStatus.gotUSBPackage)
  {
    //got a USB package
    globalDeviceStatus.gotUSBPackage=false;
    PARSER_Comm(PORT_MODE_USB);
    return true;
  }
  return false;
}
