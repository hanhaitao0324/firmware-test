#include "version.h"

/* History
  01.16 - TIMEDATE_IsValidDate bug fixed (29th February),
          can enter 29th of February now, date edit function improved,
          LEDs switched off in bus update mode;

  01.15 - beeper output bug fixed;

  01.14 - instrument ID removed from big test report;

  01.13 - some structure changes for reports,
          portable printer support added, P.TYP in configuration menu,
          printer board updater function added,
          small possible problem in parsing commands solved,
          changes in OQ procedures (height-3/14mm, time is variable),
          changes in test report - min-max speed instead of unclear table,
          shows printer board version in date/time menu;

  01.12 - standard PPR setting changed to 120;

  01.11 - new board from AKC support,
          sounds for repeatable keys removed,
          device restarts after update, bug fixed;

  01.10 - PPR engineering menu added,
          LEDs switched off in update mode,
          test mode in engineering menu,
          last result skipping bug for some stroke mode tests fixed (9 results in full test, not 10);

  01.09 - TDT types changed (1-12/12/1-22/22/23);

  01.08 - PCB id in report shown without prefix;

  01.07 - slow motor start;

  01.06 - SETPPR limits lowered to 10 ppm;

  01.05 - possible timing bug fixed,
          possible statistics bug fixed, speed wrong with test time less than 20 seconds;

  01.04 - EKey status protocol changed;

  01.03 - logo and device displayed on start,
          GETSPEED bug fixed,
          motor control improved,
          device types and modes (SVM/TDT),
          firmware version in CONF menu,
          SETSERIAL bug fixed,
          USB buffer emptying after connection;

  01.02 - EKey functionality added;

  01.01 - report changed,
          some minor changes in parser module;

  01.00	- base version;
*/

//__flash char VERSION_Revision[]="01.16";
//__flash char VERSION_Revision[]="01.20"; 					// Q1，Gavin，2019-05-09 星期四 14:36:24
//__flash char VERSION_Revision[]="01.30"; 					// v1.30, Q1，Gavin，2019-08-03 星期六 14:44:42
__flash char VERSION_Revision[]="01.31"; 					// Gavin，2021-12-22 星期三 15:06:30

__flash char VERSION_Str[]="VER ";

__flash char DEVICE_TYPE[DEVICE_TYPE_LAST][5]=
{
  {"SVM\x00"},
  {"TDT\x00"}
};

__flash char DEVICE_MODE[DEVICE_TYPE_LAST][DEVICE_MODE_LAST][5]=
{
  {
    {"121\x00"},
    {"122\x00"},
    {"221\x00"},
    {"222\x00"},
    {"223\x00"}
  },
  {
    /*{"12\x00"},
    {"22\x00"},
    {"23\x00"},
    {"\x00"},
    {"\x00"}*/
    {"1-12\x00"},
    {"12\x00"},
    {"1-22\x00"},
    {"22\x00"},
    {"23\x00"}
  }
};

__flash char DEVICE_LOGO[DEVICE_LOGO_LAST][10]=
{
  {"ERWEKA\x00"},
  {"CALEVA\x00"},
  {"TABTEST\x00"},
  {"QTEST\x00"}
};
