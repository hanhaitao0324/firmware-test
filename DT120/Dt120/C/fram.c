// FRAM Memory
#include "pins.h"
#include "platform.h"
#include "spi.h"
#include "const.h"
#include "fram.h"

TFHeader FHeader;
TFReport FReport;

volatile TFOperation FRAM_Operate={MEM_NOOP, 0};

//TBool FRAM_WrHeader();//const void* Data);

//------------------------------------------------------------------------------
// Init FRAM memory
//------------------------------------------------------------------------------
TBool FRAM_Init()												
{
  FHeader.Number=0;
  //FHeader.StartTime=0;
  //FHeader.Time=0;
  FHeader.Speed=0;
  FHeader.Temp=0;
  FHeader.Interval=0;
  FHeader.MinSpeed=0;
  FHeader.MaxSpeed=0;
  FHeader.MinTemp=0;
  FHeader.MaxTemp=0;
  FHeader.Method=METH_MODE_BSKT;
  FHeader.Volume=VOLUME_MODE_1000;
  FHeader.Head=HEAD_MODE_LOW;
  FHeader.TempOffset=0;
  FHeader.RunTime=0;
  FHeader.Status=0;
  //FHeader.Multiplex=1;
  return true;
}

//------------------------------------------------------------------------------
// Read Len bytes from address Addr to buffer Smb
//------------------------------------------------------------------------------
TBool FRAM_Read(void* Data, TWord Addr, TByte Len)
{
  TByte ix;
  TByte* vData = (TByte*)Data;
	
  if (Len==0) return true;

  SPI_Select(SPI_FRAM);
  SPI_IOByte(FRAM_READ);
  SPI_IOByte(HIBYTE(Addr));
  SPI_IOByte(LOBYTE(Addr));
  for (ix = 0; ix<Len; ix++)
  {
    *vData++ = SPI_IOByte(SPI_EMPTY_BYTE);
  }
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// write Len bytes from buffer Data to address Addr
//------------------------------------------------------------------------------
TBool FRAM_Write(const void* Data, TWord Addr, TByte Len)	
{
  TByte ix;
  TByte* vData = (TByte*)Data;
	
  if (Len==0) return true;
	
  SPI_Select(SPI_FRAM);
  SPI_IOByte(FRAM_WRITE_EN);
  SPI_Select(SPI_NONE);
  
  SPI_Select(SPI_FRAM);
  SPI_IOByte(FRAM_WRITE);
  SPI_IOByte(HIBYTE(Addr));
  SPI_IOByte(LOBYTE(Addr));
  for (ix = 0; ix<Len; ix++)
  {
    SPI_IOByte(*vData++);
  }
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// clear Len bytes from starting address Addr
//------------------------------------------------------------------------------
TBool FRAM_Clear(TWord Addr, TWord Len)
{
  TWord ix;
	
  if (Len==0) return true;
	
  SPI_Select(SPI_FRAM);
  SPI_IOByte(FRAM_WRITE_EN);
  SPI_Select(SPI_NONE);
  
  SPI_Select(SPI_FRAM);
  SPI_IOByte(FRAM_WRITE);
  SPI_IOByte(HIBYTE(Addr));
  SPI_IOByte(LOBYTE(Addr));
  for (ix = 0; ix<Len; ix++)
  {
    SPI_IOByte(0);
  }
  SPI_Select(SPI_NONE);
  return true;
}

//------------------------------------------------------------------------------
// write report header to FRAM
//------------------------------------------------------------------------------
TBool FRAM_WrHeader()//const void* Data)
{
  //TByte* vData = (TByte*)Data;
  
  //SPI_Busy=true;

  FRAM_Write(&FHeader, FRAM_ADDR, FRAM_HEADER_SIZE);
  //SPI_Busy=false;
  return true;
}

//------------------------------------------------------------------------------
// read report header from FRAM
//------------------------------------------------------------------------------
TBool FRAM_RdHeader()//void* Data)
{
  //TByte* vData = (TByte*)Data;

  //SPI_Busy=true;

  FRAM_Read(&FHeader, FRAM_ADDR, FRAM_HEADER_SIZE);
  //SPI_Busy=false;
  return true;
}

//------------------------------------------------------------------------------
// clear report header
//------------------------------------------------------------------------------
TBool FRAM_ClrHeader(TFHeader *Header)
{
  (*Header).Number = 0;
  //(*Header).StartTime = 0;
  (*Header).Interval = 0;
  (*Header).Status = RSTS_NO;
  
  if (FRAM_Clear(FRAM_ADDR, FRAM_HEADER_SIZE))
    return true;
  else
    return false;
}

//------------------------------------------------------------------------------
// read report Num from FRAM
//------------------------------------------------------------------------------
TBool FRAM_RdReport(TWord Num)
{
  TBool Result;

  if(Num >= MAX_REPORT_NUMBER) return false;
  //SPI_Busy=true;
  Result = FRAM_Read(&FReport, FRAM_ADDR + FRAM_HEADER_SIZE + FRAM_REPORT_SIZE * Num, FRAM_REPORT_SIZE);
  //SPI_Busy=false;
  return Result;
}

//------------------------------------------------------------------------------
// write report Num from FRAM
//------------------------------------------------------------------------------
TBool FRAM_WrReport(TWord Num)
{
  TBool Result;

  if(Num >= MAX_REPORT_NUMBER) return false;
  //SPI_Busy=true;
  Result = FRAM_Write(&FReport, FRAM_ADDR + FRAM_HEADER_SIZE + FRAM_REPORT_SIZE * Num, FRAM_REPORT_SIZE);
  //SPI_Busy=false;
  return Result;
}

//------------------------------------------------------------------------------
// memory ready to write 
//------------------------------------------------------------------------------
void FRAM_IntHandler(void)
{
  if (FRAM_Operate.Type)
  {
    switch (FRAM_Operate.Type)
    {
      case MEM_READ:
        FRAM_RdReport(FRAM_Operate.Number);
        break;
      case MEM_WRITE:
        FRAM_WrReport(FRAM_Operate.Number);
        break;
      case MEM_READ_HEADER:
        FRAM_RdHeader();
        break;
      case MEM_WRITE_HEADER:
        FRAM_WrHeader();
        break;
    }
    FRAM_Operate.Type=MEM_NOOP;
  }
}

//------------------------------------------------------------------------------
// perform memory operation 
//------------------------------------------------------------------------------
void FRAM_Do(TByte Type, TWord Number)
{
  FRAM_Operate.Type=Type;
  FRAM_Operate.Number=Number;
  while (FRAM_Operate.Type!=MEM_NOOP);
}

//------------------------------------------------------------------------------
// read virtual record from the memory (used to save memory size) 
//------------------------------------------------------------------------------
TBool FRAM_GetRecord(TWord Number)
{
  //TByte offset, i;
  //TFReport FRepOld;
  //TBool calculate=false;
  
  if ((FHeader.Status==RSTS_NO)||(Number>=FHeader.Number))
    return false;
  
  /*if (Number)
  {
    if (!((Number-1)%FHeader.Multiplex))
      Number=(Number-1)/FHeader.Multiplex+1;
    else
      if (Number>(((FHeader.Number-1)/FHeader.Multiplex)*FHeader.Multiplex+1))
        Number=(Number-1)/FHeader.Multiplex+1+((Number-1)%FHeader.Multiplex);
      else
      {
        //value should be calculated
        offset=(Number-1)%FHeader.Multiplex;
        Number=(Number-1)/FHeader.Multiplex+1;
        calculate=true;
      }
  }*/
  FRAM_Do(MEM_READ, Number);
  /*if (calculate)
  {
    memcpy(&FRepOld, &FReport, sizeof(TFReport));
    Mem_Do(MEM_READ, ++Number);
    if (FReport.Speed>FRepOld.Speed)
      FReport.Speed=FRepOld.Speed+(FReport.Speed-FRepOld.Speed)*offset/FHeader.Multiplex;
    else
      FReport.Speed=FRepOld.Speed+(FRepOld.Speed-FReport.Speed)*offset/FHeader.Multiplex;
    if (FReport.Temp==TEMP_ERROR)
      FReport.Temp=FRepOld.Temp;
    else 
    if (FRepOld.Temp!=TEMP_ERROR)
      if (FReport.Temp>FRepOld.Temp)
        FReport.Temp=FRepOld.Temp+(FReport.Temp-FRepOld.Temp)*offset/FHeader.Multiplex;
      else
        FReport.Temp=FRepOld.Temp+(FRepOld.Temp-FReport.Temp)*offset/FHeader.Multiplex;
    for (i=0; i<MAX_SENSORS_NUM; i++)
    {
      if (FReport.SampT[i]==TEMP_ERROR)
        FReport.SampT[i]=FRepOld.SampT[i];
      else 
      if (FRepOld.SampT[i]!=TEMP_ERROR)
        if (FReport.SampT[i]>FRepOld.SampT[i])
          FReport.SampT[i]=FRepOld.SampT[i]+(FReport.SampT[i]-FRepOld.SampT[i])*offset/FHeader.Multiplex;
        else
          FReport.SampT[i]=FRepOld.SampT[i]+(FRepOld.SampT[i]-FReport.SampT[i])*offset/FHeader.Multiplex;
    }
  }*/
  return true;
}
