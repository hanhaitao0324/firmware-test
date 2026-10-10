#include <ctype.h>
#include <string.h>
#include <stdio.h>
                  
#include "calibrate.h"
#include "const.h"
#include "control.h"
#include "dhc.h"
#include "dropper.h"
#include "eeprom.h"
#include "ekey.h"
#include "fram.h"
#include "global.h"
#include "led.h"
#include "lifter.h"
#include "motor.h"
#include "parser.h"
#include "platform.h"
#include "printrtc.h"
#include "rs232.h"
#include "sampler.h"
#include "sound.h"
#include "stepper.h"
#include "strings.h"
#include "temp.h"
#include "updater.h"
#include "usb.h"
#include "version.h"

#define BUFFER_SIZE   (RS232_BUFFER_I_SIZE)

//------------------------------------------------------------------------------
enum PARSER_COMMANDS
{
	PARSER_CMD_FW, 							// 0，获取软件版本号
	PARSER_CMD_ID, 							// 1，获取设备识别号
	PARSER_CMD_GI, 							// 2，获取序列号
	PARSER_CMD_SIN, 						// 3，设置序列号
	PARSER_CMD_PID, 						// 4，获取PCB识别号
	PARSER_CMD_HELP, 						// 5，获取帮助屏幕
	PARSER_CMD_PSW, 						// 6，initiate connection with password
	PARSER_CMD_SDV, 						// 7，预设默认值
	PARSER_CMD_BR, 							// 8，设置亮度值（0-9）
	PARSER_CMD_SND, 						// 9，设置声音模式（0-关闭，1-打开） 
	
	PARSER_CMD_ALR, 						// 10，设置报警模式（0-关，1-开）
	PARSER_CMD_PRN, 						// 11，设置打印模式（0-关闭，1-自动，2-询问）
	PARSER_CMD_DTF, 						// 12，设置日期/时间格式（0-EUR，1-USA）
	PARSER_CMD_CNT, 						// 13，设置时间计数模式（0-倒计时，1-顺计时）
	
	/*
	PARSER_CMD_REP,
	PARSER_CMD_LOGO,
	PARSER_CMD_SLOGO,
	*/
	
	PARSER_CMD_SD, 							// 14，设置测试持续时间：HHMM
	PARSER_CMD_TD, 							// 15，获取剩余测试时间：HHMMSS
	PARSER_CMD_CLPF, 						// 16，为PT100设置第一个校准点 
	PARSER_CMD_CLPS, 						// 17，为PT100设置第二个校准点 
	PARSER_CMD_CLBR, 						// 18，对PT100进行温度校准，将CLPF值设置为实际温度
	PARSER_CMD_CLHF, 						// 19，对加热器进行第一点校准 
	
	PARSER_CMD_CLHS, 						// 20，对加热器进行第二点校准 
	PARSER_CMD_TR, 							// 21，获取实际恒温器温度 
	PARSER_CMD_TT, 							// 22，设定目标温度 
	PARSER_CMD_TI, 							// 23，获取PT100传感器的温度 
	PARSER_CMD_MINT, 						// 24，获得最低温度 
	PARSER_CMD_MAXT, 						// 25，获得最高温度 
	PARSER_CMD_SMINT, 					// 26，设定最低温度 
	PARSER_CMD_SMAXT, 					// 27，设定最高温度 
	PARSER_CMD_TO, 							// 28，获取当前温度偏移
	PARSER_CMD_TB, 							// 29，设定温度偏差，-2.5-+2.5C 

	PARSER_CMD_SR, 							// 30，获取实际电机转速，sxx 
	PARSER_CMD_ST, 							// 31，设定搅拌目标速度 
	PARSER_CMD_SS, 							// 32，开始搅拌。要停止，需要将速度设置为0 
	PARSER_CMD_SA, 							// 33，在测试期间获得最大速度，AXXX
	PARSER_CMD_SI, 							// 34，在测试期间获得最小速度，IXXX
	PARSER_CMD_PPR, 						// 35，获取每次旋转的脉冲数 
	//PARSER_CMD_SPPR, 						// 36，设置每次旋转的脉冲数 2019-3-1 星期五 14:38:22
	PARSER_CMD_ADC, 						// 36，获取原始ADC值 
	PARSER_CMD_UT, 							// 37，获取屏幕上温度更新的刷新时间 
	PARSER_CMD_SUT, 						// 38，设定屏幕上温度更新的刷新时间（1-10） 
	
	PARSER_CMD_PRD, 						// 39，设置当前日期（ddmmyy），需要连接打印板
	PARSER_CMD_PRT, 						// 40，设置当前时间（hhmm），需要连接打印板
	PARSER_CMD_GDT, 						// 41，获取当前日期/时间，需要连接打印板
	PARSER_CMD_GUT, 						// 42，获取设备使用时间（小时）：GU XXXXXX
	PARSER_CMD_RUT, 						// 43，重置设备使用时间 
	
	/*
	PARSER_CMD_GLH,
	PARSER_CMD_GLR,
	*/
	
	PARSER_CMD_SM, 							// 44，request of methode
	PARSER_CMD_TM, 							// 45，设置方法（0-篮，1-桨，2-桨盘，3-缸） 
	PARSER_CMD_RE, 							// 46，退出远程模式 
	
	/*
	PARSER_CMD_STP,
	PARSER_CMD_CLAS1,
	PARSER_CMD_CLAS2,
	PARSER_CMD_PH1,
	PARSER_CMD_PH0,
	PARSER_CMD_LU,
	PARSER_CMD_LD,
	PARSER_CMD_PO,
	PARSER_CMD_VN,
	PARSER_CMD_PU,
	PARSER_CMD_PD,
	PARSER_CMD_PA,
	PARSER_CMD_PE,
	PARSER_CMD_SMP,
	PARSER_CMD_MS,
	PARSER_CMD_SMS,
	PARSER_CMD_PB,
	PARSER_CMD_PF,
	PARSER_CMD_TN,
	PARSER_CMD_T,
	PARSER_CMD_SHM,
	PARSER_CMD_GHM,
	PARSER_CMD_SBO,
	PARSER_CMD_GBO,
	*/
	
	//PARSER_CMD_SVN,
	//PARSER_CMD_SPID,
	
	PARSER_CMD_UPFW, 						// 47，
	PARSER_CMD_UPBUS, 					// 48，
	
	PARSER_CMD_EKEY, 						// 49，

	PARSER_CMD_DT, 							// 50，

	PARSER_CMD_NULL, 						// 51, 空命令

	PARSER_NUM_COMMANDS 				// 52
};

//------------------------------------------------------------------------------

// - FW -
__flash char parserCmdVer[]="FW";
__flash char parserOutVer[]="FW %s";
__flash char parserHlpVer[]="get firmware version: FW XX.XX";

// - ID -
__flash char parserCmdID[]="ID";
__flash char parserOutID[]="I%d2%1d";                 
__flash char parserHlpID[]="get device identification number";

// - GI -
__flash char parserCmdGI[]="GI";
__flash char parserOutGI[]="GI %s";
__flash char parserHlpGI[]="get serial number: GI XXXXXX";

// - SIN -
__flash char parserCmdSIN[]="SIN";
__flash char parserInSIN[]="%s";
__flash char parserHlpSIN[]="set serial number";

// - PID -
__flash char parserCmdPID[]="PID";
__flash char parserOutPID[]="PID %s";
__flash char parserHlpPID[]="get PCB identification number: PID XXXXXXXXXX";

/*// - SPID -
__flash char parserCmdSPID[]="SPID";
__flash char parserInSPID[]="%s";
__flash char parserHlpSPID[]="set PCB identification number";*/

// - HELP -
__flash char parserCmdHelp[]="HELP";
__flash char parserHlpHelp[]="get help screen";

// - PSW -
__flash char parserCmdPSW[]="PSW";
__flash char parserInPSW[]="%04d";
__flash char parserHlpPSW[]="initiate connection with password XXXX";

// - SDV -
__flash char parserCmdSDV[]="SDV";
__flash char parserHlpSDV[]="preset default values";

// - BR -
__flash char parserCmdBR[]="BR";
__flash char parserInBR[]="%1d";
__flash char parserHlpBR[]="set brightness value (0-9)";

// - SND -
__flash char parserCmdSND[]="SND";
__flash char parserInSND[]="%1d";
__flash char parserHlpSND[]="set sound mode (0-off, 1-on)";

// - ALR -
__flash char parserCmdALR[]="ALR";
__flash char parserInALR[]="%1d";
__flash char parserHlpALR[]="set alarm mode (0-off, 1-on)";

// - PRN -
__flash char parserCmdPRN[]="PRN";
__flash char parserInPRN[]="%1d";
__flash char parserHlpPRN[]="set printing mode (0-off, 1-auto, 2-ask)";

// - DTF -
__flash char parserCmdDTF[]="DTF";
__flash char parserInDTF[]="%d";
__flash char parserHlpDTF[]="set date/time format (0-EUR, 1-USA)";

// - CNT -
__flash char parserCmdCNT[]="CNT";
__flash char parserInCNT[]="%1d";
__flash char parserHlpCNT[]="set time counting mode (0 - count down, 1 - count up)";

/* 2019-2-28 星期四 16:40:57
// - REP -
__flash char parserCmdREP[]="REP";
__flash char parserInREP[]="%d";
__flash char parserHlpREP[]="set report interval in minutes (1-60)";

// - LOGO -
__flash char parserCmdLOGO[]="LOGO";
__flash char parserOutLOGO[]="%s";
__flash char parserHlpLOGO[]="get firm logo";

// - SLOGO -
__flash char parserCmdSLOGO[]="SLOGO";
__flash char parserInSLOGO[]="%1d";
__flash char parserHlpSLOGO[]="set firm logo (0-Erweka, 1-Caleva, 2-Tabtest, 3-Qtest)";
*/

// - SD -
__flash char parserCmdSD[]="SD";
__flash char parserInSD[]="%02d%02d";
__flash char parserHlpSD[]="set test duration: HHMM";

// - TD -
__flash char parserCmdTD[]="TD";
__flash char parserOutTD[]="%02d%02d%02d";
__flash char parserHlpTD[]="get test time left: HHMMSS";

// - CLPF -
__flash char parserCmdCLPF[]="CLPF";
__flash char parserInCLPF[]="%f";
__flash char parserHlpCLPF[]="set first calibration point for Pt100";

// - CLPS -
__flash char parserCmdCLPS[]="CLPS";
__flash char parserInCLPS[]="%f";
__flash char parserHlpCLPS[]="set second calibration point for Pt100";

// - CLBR -
__flash char parserCmdCLBR[]="CLBR";
__flash char parserHlpCLBR[]="perform temperature calibration for Pt100";

// - CLHF -
__flash char parserCmdCLHF[]="CLHF";
__flash char parserInCLHF[]="%f";
__flash char parserHlpCLHF[]="perform first point calibration for heater";

// - CLHS -
__flash char parserCmdCLHS[]="CLHS";
__flash char parserInCLHS[]="%f";
__flash char parserHlpCLHS[]="perform second point calibration for heater";

// - TR -
__flash char parserCmdTR[]="TR";
__flash char parserOutTR[]="T%04.1f";
__flash char parserHlpTR[]="get actual thermostat temperature";

// - TT -
__flash char parserCmdTT[]="TT";
__flash char parserInTT[]="%f";
__flash char parserHlpTT[]="set target temperature";

// - TI -
__flash char parserCmdTI[]="TI";
__flash char parserOutTI[]="I%04.1f";
__flash char parserHlpTI[]="get temperature of Pt100 sensor";

// - MINT -
__flash char parserCmdMINT[]="MINT";
__flash char parserOutMINT[]="%.1f";
__flash char parserHlpMINT[]="get minimal temperature";

// - MAXT -
__flash char parserCmdMAXT[]="MAXT";
__flash char parserOutMAXT[]="%.1f";
__flash char parserHlpMAXT[]="get maximal temperature";

// - SMINT -
__flash char parserCmdSMINT[]="SMINT";
__flash char parserInSMINT[]="%f";
__flash char parserHlpSMINT[]="set minimal temperature";

// - SMAXT -
__flash char parserCmdSMAXT[]="SMAXT";
__flash char parserInSMAXT[]="%f";
__flash char parserHlpSMAXT[]="set maximal temperature";

// - TO -
__flash char parserCmdTO[]="TO";
__flash char parserOutTO[]="B%.1f";
__flash char parserHlpTO[]="get current temperature offset";

// - TB -
__flash char parserCmdTB[]="TB";
__flash char parserInTB[]="%f";
__flash char parserHlpTB[]="set temperature offset, -2.5 - +2.5C";

// - SR -
__flash char parserCmdSR[]="SR";
__flash char parserOutSR[]="S%03d";
__flash char parserHlpSR[]="get actual motor speed, SXXX";

// - ST -
__flash char parserCmdST[]="ST";
__flash char parserInST[]="%03d";
__flash char parserHlpST[]="set target speed of stirling";

// - SS -
__flash char parserCmdSS[]="SS";
__flash char parserHlpSS[]="start stirling. To stop needs to set speed to 0";

// - SA -
__flash char parserCmdSA[]="SA";
__flash char parserOutSA[]="A%03d";
__flash char parserHlpSA[]="get maximal speed during test, AXXX";

// - SI -
__flash char parserCmdSI[]="SI";
__flash char parserOutSI[]="I%03d";
__flash char parserHlpSI[]="get minimal speed during test, IXXX";

// - PPR -
__flash char parserCmdPPR[]="PPR";
__flash char parserOutPPR[]="PPR%03d";
__flash char parserHlpPPR[]="get pulses per rotation";

/* 2019-3-1 星期五 14:40:19
// - SPPR -
__flash char parserCmdSPPR[]="SPPR";
__flash char parserInSPPR[]="%d";
__flash char parserHlpSPPR[]="set pulses per rotation";
*/

// - ADC -
__flash char parserCmdADC[]="ADC";
__flash char parserOutADC[]="%04x";
__flash char parserHlpADC[]="get raw ADC value";

// - UT -
__flash char parserCmdUT[]="UT";
__flash char parserOutUT[]="UT%02d";
__flash char parserHlpUT[]="get refresh time for temperature on screen to update";

// - SUT -
__flash char parserCmdSUT[]="SUT";
__flash char parserInSUT[]="%d";
__flash char parserHlpSUT[]="set refresh time for temperature on screen to update (1-10)";

// - PRD -
__flash char parserCmdPRD[]="PRD";
__flash char parserInPRD[]="%02d%02d%02d";
__flash char parserHlpPRD[]="set current date (ddmmyy)";

// - PRT -
__flash char parserCmdPRT[]="PRT";
__flash char parserInPRT[]="%02d%02d";
__flash char parserHlpPRT[]="set current time (hhmm)";

// - GDT -
__flash char parserCmdGDT[]="GDT";
__flash char parserOutGDT[]="%02d.%02d.%02d %02d:%02d";
__flash char parserHlpGDT[]="get current date/time";

// - GUT - 
__flash char parserCmdGUT[]="GUT";
__flash char parserOutGUT[]="GUT %06d";
__flash char parserHlpGUT[]="get device using time in hours: GU XXXXXX";

// - RUT - 
__flash char parserCmdRUT[]="RUT";
__flash char parserHlpRUT[]="reset device using time";

/* 2019-2-28 星期四 18:04:37
// - GLH -
__flash char parserCmdGLH[]="GLH";
__flash char parserOutGLH[]="%d|%02d|%.1f|%.1f|%.1f|%.1f";
__flash char parserHlpGLH[]="get log header (records|interval|min.speed|max.speed|min.temp|max.temp)";

// - GLR -
__flash char parserCmdGLR[]="GLR";
__flash char parserInGLR[]="%4d";
__flash char parserOutGLR[]="%s|%.1f";//|%s|%s|%s|%s|%s|%s|%s|%s";
__flash char parserHlpGLR[]="get log record (temp|speed)";
*/

// - SM -
__flash char parserCmdSM[]="SM";
__flash char parserOutSM[]="M%d";
__flash char parserHlpSM[]="request of methode";

// - TM -
__flash char parserCmdTM[]="TM";
__flash char parserInTM[]="%d";
__flash char parserHlpTM[]="set methode (0-basket, 1-paddle, 2-paddle over disk, 3-cylinder)";

// - RE -
__flash char parserCmdRE[]="RE";
__flash char parserHlpRE[]="exit remote mode";

/* 2019-2-28 星期四 16:38:34
// - STP -
__flash char parserCmdSTP[]="STP";
__flash char parserInSTP[]="%d";
__flash char parserHlpSTP[]="set device type (0-DT620, 1-DT720, 2-DT1420)";

// - CLAS1 -
__flash char parserCmdCLAS1[]="CLASF";
__flash char parserInCLAS1[]="%1d %f";
__flash char parserHlpCLAS1[]="perform first point calibration for autosampler (Y ZZ.Z - channel, temperature)";

// - CLAS2 -
__flash char parserCmdCLAS2[]="CLASS";
__flash char parserInCLAS2[]="%1d %f";
__flash char parserHlpCLAS2[]="perform second point calibration for autosampler (Y ZZ.Z - channel, temperature)";

// - PH1 -
__flash char parserCmdPH1[]="PH1";
__flash char parserHlpPH1[]="open dropper";

// - PH0 -
__flash char parserCmdPH0[]="PH0";
__flash char parserHlpPH0[]="close dropper";

// - LU -
__flash char parserCmdLU[]="LU";
__flash char parserInLU[]="%04d";
__flash char parserHlpLU[]="lift up the head";

// - LD -
__flash char parserCmdLD[]="LD";
__flash char parserInLD[]="%04d";
__flash char parserHlpLD[]="lift down the head";

// - PO -
__flash char parserCmdPO[]="PO";
__flash char parserOutPO[]="%s";
__flash char parserHlpPO[]="request of lifter position";

// - VN -
__flash char parserCmdVN[]="VN";
__flash char parserOutVN[]="V%02d";
__flash char parserHlpVN[]="get number of vessels: VXX";

// UP -
__flash char parserCmdPU[]="PU";
__flash char parserHlpPU[]="move up probe tubes";

// - PD -
__flash char parserCmdPD[]="PD";
__flash char parserInPD[]="%4d";
__flash char parserHlpPD[]="move down probe tubes";

// - PA -
__flash char parserCmdPA[]="PA";
__flash char parserOutPA[]="%s";
__flash char parserHlpPA[]="request of probe position";

// - PE -
__flash char parserCmdPE[]="PE";
__flash char parserOutPE[]="%01d";
__flash char parserHlpPE[]="request of probes (0 is none)";

// - SMP -
__flash char parserCmdSMP[]="SMP";
__flash char parserInSMP[]="%d";
__flash char parserHlpSMP[]="set sampler mode (0-off, 1-auto)";

// - MS -
__flash char parserCmdMS[]="MS";
__flash char parserOutMS[]="MS%3.1f";
__flash char parserHlpMS[]="request of stepper spindle step";

// - SMS -
__flash char parserCmdSMS[]="SMS";
__flash char parserInSMS[]="%f";
__flash char parserHlpSMS[]="set stepper spindle step in mm";

// - PB -
__flash char parserCmdPB[]="PB";
__flash char parserInPB[]="%f";
__flash char parserHlpPB[]="set filter offset";

// - PF -
__flash char parserCmdPF[]="PF";
__flash char parserOutPF[]="F%+05.1f";
__flash char parserHlpPF[]="request of filter offset";

// - TN -
__flash char parserCmdTN[]="TN";
__flash char parserOutTN[]="N%1d";
__flash char parserHlpTN[]="request number of temperature sensors";

// - T -
__flash char parserCmdT[]="T";
__flash char parserInT[]="%d";
__flash char parserOutT[]="I%1d%s";
__flash char parserHlpT[]="request temperature of sensor";

// - SHM -
__flash char parserCmdSHM[]="SHM";
__flash char parserInSHM[]="%1d";
__flash char parserHlpSHM[]="set head mode (0-low, 1-high)";

// - GHM -
__flash char parserCmdGHM[]="GHM";
__flash char parserOutGHM[]="H%1d";
__flash char parserHlpGHM[]="get head mode (0-low, 1-high)";

// - SBO -
__flash char parserCmdSBO[]="SBO";
__flash char parserInSBO[]="%1d %f";
__flash char parserHlpSBO[]="set method offset";

// - GBO -
__flash char parserCmdGBO[]="GBO";
__flash char parserInGBO[]="%1d";
__flash char parserOutGBO[]="B%4.1f";
__flash char parserHlpGBO[]="get method offset";

*/

/* 2019-2-28 星期四 18:29:27
// - SVN -
__flash char parserCmdSVN[]="SVN";
__flash char parserInSVN[]="%2d";
__flash char parserHlpSVN[]="set number of vessels";
*/

// - UPFW -
__flash char parserCmdUPFW[]="UPFW";

// - UPBUS -
__flash char parserCmdUPBUS[]="UPBUS";
__flash char parserInUPBUS[]="%X";

// - EKEY -
__flash char parserCmdEKEY[]="EKEY";

// - DT -
__flash char parserCmdDT[]="DT";

//2014/3/14 13:17:28
// - QDT -
//__flash char parserCmdQDT[]="QDT";

__flash char parserCmdNull[]="";

//------------------------------------------------------------------------------

__flash char parserMsgPrompt[] 										= "\r\n>";

__flash char parserErrOk[] 												= "O"; 							// OK，2019-3-1 星期五 15:55:01
__flash char parserErrNotConnected[] 							= "E1"; 						// 没有连接
__flash char parserErrWrongCommand[] 							= "E2"; 						// 指令错误
__flash char parserErrWrongValue[] 								= "E2"; 						// 错误的数值
__flash char parserErrWrongParamList[] 						= "E2"; 						// 错误的数据列表
__flash char parserErrMainMenu[] 									= "E4"; 						// 主菜单
__flash char parserErrWrongTemp[] 								= "E5"; 						// 温度方面的错误
__flash char parserErrCalibrationFailed[] 				= "E6"; 						// 校正失败
__flash char parserPrinterError[] 								= "E7"; 						// 打印错误
__flash char parserErrNotLogged[] 								= "E8"; 						// 没有登陆
__flash char parserErrWrongPassword[] 						= "E9"; 						// 密码错误
__flash char parserErrTestRunning[] 							= "E10"; 						// 测试正在运行
__flash char parserErrDHC[] 											= "E11"; 						// DHC（加热器或加热板）由问题，也可能是无法从DHC加热器获取温度
__flash char parserErrNoLog[] 										= "E12"; 						// 没有锁定（不是很确定）
__flash char parserErrNoDropper[] 								= "E14"; 						// 投放器出错（我们没有这个部分）
__flash char parserErrMotor[] 										= "E15"; 						// 电机运行出错
__flash char parserErrEncoder[] 									= "E16"; 						// 编码器出错

__flash char parserHelpSeparator[] 								=	" - ";
__flash char parserCRLF[] 												= "\r";   

__flash char tempErr[] 														= "-E--";

//------------------------------------------------------------------------------
__flash TParsedItem parserItems[PARSER_NUM_COMMANDS]=
  {
    {parserCmdVer,            NULL,                   parserOutVer,             parserHlpVer,         false},
    {parserCmdID,             NULL,                   parserOutID,              parserHlpID,          false},
    {parserCmdGI,             NULL,                   parserOutGI,              parserHlpGI,          false},
    {parserCmdSIN,            parserInSIN,            NULL,                     parserHlpSIN,         false}, 					// Gavin 20131224, from true to false
    {parserCmdPID,            NULL,                   parserOutPID,             parserHlpPID,         false},
    {parserCmdHelp,           NULL,                   NULL,                     parserHlpHelp,        false},
    {parserCmdPSW,            parserInPSW,            NULL,                     parserHlpPSW,         false},
		{parserCmdSDV,            NULL,                   NULL,                     parserHlpSDV,         false},
    {parserCmdBR,             parserInBR,             NULL,                     parserHlpBR,          false},
    {parserCmdSND,            parserInSND,            NULL,                     parserHlpSND,         false},
    {parserCmdALR,            parserInALR,            NULL,                     parserHlpALR,         false},
    {parserCmdPRN,            parserInPRN,            NULL,                     parserHlpPRN,         false},
    {parserCmdDTF,            parserInDTF,            NULL,                     parserHlpDTF,         false},
    {parserCmdCNT,            parserInCNT,            NULL,                     parserHlpCNT,         false},

/* 2019-2-28 星期四 16:24:06
    {parserCmdREP,            parserInREP,                   NULL,              parserHlpREP,         false},

    {parserCmdLOGO,           NULL,                   parserOutLOGO,            parserHlpLOGO,        false},
    {parserCmdSLOGO,          parserInSLOGO,          NULL,                     parserHlpSLOGO,       false}, 					// Gavin 20131224, from true to false
*/
    {parserCmdSD,             parserInSD,             NULL,                     parserHlpSD,          false},
    {parserCmdTD,             NULL,                   parserOutTD,              parserHlpTD,          false},
		{parserCmdCLPF,           parserInCLPF,           NULL,                     parserHlpCLPF,        false},
    {parserCmdCLPS,           parserInCLPS,           NULL,                     parserHlpCLPS,        false}, 					// Gavin 20131224, from true to false
    {parserCmdCLBR,           NULL,                   NULL,                     parserHlpCLBR,        false},
		{parserCmdCLHF,           parserInCLHF,           NULL,                     parserHlpCLHF,        false},
    {parserCmdCLHS,           parserInCLHS,           NULL,                     parserHlpCLHS,        false}, 					// Gavin 20131224, from true to false
		{parserCmdTR,             NULL,                   parserOutTR,              parserHlpTR,          false},
    {parserCmdTT,             parserInTT,             NULL,                     parserHlpTT,          false},
    {parserCmdTI,             NULL,                   parserOutTI,              parserHlpTI,          false},
    {parserCmdMINT,           NULL,                   parserOutMINT,            parserHlpMINT,        false},
    {parserCmdMAXT,           NULL,                   parserOutMAXT,            parserHlpMAXT,        false},
    {parserCmdSMINT,          parserInSMINT,          NULL,                     parserHlpSMINT,       false},
    {parserCmdSMAXT,          parserInSMAXT,          NULL,                     parserHlpSMAXT,       false},

    {parserCmdTO,             NULL,                   parserOutTO,              parserHlpTO,          false},
    {parserCmdTB,             parserInTB,             NULL,                     parserHlpTB,          false},

    {parserCmdSR,             NULL,                   parserOutSR,              parserHlpSR,          false},
    {parserCmdST,             parserInST,             NULL,                     parserHlpST,          false},
    {parserCmdSS,             NULL,                   NULL,                     parserHlpSS,          false},
    {parserCmdSA,             NULL,                   parserOutSA,              parserHlpSA,          false},
    {parserCmdSI,             NULL,                   parserOutSI,              parserHlpSI,          false},

    {parserCmdPPR,            NULL,                   parserOutPPR,             parserHlpPPR,         false}, 					// Gavin 20131224, from true to false
    
    // 2019-3-1 星期五 14:40:39
    //{parserCmdSPPR,           parserInSPPR,           NULL,                     parserHlpSPPR,        false}, 					// Gavin 20131224, from true to false

    {parserCmdADC,            NULL,                   parserOutADC,             parserHlpADC,         false}, 					// Gavin 20131224, from true to false
    
    {parserCmdUT,             NULL,                   parserOutUT,              parserHlpUT,          false},
    {parserCmdSUT,            parserInSUT,            NULL,                     parserHlpSUT,         false},

    {parserCmdPRD,            parserInPRD,            NULL,                     parserHlpPRD,         false},
    {parserCmdPRT,            parserInPRT,            NULL,                     parserHlpPRT,         false},
    {parserCmdGDT,            NULL,                   parserOutGDT,             parserHlpGDT,         false},
    
    {parserCmdGUT,            NULL,                   parserOutGUT,             parserHlpGUT,         false},
    {parserCmdRUT,            NULL,                   NULL,                     parserHlpRUT,         false}, 					// Gavin 20131224, from true to false

/* 2019-2-28 星期四 18:28:53
    {parserCmdGLH,            NULL,                   parserOutGLH,             parserHlpGLH,         false},
    {parserCmdGLR,            parserInGLR,            parserOutGLR,             parserHlpGLR,         false},
*/

		{parserCmdSM,             NULL,                   parserOutSM,              parserHlpSM,          false},
    {parserCmdTM,             parserInTM,             NULL,                     parserHlpTM,          false},
    {parserCmdRE,             NULL,                   NULL,                     parserHlpRE,          false},

/* 2019-2-28 星期四 16:24:55
    {parserCmdSTP,            parserInSTP,            NULL,                     parserHlpSTP,         true},
    {parserCmdCLAS1,          parserInCLAS1,          NULL,                     parserHlpCLAS1,       true}, 						// Gavin 20131224, from false to true
    {parserCmdCLAS2,          parserInCLAS2,          NULL,                     parserHlpCLAS2,       true},
		
    {parserCmdPH1,            NULL,                   NULL,                     parserHlpPH1,         true}, 						// Gavin 20131224, from false to true
    {parserCmdPH0,            NULL,                   NULL,                     parserHlpPH0,         true}, 						// Gavin 20131224, from false to true

    {parserCmdLU,             parserInLU,             NULL,                     parserHlpLU,          true}, 						// Gavin 20131224, from false to true
    {parserCmdLD,             parserInLD,             NULL,                     parserHlpLD,          true}, 						// Gavin 20131224, from false to true
    {parserCmdPO,             NULL,                   parserOutPO,              parserHlpPO,          true}, 						// Gavin 20131224, from false to true
		
    {parserCmdVN,             NULL,                   parserOutVN,              parserHlpVN,          true}, 						// Gavin 20131224, from false to true
    {parserCmdSVN,            parserInSVN,            NULL,                     parserHlpSVN,         true},

    {parserCmdPU,             NULL,                   NULL,                     parserHlpPU,          true}, 						// Gavin 20131224, from false to true
    {parserCmdPD,             parserInPD,             NULL,                     parserHlpPD,          true}, 						// Gavin 20131224, from false to true
    {parserCmdPA,             NULL,                   parserOutPA,              parserHlpPA,          true}, 						// Gavin 20131224, from false to true
    {parserCmdPE,             NULL,                   parserOutPE,              parserHlpPE,          true}, 						// Gavin 20131224, from false to true

    {parserCmdSMP,            parserInSMP,            NULL,                     parserHlpSMP,         true}, 						// Gavin 20131224, from false to true

    {parserCmdMS,             NULL,                   parserOutMS,              parserHlpMS,          true}, 						// Gavin 20131224, from false to true
    {parserCmdSMS,            parserInSMS,            NULL,                     parserHlpSMS,         true},

    {parserCmdPB,             parserInPB,             NULL,                     parserHlpPB,          true}, 						// Gavin 20131224, from false to true
    {parserCmdPF,             NULL,                   parserOutPF,              parserHlpPF,          true}, 						// Gavin 20131224, from false to true

    {parserCmdTN,             NULL,                   parserOutTN,              parserHlpTN,          true}, 						// Gavin 20131224, from false to true
    {parserCmdT,              parserInT,              parserOutT,               parserHlpT,           true}, 						// Gavin 20131224, from false to true

    {parserCmdSHM,            parserInSHM,            NULL,                     parserHlpSHM,         true}, 						// Gavin 20131224, from false to true
    {parserCmdGHM,            NULL,                   parserOutGHM,             parserHlpGHM,         true}, 						// Gavin 20131224, from false to true

    {parserCmdSBO,            parserInSBO,            NULL,                     parserHlpSBO,         true}, 						// Gavin 20131224, from false to true
    {parserCmdGBO,            parserInGBO,            parserOutGBO,             parserHlpGBO,         true}, 						// Gavin 20131224, from false to true
*/
    //{parserCmdSPID,           parserInSPID,           NULL,                     parserHlpSPID,        true},
    
    {parserCmdUPFW,           NULL,                   NULL,                     NULL,                 false},
    {parserCmdUPBUS,          parserInUPBUS,          NULL,                     NULL,                 false},

    {parserCmdEKEY,           NULL,                   NULL,                     NULL,                 true},

    {parserCmdDT,             NULL,                   NULL,                     NULL,                 false},

    {parserCmdNull,           NULL,                   NULL,                     NULL,                 false}

  };

static char buffer[20];

TBool secured=false;

TByte PARSER_Channel;

extern void Update();

//------------------------------------------------------------------------------
// toupper
//------------------------------------------------------------------------------
int my_toupper(int c)
{
  if ((c>=0x61)&&(c<=0x7A))
    return (c-0x20);
  else
    return c;
}

//------------------------------------------------------------------------------
// strupr
//------------------------------------------------------------------------------
char *strupr(char *s)
{
  char *str=s;

  while (*str)
  {
    *str=my_toupper(*str);
    str++;
  }
  return s;
}

//------------------------------------------------------------------------------
// ParserGetIndex  - search for an index in the string array
//------------------------------------------------------------------------------
/*signed char PARSER_GetIndex(TParsedItem __flash *items,char *buffer)
{
  signed char i;

  strupr(buffer);
  i=0;
  for (i=0;i<PARSER_NUM_COMMANDS;i++)
    if (0==strncmp_P(buffer,items[i].itemStr,strlen_P(items[i].itemStr)))
      return i;
  return -1;
}*/
// 2019-2-27 星期三 10:53:31
signed char PARSER_GetIndex(TParsedItem __flash *items,char *Tbuffer)
{
  signed char i;

  strupr(Tbuffer);
  i=0;
  for (i=0;i<PARSER_NUM_COMMANDS;i++)
    if (0==strncmp_P(Tbuffer,items[i].itemStr,strlen_P(items[i].itemStr)))
      return i;
  return -1;
}
//------------------------------------------------------------------------------
// putchar - redefine compiler putchar
//------------------------------------------------------------------------------
int putchar(int c)
{
  switch (PARSER_Channel)
  {
    case PORT_MODE_COM:
      RS232_SendChar(c);
      break;
    case PORT_MODE_USB:
      USB_SendChar(c);
      break;
  }
  return 0;
}

//------------------------------------------------------------------------------
// Parse - parser main
//------------------------------------------------------------------------------
void PARSER_Comm(TByte channel)
{
  signed char index;
  char *cmd;
  int pos;
  TWord intValue0, intValue1;
  char __flash *errMsg;
  TReal32 fltValue0;
  //char buffer[20];
  TByte cmd_length;

  if (channel>=PORT_MODE_LAST)
    return;
  
  PARSER_Channel=channel;

  //make an ASCIIZ string from the buffer
  switch (PARSER_Channel)
  {
    case PORT_MODE_USB:
      cmd=USBIBuffer;
      break;
    case PORT_MODE_COM:
      cmd=RS232_IBuffer;
      break;
  }

  //make an ASCIIZ string from the buffer
  pos=0;
  while (('\r'!=cmd[pos])&&('\n'!=cmd[pos])&&cmd[pos])
    pos++;
            
  if (pos>=BUFFER_SIZE)
    pos=BUFFER_SIZE-1;
  cmd[pos]='\0';
	
	//find command - skip blanks
	while (*cmd&&isspace(*cmd))
		cmd++;
	
	pos=0;
	while (cmd[pos])//&&(cmd[pos]!=0x20))
		pos++;
	
	cmd[pos]='\0';
	
	//check command and get its index
	index=PARSER_GetIndex(parserItems,cmd);
	
	if (index>=0)
	{
		cmd_length=strlen_P(parserItems[index].itemStr);
		if (strlen(cmd)>cmd_length)
		{
			cmd=&cmd[cmd_length];
		}
		else
			cmd=&cmd[pos];//strlen(cmd)+1];
	}
	errMsg=NULL;
	
	if (!globalDeviceStatus.enableUSB)
	{
		errMsg=parserErrMainMenu;
	}
	else if ((index==-1)||(index>=PARSER_CMD_NULL))
	{
		errMsg=parserErrWrongCommand;
	}
	else if ((!secured)&&(parserItems[index].secure))
	{
		errMsg=parserErrNotLogged;
	}
	else
	{
		switch (index)
		{
			case PARSER_CMD_FW: 							// 软件版本
			{
				strcpy_P(buffer, DEVICE_VERSION);
				printf_P(parserItems[PARSER_CMD_FW].outFmt, buffer);
			}break;
			case PARSER_CMD_ID: 							// 回复仪器ID号码
			{
				// v200-Q10.2, 2019-2-27 星期三 10:44:56
				//if (EEPROM1.FirmLogo!=DEVICE_LOGO_CALEVA)
				//{
					/*
					if (globalDeviceType!=DEVICE_DT1420)
					{
						if (globalDeviceType==DEVICE_DT620)
							printf_P(parserItems[PARSER_CMD_ID].outFmt, 6, (int)EEPROM1.NumVessels);
						else
							printf_P(parserItems[PARSER_CMD_ID].outFmt, 7, (int)EEPROM1.NumVessels);
					}
					else
					{
						printf_P(parserItems[PARSER_CMD_ID].outFmt, 14, 0);
					}
					*/
					
					// Gavin 20131224---------------------------------
					if (globalDeviceType==DEVICE_DT520)
						//printf_P(parserItems[PARSER_CMD_ID].outFmt, 5, (int)EEPROM1.NumVessels);
						printf_P(parserItems[PARSER_CMD_ID].outFmt, 1, (int)EEPROM1.NumVessels); 			// 2019-3-1 星期五 9:40:12
				/*}
				else
				{
					//if (globalDeviceType==DEVICE_DT620)
					// Gavin 20131224---------------------------------
					if (globalDeviceType==DEVICE_DT520)
						printf_P(devCalevaSt);
					else
						printf_P(devCalevaStPlus);
				}*/
			}break;
			/* 2019-2-28 星期四 18:59:20
			case PARSER_CMD_STP:
			{
				if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_STP].inFmt, &intValue0))
				{
					errMsg=parserErrWrongParamList;
					break;
				}
				
				if (intValue0>=DEVICE_LAST)
				{
					errMsg=parserErrWrongValue;
					break;
				}  
				
				EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DeviceType, (TByte *)&intValue0, sizeof(EEPROM1.DeviceType));
				globalDeviceType=EEPROM1.DeviceType;
				
				//if (globalDeviceType==DEVICE_DT620)
				// Gavin 20131224-----------------------------------
				if (globalDeviceType==DEVICE_DT520)
				{
					intValue0=HEAD_MODE_HIGH;
					EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Head, (TByte *)&intValue0, sizeof(EEPROM1.Head));
					globalDeviceStatus.Head=EEPROM1.Head;
					//globalDeviceStatus.sensorExtError=true;
					//globalDeviceStatus.installedExtADC=false;
				}
				else
				{
					globalDeviceStatus.installedExtADC=true;
				}  
				
				errMsg=parserErrOk;
			}break;
			*/
			case PARSER_CMD_GI: 							// 获取序列号
			{
				//read
				EEPROM_ReadString(buffer, EEPROM1.SerialNumber, sizeof(EEPROM1.SerialNumber));
				printf_P(parserItems[PARSER_CMD_GI].outFmt, buffer);
			}break;
			case PARSER_CMD_SIN: 							// 设置序列号
			{
				//store
				pos = strlen(cmd);
				if (pos>SERIAL_NUM_MAX_LEN)
					pos = SERIAL_NUM_MAX_LEN;
				
				cmd[pos]='\0';
				EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.SerialNumber, (TByte *)cmd, sizeof(EEPROM1.SerialNumber));
				errMsg=parserErrOk;
			}break;
			case PARSER_CMD_PID: 							// PCB号码
			{
				//read
				EEPROM_ReadString(buffer, PCBID, PCB_ID_MAX_LEN);
				printf_P(parserItems[PARSER_CMD_PID].outFmt, buffer);
			}break;
			case PARSER_CMD_HELP: 						// 帮助列表
			{
				for (pos=0;pos<PARSER_CMD_UPFW;pos++)
				{
					if ((!parserItems[pos].secure)||(secured))
					{
						printf_P(parserItems[pos].itemStr);
						printf_P(parserHelpSeparator);
						printf_P(parserItems[pos].help);
						printf_P(parserCRLF);
					}
				}
			}break;
			case PARSER_CMD_PSW: 							// 设置密码
			{
				if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_PSW].inFmt, &intValue0))
				{
					errMsg=parserErrWrongParamList;
					break;
				}
				
				if (intValue0!=EEPROM1.Password)
				{
					errMsg=parserErrWrongPassword;
					break;
				}
				
				secured=true;
				errMsg=parserErrOk;
			}break;
			case PARSER_CMD_SDV: 							// 返回默认设置值
			{
      	//return to initial settings
      	if (globalTestStatus.State==TEST_STOPPED)
      	{
      		EEPROM_LoadDefault();
      		TEMP_Init();
        	globalDeviceStatus.valueUpdate=true;
        	errMsg=parserErrOk;
        }
        else
        	errMsg=parserErrWrongCommand;
      }break;
      case PARSER_CMD_BR: 							// LED亮度设置
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_BR].inFmt,&intValue0))
      	{
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (!CTL_Brightness(intValue0))
        {
        	errMsg=parserErrWrongValue;
        }
        else
        {
        	errMsg=parserErrOk;
        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.LED, (TByte *)&intValue0, sizeof(EEPROM1.LED));
        }
      }break;
      case PARSER_CMD_SND: 							// 声音开关
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_SND].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        switch (intValue0)
        {
        	case SOUND_MODE_OFF:
        		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&intValue0, sizeof(EEPROM1.Sound));
        		SND_Off();
        		errMsg=parserErrOk;
        	break;
        	case SOUND_MODE_ON:
        		EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sound, (TByte *)&intValue0, sizeof(EEPROM1.Sound));
        		SND_On();
        		errMsg=parserErrOk;
        	break;
        	default:
        		errMsg=parserErrWrongValue;
        	break;
        }
      }break;
      case PARSER_CMD_ALR: 							// 警报声开关
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_ALR].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (intValue0>=ALARM_MODE_LAST)
        	errMsg=parserErrWrongValue;
        else
        {
        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Alarm, (TByte *)&intValue0, sizeof(EEPROM1.Alarm));
        	errMsg=parserErrOk;
        }
      }break;
      case PARSER_CMD_PRN: 							// 打印设置
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_PRN].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (intValue0>=PRINTER_MODE_LAST)
        	errMsg=parserErrWrongValue;
        else
        {
        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Print, (TByte *)&intValue0, sizeof(EEPROM1.Print));
        	errMsg=parserErrOk;
        }
      }break;        
      case PARSER_CMD_DTF: 							// 时间日期格式
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_DTF].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (intValue0>=DFORMAT_MODE_LAST)
        	errMsg=parserErrWrongValue;
        else
        {
        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.DFormat, (TByte *)&intValue0, sizeof(EEPROM1.DFormat));
        	errMsg=parserErrOk;
        }
      }break;        
      case PARSER_CMD_CNT: 							// 测试计时方式
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_CNT].inFmt, &intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TestCount, (TByte *)&intValue0, sizeof(EEPROM1.TestCount));
        errMsg=parserErrOk;
      }break;
      /* 2019-2-28 星期四 19:06:38
      case PARSER_CMD_REP: 							// 报告刷新时间
      {
      	if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_REP].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((intValue0>MAX_REPORT_INTERVAL)||(intValue0<MIN_REPORT_INTERVAL))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.ReportInterval, (TByte *)&intValue0, sizeof(EEPROM1.ReportInterval));
        errMsg=parserErrOk;
			}break;
			*/
			case PARSER_CMD_SD: 							// 设置测试时间
			{
       	TDWord Val;
       	if (2!=sscanf_P(cmd,parserItems[PARSER_CMD_SD].inFmt,&intValue0,&intValue1))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if ((globalTestStatus.State==TEST_RUNNING)||(globalTestStatus.State==TEST_PAUSED))
       	{
       		errMsg=parserErrTestRunning;
       		break;
       	}
       	
       	if ((intValue1>59)||(intValue0>99))
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	//eepromTime = (TDWord)intValue0*3600 + (TDWord)intValue1*60;
       	Val = (TDWord)intValue0*3600 + (TDWord)intValue1*60;
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Time, (TByte *)&Val, sizeof(EEPROM1.Time));
       	globalDeviceStatus.valueUpdate=true;
       	errMsg=parserErrOk;
			}break;
			case PARSER_CMD_TD: 							// 获取测试时间
			{
				TDWord tempT=globalTestStatus.Ticker;
				if (tempT>=EEPROM1.Time)
        	printf_P(parserItems[PARSER_CMD_TD].outFmt, 0, 0, 0);
        else
        	printf_P(parserItems[PARSER_CMD_TD].outFmt, (int)(EEPROM1.Time-tempT)/3600, (int)((EEPROM1.Time-tempT)%3600)/60, (int)((EEPROM1.Time-tempT)%60));
			}break;
			case PARSER_CMD_TR: 							// 获取加热器温度
			{
				//get temperature from DHC1200
        if (globalDeviceStatus.dhcFailure)
        {
        	errMsg=parserErrDHC;
        }
        else
        {  
        	if (DHC_GetTemp(&fltValue0))
        	{
        		printf_P(parserItems[PARSER_CMD_TR].outFmt, fltValue0);
        	}
        	else
        		errMsg=parserErrWrongValue;
        }
			}break;
			case PARSER_CMD_TT: 							// 获取目标设置温度
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_TT].inFmt, &fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((!globalDeviceStatus.remoteMode)&&((globalTestStatus.State==TEST_RUNNING)||(globalTestStatus.State==TEST_PAUSED)))
        {
        	errMsg=parserErrTestRunning;
        	break;
        }
        
        if ((TWord)fltValue0==0)
        {
        	if (DHC_Standby())
        		errMsg=parserErrOk;
        	else
        		errMsg=parserErrDHC;
        }
        else
        {
        	if ((fltValue0>(TReal32)EEPROM1.MaxTemp/10)||(fltValue0<(TReal32)EEPROM1.MinTemp/10))
        	{
        		errMsg=parserErrWrongValue;
        		break;
        	}        
        	
        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Temp, (TByte *)&fltValue0, sizeof(EEPROM1.Temp));
        	if (DHC_SetTemp(&fltValue0))
        		errMsg=parserErrOk;
        	else
        		errMsg=parserErrDHC;
        	
        	globalDeviceStatus.valueUpdate=true;
        }
			}break;
			case PARSER_CMD_TI: 							// 获取PT100温度
			{
				//2014/3/16 11:18:102014
				//get temperature from Pt100
        /*
        fltValue0=TEMP_GetValue(ADC_CHNL_EXTERNAL);
        if ((globalDeviceStatus.sensorExtError)||(!globalDeviceStatus.installedExtADC))
        	errMsg=parserErrNotConnected;
        else
        	printf_P(parserItems[PARSER_CMD_TI].outFmt, fltValue0);
        */
        
        // V200-Q1.5, 2019-2-24 星期日 20:26:02
        //if(globalDeviceType != DEVICE_QDT6)
				//{
					//get temperature from Pt100
	        fltValue0=TEMP_GetValue(ADC_CHNL_EXTERNAL);
	        if ((globalDeviceStatus.sensorExtError)||(!globalDeviceStatus.installedExtADC))
	        	errMsg=parserErrNotConnected;
	        else
	        	printf_P(parserItems[PARSER_CMD_TI].outFmt, fltValue0);
        //}
        //else
        //{
        //	errMsg=parserErrWrongCommand;
        //}
			}break;
			case PARSER_CMD_MINT: 						// 获取温控下限
			{
				fltValue0=(TReal32)EEPROM1.MinTemp/10;
        printf_P(parserItems[PARSER_CMD_MINT].outFmt, fltValue0);
			}break;
			case PARSER_CMD_MAXT: 						// 获取温控上限
			{
				fltValue0=(TReal32)EEPROM1.MaxTemp/10;
        printf_P(parserItems[PARSER_CMD_MAXT].outFmt, fltValue0);
			}break;
			case PARSER_CMD_SMINT: 						// 设置温控下限
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_SMINT].inFmt, &fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((fltValue0<MIN_TEMP_VALUE)||(fltValue0>MAX_TEMP_VALUE))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        intValue0=(TWord)(fltValue0*10);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MinTemp, (TByte *)&intValue0, sizeof(TWord));
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_SMAXT: 						// 设置温控上线
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_SMAXT].inFmt, &fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((fltValue0<MIN_TEMP_VALUE)||(fltValue0>MAX_TEMP_VALUE))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        intValue0=(TWord)(fltValue0*10);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MaxTemp, (TByte *)&intValue0, sizeof(TWord));
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_TO: 							// 获取当前温度偏置值
			{
				//read
        fltValue0=(TReal32)EEPROM1.TempOffset/10;
        printf_P(parserItems[PARSER_CMD_TO].outFmt, fltValue0);
			}break;
			case PARSER_CMD_TB: 							// 设置温度偏置值
			{
				if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_TB].inFmt, &fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (fltValue0>MAX_OFFSET_VALUE)
        	fltValue0=MAX_OFFSET_VALUE;
        
        if (fltValue0<MIN_OFFSET_VALUE)
        	fltValue0=MIN_OFFSET_VALUE;
        
        intValue0=(TWord)(fltValue0*10);
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempOffset, (TByte *)&intValue0, sizeof(TWord));
        //set heater correction
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_SR: 							// 获取目标电机转速
			{
				if (globalDeviceStatus.motorFailure)
        {
        	switch (globalDeviceStatus.motorFailure)
        	{
        		case MOTOR_ERROR_ENC:
        			errMsg=parserErrEncoder;
        		break;
        		case MOTOR_ERROR_CURR:
        		default:
        			errMsg=parserErrMotor;
        		break;
        	}  
        	break;
        }
        
        intValue0=(int)((MOTOR_GetCurrSpeed()+5)/10);
        printf_P(parserItems[PARSER_CMD_SR].outFmt, intValue0);
			}break;
			case PARSER_CMD_ST: 							// 设置目标电机转速
			{
				if (globalDeviceStatus.motorFailure)
        {
        	switch (globalDeviceStatus.motorFailure)
        	{
        		case MOTOR_ERROR_ENC:
        			errMsg=parserErrEncoder;
        		break;
        		case MOTOR_ERROR_CURR:
        		default:
        			errMsg=parserErrMotor;
        		break;
        	}
        	break;
        }
        
        if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_ST].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (intValue0==0)
        {
        	//stop test
        	//if ((globalTestStatus.State==TEST_RUNNING)||(globalTestStatus.State==TEST_PAUSED))
        	//{
        	globalTestStatus.State=TEST_STOPPED;
        	errMsg=parserErrOk;
        	//} else
        	//{
        	//  errMsg=parserErrWrongValue;
        	//}
        	break;
        }
        
        if ((intValue0>MAX_SPEED_VALUE)||(intValue0<MIN_SPEED_VALUE))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }        
        
        //enter remote mode
        globalDeviceStatus.remoteMode=true;
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Speed, (TByte *)&intValue0, sizeof(EEPROM1.Speed));
        globalDeviceStatus.valueUpdate=true;
        MOTOR_SetSpeed(intValue0);
        globalTestStatus.MinSpeed=globalTestStatus.MaxSpeed=intValue0;
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_SS: 							// 开始电机转动,如要停止把转速设置为0
			{
				if (globalDeviceStatus.motorFailure)
        {
        	switch (globalDeviceStatus.motorFailure)
        	{
        		case MOTOR_ERROR_ENC:
        			errMsg=parserErrEncoder;
        		break;
        		case MOTOR_ERROR_CURR:
        		default:
        			errMsg=parserErrMotor;
        		break;
        	}
        	break;
        }  
        
        //if (globalTestStatus.State==TEST_STOPPED)
        //{
        globalTestStatus.State=TEST_RUNNING;
        errMsg=parserErrOk;
        //}
        //} else errMsg=parserErrWrongValue;
			}break;
			case PARSER_CMD_SA: 							// 获取测试中的最大电机转速
			{
				intValue0=(int)(globalTestStatus.MaxSpeed+0.5);
        printf_P(parserItems[PARSER_CMD_SA].outFmt, intValue0);
			}break;
			case PARSER_CMD_SI: 							// 获取测试中的最小电机转速
			{
				intValue0=(int)(globalTestStatus.MinSpeed+0.5);
        printf_P(parserItems[PARSER_CMD_SI].outFmt, intValue0);
			}break;
			/* 2019-2-28 星期四 19:08:23
			case PARSER_CMD_PH1:
			{
				globalDeviceStatus.droppedMode=true;
        if (globalDeviceStatus.haveDropper)
        {
        	if (DROPPER_Open())
        	{
        		errMsg=parserErrOk;
        	}
        	else
        	{
        		errMsg=parserErrWrongCommand;
        	}
        }
        else
        	errMsg=parserErrNoDropper;
			}break;
			case PARSER_CMD_PH0:
			{
				if (globalDeviceStatus.haveDropper)
        {
        	if (DROPPER_Close())
        		errMsg=parserErrOk;
        	else
        		errMsg=parserErrWrongCommand;
        }
        else
        	errMsg=parserErrNoDropper;
			}break;
			case PARSER_CMD_LU:
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_LU].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        LIFTER_Up(intValue0);
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_LD:
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_LD].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        LIFTER_Down(intValue0);
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_PO:
			{
				sprintf_P(buffer, STATUS_LIFTER[LIFTER_GetStatus()]);
        printf(buffer);
			}break;
			*/
			case PARSER_CMD_CLPF: 						// 设置第一个PT100温度校准点
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_CLPF].inFmt,&fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (!TEMP_SetCalPoint(ADC_CHNL_EXTERNAL,0,&fltValue0))
        	errMsg=parserErrWrongValue;
        else
        	errMsg=parserErrOk;
			}break;
			case PARSER_CMD_CLPS: 						// 设置第二个PT100温度校准点
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_CLPS].inFmt,&fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (!TEMP_SetCalPoint(ADC_CHNL_EXTERNAL,1,&fltValue0))
        	errMsg=parserErrWrongValue;
        else
        	errMsg=parserErrOk;
			}break;
			case PARSER_CMD_CLBR: 						// 开始PT100的温度校准
			{
				if (!TEMP_Calibrate())
					errMsg=parserErrCalibrationFailed;
        else
        {
        	if (globalDeviceStatus.installedPrinter)
        	{
        		// save calibration date/time
        		if (RTC_GetDateTime(&curDTime))
        			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeSensorCal, (TByte *)&curDTime, sizeof(EEPROM1.timeSensorCal));
        	}
        	
        	errMsg=parserErrOk;
        }
			}break;
			case PARSER_CMD_CLHF: 						// 开始加热器第一个温度点校准
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_CLHF].inFmt,&fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (!DHC_FirstPCal(&fltValue0))
        	errMsg=parserErrWrongValue;
        else
        {
        	if (globalDeviceStatus.installedPrinter)
        	{
        		// save calibration date/time
        		if (RTC_GetDateTime(&curDTime))
        			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeHeaterCal, (TByte *)&curDTime, sizeof(EEPROM1.timeHeaterCal));
        	}
        }
        
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_CLHS: 						// 开始加热器第二个温度点校准
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_CLHS].inFmt,&fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (!DHC_SecondPCal(&fltValue0))
        	errMsg=parserErrWrongValue;
        else
        {
        	if (globalDeviceStatus.installedPrinter)
        	{
        		// save calibration date/time
        		if (RTC_GetDateTime(&curDTime))
        			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeHeaterCal, (TByte *)&curDTime, sizeof(EEPROM1.timeHeaterCal));
        	}
        }
        
        errMsg=parserErrOk;
			}break;
			/* 2019-2-28 星期四 19:09:22
			case PARSER_CMD_CLAS1: 						// 
			{
				if (2!=sscanf_P(cmd,parserItems[PARSER_CMD_CLAS1].inFmt, &intValue0, &fltValue0))
				{
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (globalDeviceStatus.samplerFailure)
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        if (!SAMPLER_Calibrate(0, &fltValue0, intValue0))
        	errMsg=parserErrWrongValue;
        else
        {
        	if (globalDeviceStatus.installedPrinter)
        	{
        		// save calibration date/time
        		if (RTC_GetDateTime(&curDTime))
        			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeSamplerCal, (TByte *)&curDTime, sizeof(EEPROM1.timeSamplerCal));
        	}
        }
        
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_CLAS2:
			{
				if (2!=sscanf_P(cmd,parserItems[PARSER_CMD_CLAS2].inFmt, &intValue0, &fltValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if (globalDeviceStatus.samplerFailure)
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        if (!SAMPLER_Calibrate(1, &fltValue0, intValue0))
        	errMsg=parserErrWrongValue;
        else
        {
        	if (globalDeviceStatus.installedPrinter)
        	{
        		// save calibration date/time
        		if (RTC_GetDateTime(&curDTime))
        			EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.timeSamplerCal, (TByte *)&curDTime, sizeof(EEPROM1.timeSamplerCal));
        	}
        }
        
        errMsg=parserErrOk;
			}break;
			*/
			case PARSER_CMD_PPR: 							// 获取电机PPR参数
			{
				intValue0=EEPROM1.PulsesPerRotat;
        printf_P(parserItems[PARSER_CMD_PPR].outFmt,intValue0);
			}break;
			/* 2019-3-1 星期五 14:42:38
			case PARSER_CMD_SPPR: 						// 设置电机PPR参数
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_SPPR].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((intValue0>1000)||(intValue0<100))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }
        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.PulsesPerRotat, (TByte *)&intValue0, sizeof(EEPROM1.PulsesPerRotat));
        errMsg=parserErrOk;
			}break;
			*/
			case PARSER_CMD_ADC: 							// ?
			{
				printf_P(parserItems[PARSER_CMD_ADC].outFmt,ADC_GetValue());
			}break;
			case PARSER_CMD_UT: 							// 获取温度显示的刷新时间
			{
				intValue0=EEPROM1.TempUpdateInterval;
				printf_P(parserItems[PARSER_CMD_UT].outFmt,intValue0);
			}break;
			case PARSER_CMD_SUT: 							// 设置温度显示的刷新时间
			{
				if (1!=sscanf_P(cmd,parserItems[PARSER_CMD_SUT].inFmt,&intValue0))
        {
        	errMsg=parserErrWrongParamList;
        	break;
        }
        
        if ((intValue0>MAX_UPDATE_INTERVAL)||(intValue0<MIN_UPDATE_INTERVAL))
        {
        	errMsg=parserErrWrongValue;
        	break;
        }                                       
        
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.TempUpdateInterval, (TByte *)&intValue0, sizeof(EEPROM1.TempUpdateInterval));
        errMsg=parserErrOk;
			}break;
			case PARSER_CMD_PRD: 							// 设置当前日期
			{
				if (globalDeviceStatus.installedPrinter)
        {
        	TTimeDate dt;
        	int day,month,year;
        	
        	if (3!=sscanf_P(cmd,parserItems[PARSER_CMD_PRD].inFmt,&day,&month,&year))
        	{
        		errMsg=parserErrWrongParamList;
        		break;
        	}
        	
        	if (RTC_GetDateTime(&dt))
        	{
        		dt.Day=day;
        		dt.Month=month;
        		dt.Year=year;
        		if (RTC_SetDateTime(dt))
        			errMsg=parserErrOk;
        		else 
        			errMsg=parserPrinterError;
        	}
        	else
        		errMsg=parserPrinterError;
        }
        else
        	errMsg=parserErrWrongCommand;
			}break;
			case PARSER_CMD_PRT: 							// 设置当前时间
			{
				if (globalDeviceStatus.installedPrinter)
        {
        	TTimeDate dt;
        	int hour,min;
        	
        	if (2!=sscanf_P(cmd,parserItems[PARSER_CMD_PRT].inFmt,&hour,&min))
        	{
        		errMsg=parserErrWrongParamList;
        		break;
        	}
        	
        	if (RTC_GetDateTime(&dt))
        	{
        		dt.Hour=hour;
        		dt.Minute=min;
        		dt.Second=0;
        		
        		if (RTC_SetDateTime(dt))
        			errMsg=parserErrOk;
        		else
        			errMsg=parserPrinterError;
        	}
        	else
        		errMsg=parserPrinterError;
        }
        else
        	errMsg=parserErrWrongCommand;
			}break;
			case PARSER_CMD_GDT: 							// 获取当前时间和日期
			{
				if (globalDeviceStatus.installedPrinter)
        {
        	TTimeDate dt;
        	
        	if (!RTC_GetDateTime(&dt))
        		errMsg=parserPrinterError;
        	else
        	{
        		printf_P(parserItems[PARSER_CMD_GDT].outFmt,dt.Day, dt.Month, dt.Year, dt.Hour, dt.Minute);
        	}
        }
        else
        	errMsg=parserErrWrongCommand;
			}break;
//			2019-2-28 星期四 19:10:28
//			case PARSER_CMD_LOGO: 						// 获取软件LOGO
//			{
//				//read
//        //EEPROM_ReadString(buffer, EEPROM1.FirmLogo, sizeof(EEPROM1.FirmLogo));
//        strcpy_P(buffer, DEVICE_LOGO[EEPROM1.FirmLogo]);
//        printf_P(parserItems[PARSER_CMD_LOGO].outFmt, buffer);
//			}break;
//			case PARSER_CMD_SLOGO: 						// 设置软件LOGO
//			{
//				//te /r/n from the string
//        /*pos = strlen(cmd);
//        if (pos>LOGO_MAX_LEN)
//        	pos=LOGO_MAX_LEN;
//        cmd[pos]='\0';
//        //store                              
//        //EEPROM_WriteString(eepromFirmLogo,cmd,sizeof(eepromFirmLogo));
//        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FirmLogo, (TByte *)cmd, sizeof(EEPROM1.FirmLogo));
//        errMsg=parserErrOk;*/
//        
//        if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_SLOGO].inFmt, &intValue0))
//        {
//        	errMsg=parserErrWrongParamList;
//        	break;
//        }
//        
//        if (intValue0>=DEVICE_LOGO_LAST)
//        	errMsg=parserErrWrongValue;
//        else
//        {
//        	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FirmLogo, (TByte *)&intValue0, sizeof(EEPROM1.FirmLogo));
//        	errMsg=parserErrOk;
//        }
//			}break;
//			
			case PARSER_CMD_GUT: 							// 获取仪器使用时间
			{
				sprintf_P(buffer, parserItems[PARSER_CMD_GUT].outFmt, EEPROM1.UsageCounter/3600);
				printf(buffer);
			}break;
			case PARSER_CMD_RUT: 							// 重置仪器使用时间
			{
				TDWord Val=0;
				
				//eepromUsageCounter=0;
        EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.UsageCounter, (TByte *)&Val, sizeof(EEPROM1.UsageCounter));
        errMsg=parserErrOk;
			}break;
//			2019-2-28 星期四 19:12:12
//			case PARSER_CMD_VN: 							// 
//			{
//       	sprintf_P(buffer, parserItems[PARSER_CMD_VN].outFmt, EEPROM1.NumVessels);
//       	printf(buffer);
//			}break;
//			case PARSER_CMD_SVN:
//			{
//       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_SVN].inFmt, &intValue0))
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	if ((intValue0<MIN_VESSELS_NUMBER)||(intValue0>MAX_VESSELS_NUMBER))
//       	{
//       		errMsg=parserErrWrongValue;
//       		break;
//       	}
//       	
//       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.NumVessels, (TByte *)&intValue0, sizeof(EEPROM1.NumVessels));
//       	errMsg=parserErrOk;
//			}break;
//			case PARSER_CMD_GLH: 							// ?
//			{
//      	FRAM_Do(MEM_READ_HEADER, 0);
//        //print log header
//        if (FHeader.Status==RSTS_NO)
//        {
//        	errMsg=parserErrNoLog;
//        	break;
//        }
//        
//        sprintf_P(buffer, parserItems[PARSER_CMD_GLH].outFmt, (int)FHeader.Number, (int)(FHeader.Interval/60), (TReal32)FHeader.MinSpeed/10, (TReal32)FHeader.MaxSpeed/10, (TReal32)FHeader.MinTemp/10, (TReal32)FHeader.MaxTemp/10);
//        printf(buffer);
//			}break;
//			case PARSER_CMD_GLR: 							// ?
//			{
//       	//TByte i;
//       	char str[10];
//       	
//       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_GLR].inFmt, &intValue0))
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	/*if (intValue0>MAX_REPORT_NUMBER)
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}*/
//       	
//       	if (!intValue0)
//       	{
//       		errMsg=parserErrNoLog;
//       		break;
//       	}
//       	
//       	FRAM_Do(MEM_READ_HEADER, 0);
//       	intValue0--;
//       	if (!FRAM_GetRecord(intValue0))
//       	{
//       		errMsg=parserErrNoLog;
//       		break;
//       	}
//       	
//       	if (FReport.MinTemp==TEMP_ERROR)
//       		strcpy_P(str, tempError);
//       	else
//       		sprintf_P(str, tempReportFmt, (TReal32)FReport.MinTemp/10);
//       	
//       	/*for (i=0; i<MAX_SENSORS_NUM; i++)
//       	{
//       		if (FReport.SampT[i]==TEMP_ERROR)
//       			strcpy_P(CALIBRATE_String[i], tempError);
//       		else
//       			sprintf_P(CALIBRATE_String[i], tempReportFmt, (TReal32)FReport.SampT[i]/100);
//       	}*/
//       	//print log record
//       	
//       	sprintf_P(buffer, parserItems[PARSER_CMD_GLR].outFmt, str, (TReal32)FReport.MinSpeed/10);//, CALIBRATE_String[0], CALIBRATE_String[1], CALIBRATE_String[2], CALIBRATE_String[3], CALIBRATE_String[4], CALIBRATE_String[5], CALIBRATE_String[6], CALIBRATE_String[7]);
//       	printf(buffer);
//			}break;
//			case PARSER_CMD_PU:
//			{
//				//if (globalDeviceType>=DEVICE_DT724)
//        if (globalDeviceStatus.haveStepper)
//        {  
//        	//fltValue0=0.0;
//        	//STEPPER_SetDistance(&fltValue0);
//        	if (globalDeviceStatus.Head==HEAD_MODE_LOW)
//        		STEPPER_Start(true, true);
//        }
//        
//        errMsg=parserErrOk;
//			}break;
//			case PARSER_CMD_PD:
//			{
//       	//TByte i=0;
//       	
//       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_PD].inFmt, &intValue0))
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	/*while (i<DEPTH_MODE_LAST)
//       	{
//       		if (PROBE_VOLUME[i]==intValue0)
//       			break;
//       		
//       		i++;
//       	}
//       	
//       	if (i>=DEPTH_MODE_LAST)
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}*/
//       	
//       	if (intValue0>PROBE_VOLUME[0])
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	if ((globalDeviceStatus.haveStepper)&&(globalDeviceStatus.Head==HEAD_MODE_LOW))
//       	{
//       		//STEPPER_CalcDistance(i);
//       		STEPPER_CalcDistanceNew(&intValue0);
//       		STEPPER_Start(false, false);
//       	}
//       	
//       	errMsg=parserErrOk;
//			}break;
//			case PARSER_CMD_PA:
//			{
//       	sprintf_P(buffer, STATUS_PROBE[STEPPER_GetStatus()]);
//       	printf(buffer);
//			}break;
//			case PARSER_CMD_PE:
//			{
//       	if (globalDeviceStatus.haveStepper)
//       		sprintf_P(buffer, parserItems[PARSER_CMD_PE].outFmt, EEPROM1.NumVessels);
//       	else
//       		sprintf_P(buffer, parserItems[PARSER_CMD_PE].outFmt, 0);
//       	
//       	printf(buffer);
//			}break;
//			case PARSER_CMD_MS:
//			{
//       	sprintf_P(buffer, parserItems[PARSER_CMD_MS].outFmt, (TReal32)EEPROM1.MotorStepMM/10);
//       	printf(buffer);
//			}break;
//			case PARSER_CMD_SMS:
//			{
//       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_SMS].inFmt, &fltValue0))
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	if ((fltValue0>=10)||(fltValue0<1))
//       	{
//       		errMsg=parserErrWrongValue;
//       		break;
//       	}
//       	
//       	intValue0=(TWord)(fltValue0*10);
//       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MotorStepMM, (TByte *)&intValue0, sizeof(EEPROM1.MotorStepMM));
//       	errMsg=parserErrOk;
//			}break;
//			
			case PARSER_CMD_SM: 							// 获取测试方法
			{
       	sprintf_P(buffer, parserItems[PARSER_CMD_SM].outFmt, EEPROM1.Method);
       	printf(buffer);
			}break;
			case PARSER_CMD_TM: 							// 设置测试方法
			{
       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_TM].inFmt, &intValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if (intValue0>=METH_MODE_LAST)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Method, (TByte *)&intValue0, sizeof(EEPROM1.Method));
       	errMsg=parserErrOk;
			}break;
			/* 2019-2-28 星期四 18:58:32
			case PARSER_CMD_SMP:
			{
       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_SMP].inFmt, &intValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if (intValue0>=SAMPLER_MODE_LAST)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Sams, (TByte *)&intValue0, sizeof(EEPROM1.Sams));
       	errMsg=parserErrOk;
			}break;
			*/
			/* 2019-2-28 星期四 18:58:29
			case PARSER_CMD_PB:
			{
       	signed char temp;
       	
       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_PB].inFmt, &fltValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if ((fltValue0<MIN_FILTER_VALUE)||(fltValue0>MAX_FILTER_VALUE))
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	temp=(signed char)(fltValue0*10);
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.FilterOffset, (TByte *)&temp, sizeof(EEPROM1.FilterOffset));
       	errMsg=parserErrOk;
			}break;
			*/
			/* 2019-2-28 星期四 18:56:50
			case PARSER_CMD_PF:
			{
				sprintf_P(buffer, parserItems[PARSER_CMD_PF].outFmt, (TReal32)EEPROM1.FilterOffset/10);
        printf(buffer);
			}break;
			*/
			/* 2019-2-28 星期四 18:55:59
			case PARSER_CMD_TN:
			{
       	if (globalDeviceStatus.samplerFailure)
       		sprintf_P(buffer, parserItems[PARSER_CMD_TN].outFmt, 0);
       	else
       		sprintf_P(buffer, parserItems[PARSER_CMD_TN].outFmt, 9);
       	
       	printf(buffer);
			}break;
			*/
// 2019-2-28 星期四 18:56:08
//			case PARSER_CMD_T:
//			{
//       	char str[10];
//       	
//       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_T].inFmt, &intValue0))
//       	{
//       		errMsg=parserErrWrongParamList;
//       		break;
//       	}
//       	
//       	if (intValue0>EEPROM1.NumSensors)
//       	{
//       		errMsg=parserErrWrongValue;
//       		break;
//       	}
//       	
//       	//get temperature from autosampler board
//       	if (globalDeviceStatus.samplerFailure)
//       	{
//       		strcpy_P(str, tempErr);
//       	}
//       	else
//       	{
//       		if (SAMPLER_Temp[intValue0-1]==TEMP_ERROR)
//       		{
//       			strcpy_P(str, tempErr);
//       		}
//       		else
//       		{
//       			sprintf_P(str, tempReportFmt, (TReal32)SAMPLER_Temp[intValue0-1]/100);
//       		}
//       		
//       		/*fltValue0=(TReal32)SAMPLER_Temp[0]/100;
//       		if (intValue0-1)
//       		{
//       			if (intValue0%2)
//       				fltValue0+=0.1;
//       			else
//       				fltValue0-=0.1;
//       		}
//       		
//       		sprintf_P(str, tempFmt, fltValue0);*/
//       	}
//       	
//       	sprintf_P(buffer, parserItems[PARSER_CMD_T].outFmt, intValue0, str);//fltValue0);
//       	printf(buffer);
//			}break;
//			
			/* 2019-2-28 星期四 18:53:31
			case PARSER_CMD_SHM:
			{
       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_SHM].inFmt, &intValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if (intValue0>=HEAD_MODE_LAST)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	//if (globalDeviceType==DEVICE_DT620)
       	// Gavin 20131224-----------------------------------
       	if (globalDeviceType==DEVICE_DT520)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.Head, (TByte *)&intValue0, sizeof(EEPROM1.Head));
       	globalDeviceStatus.Head=EEPROM1.Head;
       	errMsg=parserErrOk;
			}break;
			*/
			/* 2019-2-28 星期四 19:13:13
			case PARSER_CMD_GHM:
			{
       	sprintf_P(buffer, parserItems[PARSER_CMD_GHM].outFmt, EEPROM1.Head);
       	printf(buffer);
			}break;
			case PARSER_CMD_SBO:
			{
       	if (2!=sscanf_P(cmd, parserItems[PARSER_CMD_SBO].inFmt, &intValue0, &fltValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if (intValue0>=METH_MODE_LAST)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	if ((fltValue0>(TReal32)METHOD_MAX_OFFS[intValue0]/10)||(fltValue0<0))
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}
       	
       	intValue1=(TWord)(fltValue0*10);
       	EEPROM_WriteVar((TByte __eeprom *)&EEPROM1.MethodOffset[intValue0], (TByte *)&intValue1, sizeof(TWord));
       	errMsg=parserErrOk;
			}break;
			case PARSER_CMD_GBO:
			{
       	if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_GBO].inFmt, &intValue0))
       	{
       		errMsg=parserErrWrongParamList;
       		break;
       	}
       	
       	if (intValue0>=METH_MODE_LAST)
       	{
       		errMsg=parserErrWrongValue;
       		break;
       	}  
       	
       	sprintf_P(buffer, parserItems[PARSER_CMD_GBO].outFmt, (TReal32)EEPROM1.MethodOffset[intValue0]/10);
       	printf(buffer);
			}break;
			*/
			case PARSER_CMD_RE: 							// 退出控制模式
			{
       	//exit remote mode
       	// pos是命令字符串的指针，2019-3-1 星期五 15:02:58
       	if(pos <= 2)
				{
       		globalDeviceStatus.remoteMode=false;
       		errMsg=parserErrOk;
       	}
       	else 									// 命令“RE”后面还有字符就不是“RE”命令
       	{
       		errMsg=parserErrWrongCommand;
       	}
			}break;
			case PARSER_CMD_UPFW:
			{
       	printf_P(parserErrOk);
       	printf_P(parserCRLF);
       	LED_SwitchOff();
       	__delay_cycles(XTAL/2);
       	__disable_interrupt();
       	//( *( void( * )()) 0xf800 )();
       	Update();
			}break;
			case PARSER_CMD_UPBUS:
			{
				//move to updater subroutine for RS485 bus
        if (1!=sscanf_P(cmd, parserItems[PARSER_CMD_UPBUS].inFmt, &intValue0))
        {
          errMsg = parserErrWrongParamList;
          break;
        }
        
        // 判断打印机是否连接
				if (!globalDeviceStatus.installedPrinter)
					//return false;
					break;
				
        LED_Store();
        LED_SwitchOff();
        UPDATER_BusRun(intValue0);
        if (intValue0==DEVICE_ID_DTAS)
        {
          SAMPLER_Init();
          SAMPLER_SetActive(true);
        }
        LED_Restore();
			}break;
			case PARSER_CMD_EKEY:
			{
       	TByte len;
       	
       	len=0;
       	switch (EKEY_Parser(cmd, buffer, &len))
       	{
       		case EKEY_ANSWER_PRINT: 
       			//printf(buffer);
       			USB_SendString(buffer, len);
       		break;
       		case EKEY_ANSWER_OK:
       			errMsg=parserErrOk;
       		break;
       		case EKEY_ANSWER_ERROR:
       			errMsg=parserErrWrongValue;  
       		break;
       	}
			}break;
			case PARSER_CMD_DT:
			{
       	errMsg=parserErrOk;
			}break;
			case PARSER_CMD_NULL:
			default:
				errMsg=parserErrWrongCommand;
			break;
		}
	}
	
	if (errMsg)
		printf_P(errMsg);
	
	printf_P(parserCRLF);
	memset(cmd, 0, BUFFER_SIZE);
}
