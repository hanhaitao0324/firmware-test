#include "version.h"

/* Version history
  01.34 - accept 'T' and 'I' for compartibility with new DHC board;

  01.33 - autosampler Pt100 calibration added to user menu,
          volume shown by AS Pt100 calibration corrected,
          menu for choosing remote control port (USB or COM) added to user menu,
          USB port always active in any port mode (important for update function);

  01.32 - sounds for repeatable keys removed;

  01.31 - DT620 can have external temperature sensor;

  01.30 - new board from AKC support;

  01.29 - motor error and encoder error displayed in connection with PC (E15 - current, E16 - encoder),
          test report can be printed if motor is blocked;

  01.28 - motor error classified (current - MTR.1, encoder - MTR.2) for display,
          current error from the stirrer motor switchable in engineering menu (default - On),
          current overflow error detecting algorithm changed (should apply for 0.5s in 2secs); 

  01.27 - probe height fix for 1000 ml;

  01.26 - probe height for different volumes possible (linear approximation);

  01.25 - Calibration -> Adjustment,
          min and max value for each report position is dispayed,
          check temperature procedure changed and renamed to Calibration,
          check temperature procedure added for Heater, Pt100 and Autosampler board;

  01.24 - don't auto move probes down if volume>1000ml,
          head is moving up at the test start if HighHead mode selected,
          speed tolerance changed to 2RPM,
          some strings in report changed (test was not completed);

  01.23 - 4000ml volume added;

  01.22 - time counting procedure improved (problem with Pt100 connected);

  01.21 - ST device for Caleva added;

  01.20 - 2000ml, 1900ml, 1750ml, 1500ml volumes added;

  01.19 - Prn Error if printer is not connected;

  01.18 - dropper and Pt100 sensor bug fixed (from ver.1.17);

  01.17 - dropper initial movement bug fixed;

  01.16 - ignore \n in commands;

  01.15 - possible timer bug fixed,
          DT1420 mode added,
          GBO and SBO commands changed,
          basket offset removed, method offset instead,
          Pt100 calibration mode entering error fixed;

  01.14 - high head mode for DT62x fixed,
          temperature offset range changed to -2.5..2.5C;

  01.13 - DT620 mode added,
          command for device mode setting added (STP),
          set mode option in engineering menu added;

  01.12 - print request in remote mode fixed;

  01.11 - stepper offset for basket changed;

  01.10 - report bug fixed (page truncate on 20 records),
          basket offset added,
          basket offset commands added (GBO, SBO),
          new configuration menu for basket offset added,
          report storing in flash memory changed to save space;

  01.09 - commands can be send via USB or RS232 (device will not be able to work as stand-alone if RS232 mode is selected), 
          new engineering menu for choosing remote control port added (PORT - USB or COM),
          statistics not collected in remote mode to save processor resources,
          test ending bug in remote mode fixed (test will be finished after time is out),
          TT0.0 bug fixed (E2 was displayed),
          max and min speed reset on ST command,
          no alarm in remote mode,
          blocked motor error exits remote mode and send E15 as answer for SR, ST and SS commands,
          blocked dropper exits remote mode,
          timing bug in SS command fixed;

  01.08 - small bugfix in stepper procedure (mm per rotation),
          stepper maximal distance increased to 90mm (probably error if stepper step lower than 1,1 mm per rotation);

  01.07 - report changed - date moved to the next string,
          possible temperature drift error fixed;

  01.06 - bug with sounds and commands in remote mode fixed (stops sounds after pressing any key during the test),
          stirring speed control improved,
          logo and device displayed on start,
          HighMode block for stepper used only in test process,
          shows firmware version in CONF menu,
          ignoring one-point temperature peak for DHC and external PT100,
          firm logo fixed and added to engineering menu,
          motor testing procedure improved;

  01.05 - time in remote mode not counted,
          some changes in remote mode workflow,
          number of external temperature sensors: 0 or 9;

  01.04 - parser bug fixed (wrong empty command detection - TT after TT37.0 will set temperature to 7.0C),
          checking temperatures can be modified,
          EEPROM modified to have place for putting new values in the future (20 bytes and 30 bytes),
          EEPROM content will not be erased during the update procedure,
          EEPROM variables made smaller to save size (TWord, TByte vs. TReal32),
          stepper distance calculation improved,
          stepper spindle step can be modified (MS, SMS);

  01.03 - EEPROM variables bug fixed (no EEPROM reading in interrupts allowed),
          long report bugs fixed (maximal 256 records - TByte overflow, reset time after 18h12m - TWord overflow),
          some changes in report (bad English),
          check temperature after calibration (3 points),
          calibration of autosampler Pt100 sensors removed from USER menu,
          new strings in report translated,
          check temperature report added,
          bug in autosampler sensors calibration report fixed (values not initialized),
          printer test report improved with device settings;

  01.02 - calibrating stepper position,
          report with header,
          total pages in report,
          report changed,
          some bugs fixed,
          makes report in GO-mode;

  01.01 - heating process not paused,
          starts heating before calibration,
          temperature offset only positive,
          report cycle and head position printed,
          report starts from zero,
          Pt100 in sampling station calibrated separately,
          sampling station down before calibration,
          device type unified,
          report starts with spaces to be compartible with EPSON,
          can calibrate Pt100 in sampling station with heating,
          initial heating;

  01.34	- base version;
*/

// 2017-11-5 星期日 16:01:42------------
//__flash char DEVICE_VERSION[]="01.00"; 							// 基础新型号，DT120基础上增加加热板功能

// 2018-07-13 星期五 13:48:42------------
//__flash char DEVICE_VERSION[]="01.01"; 							// 基础新型号，DT120基础上增加加热板功能

//2019-3-26 15:24:58--------------------
//__flash char DEVICE_VERSION[]="01.02"; 							// 基础新型号，DT120基础上增加加热板功能

// 240613-1007 -------------------------
__flash char DEVICE_VERSION[]="01.04"; 							//  

// Gavin, 2019-3-26 15:26:09, A2.6
/*
__flash char DEVICE_LOGO[DEVICE_LOGO_LAST][10]=
{
  {"ERWEKA\x00"},
  {"CALEVA\x00"},
  {"TABTEST\x00"},
  {"QTEST\x00"}
};
*/
__flash char DEVICE_LOGO[DEVICE_LOGO_LAST][10]=
{
  {"ERWEKA\x00"}
};

