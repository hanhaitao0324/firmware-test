#ifndef _COMMANDS_H
#define _COMMANDS_H

#include "const.h"
#include "defines.h"

//commands
enum COMMAND_SET
  {
    cmdLoadLED,			//0
    cmdGetKey,			//1
    cmdResetKBD,                //2

    cmdBeepOnce,                //3
    cmdBeepContinuous,          //4
    cmdBeepOff,                 //5

    cmdGetLifter,               //6
    cmdLiftIn,                  //7
    cmdLiftOut,                 //8

    cmdBrightnessLED,           //9

    cmdPrinterInit,		//10
    cmdPrinterPrint,		//11
    cmdPrinterStatus,		//12
    cmdPrinterDone,		//13

    cmdSetTime,			//14
    cmdGetTime,			//15

    //=========================================================================
    // zt420/720 section
    //=========================================================================

    cmdSetFlashAddr,		//16
    cmdSetBufAddr,		//17
    cmdWriteBuf,		//18
    cmdWriteFlash,		//19
    cmdReadFlash,		//20
    cmdFlashDone,		//21


    //=========================================================================
    // zt120-320 console
    //=========================================================================
    cmdSetLiftInSpeed,		//22
    cmdSetLiftOutSpeed,     	//23
    cmdGetBrightnessLED,	//24
    cmdGetLiftInSpeed,		//25
    cmdGetLiftOutSpeed,     	//26

    //=========================================================================
    // zt420/720 section
    //=========================================================================

    cmdGetValidationDays,	//27
    cmdGetValidationDate,	//28
    cmdSetValidationDate,	//29
    cmdGetValidationInterval,	//30
    cmdSetValidationInterval,	//31

    cmdGetConfig,		//36
    cmdSetConfig,		//37

    //user has logged in
    cmdLoggedIn,		//38

    cmdGetNumMedia,		//39
    cmdGetMediumName,		//40
    cmdGetMediumByID,
    cmdAddMedium,		//41
    cmdDelMedium,		//42

    cmdGetNumForms,		//43
    cmdGetFormName,		//44
    cmdGetFormByID,
    cmdAddForm,			//45
    cmdDelForm,			//46

    cmdGetWarmupSchedule,	//47
    cmdSetWarmupSchedule,	//48

    cmdGetNumProducts,
    cmdGetProductName,
    cmdGetProduct,
    cmdAddProduct,
    cmdUpdateProduct,
    cmdDelProduct,
    cmdPrintProduct,
    cmdPrintAllProducts,

    cmdGetNumUsers,
    cmdGetUserName,
    cmdGetUser,
    cmdCheckUser,
    cmdAddUser,
    cmdUpdateUser,
    cmdDelUser,
    cmdGetAccessLevels,
    cmdSetAccessLevels,

    cmdUpdate=0x80,

    //=====================================
    // bootloader
    //=====================================
    //CAN-bridge request for boot
    COMM_CMD_BOOT_REQUEST=0x90,
    //PC ack for boot
    COMM_CMD_BOOT_ACKNOWLEDGE,
    //get bootloader capabilities
    COMM_CMD_BOOT_GET_CAPS,
    //set current address
    COMM_CMD_BOOT_SET_ADDR,
    //write to buffer
    COMM_CMD_BOOT_WRITE,
    //read from current position
    COMM_CMD_BOOT_READ,
    //release bootloader - reboot
    COMM_CMD_BOOT_REBOOT,

    cmdConnect=0xA5,
    //each device realizes this function in its own way
    cmdTestCommand,

    //autosampler temperature board
    cmdSamplerGetTemp=0xC0,
    cmdSamplerCalibrate,
    cmdSamplerSetMinTemp,
    cmdSamplerSetMaxTemp,
    cmdSamplerGetID,
    cmdSamplerSetID,

    //ekey board
    cmdEkeyTransSerial=0xD0,
    cmdEkeyInitEncode,
    cmdEkeyTransKey,
    cmdEkeyGetStatus,
    cmdEkeyGetExpiredDate,
    cmdEkeyGetVersion,
    cmdEkeyGetID,
    //cmdEkeySetTime,
    cmdEkeyGetAlarmPeriod,
    cmdEkeySetAlarmPeriod,
    cmdEkeyGetErrors,
    cmdEkeyGetPeriod,
    cmdEkeyGetMaxPeriod,
    cmdEkeySetMaxPeriod,
    cmdEkeyGetPeriodLength,
    cmdEkeySetPeriodLength,
    cmdEkeyReset,

    //Ibasket board
    cmdIBasketGetID=0xE0,
    cmdIBasketGetVersion,
    cmdIBasketSetLed,
    cmdIBasketGetTemp,
    cmdIBasketTempOffset,
    cmdIBasketGetData,
    cmdIBasketGetStatus 
  };

enum USB_PRINTER_TEST_COMMANDS
  {
    cmdUSBPRN_IsConnected,
    cmdUSBPRN_GetDescriptor,
    cmdUSBPRN_GetClassStatus,
    cmdUSBPRN_SendBuffer,
    cmdUSBPRN_GetBuffer
  };

enum USB_PRINTER_DESCRIPTOR_TYPE
  {
    USBPRN_DESCR_DEVICE,
    USBPRN_DESCR_CONFIG,
    USBPRN_DESCR_STRING,
    USBPRN_DESCR_DEVICEID
  };

//statuses
enum COMMAND_STATUS
  {
    stsOk=0xA0,
    stsWrongCommand,
    stsWrongValue,
    stsPrnBoardError,
    stsPrnBoardBusy,
    stsNoResponse,
    stsOperationFailed,
    stsItemAlreadyExists,
    stsAccessDenied
  };

//structures
#pragma pack(1)
typedef struct
  {
    struct tagOptions
      {
        TByte led:4;
        TByte blink:1;
      } ledOptions;
  } TLoadLED;

//printer status
typedef struct
  {
    TByte busy:1;
    TByte error:1;
    TByte coverup:1;
    TByte outofpaper:1;
    TByte paperjam:1;
    TByte carriagestall:1;
    TByte connected:1;
  } TPrinterStatus;
#pragma pack()

#endif
