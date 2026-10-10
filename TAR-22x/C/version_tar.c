#include "version.h"

/* History
  02.17 - TIMEDATE_IsValidDate bug fixed (29th of February),
          can enter 29th of February now, date edit function improved,
          LEDs switched off in bus update mode;

  02.16 - beeper output bug fixed;

  02.15 - page number on the top of the report,
          drum data order to 2 columns;

  02.14 - some structure changes for reports,
          portable printer support added, P.TYP in configuration menu,
          printer board updater function added,
          small possible problem in parsing commands solved,
          some visual changes in the report (less lines),
          shows printer board version in date/time menu,
          changes in test report - min-max speed instead of unclear table;

  02.13 - new board from AKC support,
          LEDs switched off in update mode,
          sounds for repeatable keys removed,
          device restarts after update, bug fixed,
          time setting bug fixed: if seconds changed and after that time is set as more than 1h00, seconds are not visible, but counted as runtime;

  02.12 - TA/TAR revolutions/time settings changed to quick setup (STOP-button);

  02.11 - FT15/FT25 changed;

  02.10 - test aborted message in revolutions mode fixed;

  02.09 - PCB id in report shown without prefix;

  02.08 - test procedures added,
          motor moving bug in pause mode fixed;

  02.07 - small bugfixes in report;

  02.06 - slow motor start for high PPR;

  02.05 - statistics bug fixed, speed wrong with test time less than 20 seconds;

  02.04 - speed on start displaying bug fixed;

  02.03 - EKey functionality added;

  02.02 - logo and device displayed on start,
          speed control improved,
          GETSPEED bug fixed,
          RMP bug in configuration menu fixed (no storing),
          GETSPEED value derived from display to improve quality,
          device type and mode (TA/TAR/FT),
          preset updates sound and brightness,
          firmware version in CONF menu,
          SETTESTMODE bug fixed,
          Total runtime seconds in report fixed,
          SETSERIAL bug fixed,
          LED menu moved from Engineering to Conf,
          USB buffer emptying after connection;

  02.01 - report changed,
          some internal improving changes,
          EEPROM values saved during update process;

  02.00	- base version (new board);
*/

//__flash char VERSION_Revision[]="02.17";
//__flash char VERSION_Revision[]="02.20"; 										// Gavin， 2019-08-03 星期六 14:43:38
//__flash char VERSION_Revision[]="02.21"; 										// Gavin， 2019-11-05 星期二 16:56:22
__flash char VERSION_Revision[]="02.22"; 										// Gavin， 2021-12-22 星期三 14:01:22

__flash char VERSION_Str[]="VER ";

__flash char DEVICE_TYPE[DEVICE_TYPE_LAST][5]=
{
  {"TA\x00"},
  {"TAR\x00"},
  {"FT\x00"}
};

__flash char DEVICE_MODE[DEVICE_TYPE_LAST][DEVICE_MODE_LAST][5]=
{
  {
    "120\x00",
    "220\x00",
    "420\x00"
  },
  {
    "120\x00",
    "220\x00",
    "420\x00"
  },
  {
    "15\x00",
    "25\x00",
    "\x00"
  }
};

__flash char DEVICE_LOGO[DEVICE_LOGO_LAST][10]=
{
  {"ERWEKA\x00"},
  {"CALEVA\x00"},
  {"TABTEST\x00"},
  {"QTEST\x00"}
};
