unit main;

{$mode objfpc}{$H+}

interface

uses
  Classes, SysUtils, FileUtil, Forms, Controls, Graphics, Dialogs, ComCtrls,
  ExtCtrls, StdCtrls, EditBtn, vinfo, CalcCRC;

type

  { TmainForm }

  TmainForm = class(TForm)
    btnConvert: TButton;
    cbType: TComboBox;
    cbBaud1: TComboBox;
    cbBaud2: TComboBox;
    cbDevices: TComboBox;
    cbMajVer: TComboBox;
    cbMinVer: TComboBox;
    edFlash: TFileNameEdit;
    edResult: TFileNameEdit;
    Label1: TLabel;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    lbl_PageSize: TLabel;
    lbl_FWStart: TLabel;
    lblFWSize: TLabel;
    lbl_FlashSize: TLabel;
    lblDeviceNb: TLabel;
    lblVerPoint: TLabel;
    lblVersion: TLabel;
    lblType: TLabel;
    lblFlash: TLabel;
    lblBaud1: TLabel;
    lblBaud2: TLabel;
    lblDevice: TLabel;
    lblResult: TLabel;
    pnMain: TPanel;
    StatusBar1: TStatusBar;
    procedure btnConvertClick(Sender: TObject);
    procedure cbDevicesChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);

  private
    { private declarations }
  public
    { public declarations }
  end;

const
     LOADER_SIZE: Integer = 1280;
     IDENT_SIZE: Integer = 5;

     HEX_DATA: Char = chr($00);
     HEX_END: Char = chr($01);
     HEX_SEG: Char = chr($02);
     HEX_SSTART: Char = chr($03);
     HEX_ADDADDR: Char = chr($04);

     LPC1768_ADDR_START: Integer       = $10000;
     LPC2478_ADDR_START: Integer       = $2000;
     STM32F103_ADDR_START: Integer     = $08001000;
     LPC4088_ADDR_START: Integer       = $5000;
     LPC4088_NOR_ADDR_START: Longword  = $90000000;
     STM32F405_ADDR_START: Integer     = $08004000;
     STM32F407_ADDR_START: Integer     = $08010000;
     STM32F429_ADDR_START: Integer     = $08010000;

type DEVICES =
  (
    DEVICE_ZTx20,                 //0
    DEVICE_SVM_TAR,
    DEVICE_DT72x,
    DEVICE_AR,
    DEVICE_DHC1500,
    DEVICE_FRL,                   //5
    DEVICE_SP,
    DEVICE_OLD_TAR,
    DEVICE_ZTBOX,
    DEVICE_ST35,
    DEVICE_IBASKET,               //10
    DEVICE_TBH,
    DEVICE_GTL,
    DEVICE_GTB,
    DEVICE_DTUNI,
    DEVICE_MPREP,                 //15
    DEVICE_AUTOCAL,
    DEVICE_TFTCONSOLE,
    DEVICE_ZTHEATER,
    DEVICE_DT_AUTOSAMPLER,
    DEVICE_DT_LIFTER,             //20
    DEVICE_ACCUBOARD,
    DEVICE_PISTONPUMP,
    DEVICE_DT_DHQ,
    DEVICE_DT_CLEANER,
    DEVICE_ACCUBOARD2,            //25
    DEVICE_PMUX,
    DEVICE_HKPEXT,
    DEVICE_PPUMPRS232,
    DEVICE_PRINTERBOARD,
    DEVICE_TFT_NEW,               //30
    DEVICE_ZT_MAIN,
    DEVICE_ZT_BASKET,
    DEVICE_ZT_HEATER,
    DEVICE_VDT,
    DEVICE_ZT_BASKET_WIRELESS,    //35
    DEVICE_ZT_BASKET_HALL_WIRELESS,
    DEVICE_CAN_RS232,
    DEVICE_DTS8,
    DEVICE_UNIMAIN,
    SVM_II_Main,                   // 40
    SVM_II_Touch,                  // 41
    TBH_II,                        // 42
    DEVICE_LAST                    // 43
  );

type MCU =
  (
    MCU_ATMEGA8,
    MCU_ATMEGA8515,
    MCU_ATMEGA8535,
    MCU_ATMEGA88,
    MCU_ATMEGA16,
    MCU_ATMEGA162,
    MCU_ATMEGA163,
    MCU_ATMEGA164,
    MCU_ATMEGA165,
    MCU_ATMEGA168,
    MCU_ATMEGA169,
    MCU_ATMEGA32,
    MCU_ATMEGA323,
    MCU_ATMEGA324,
    MCU_ATMEGA329,
    MCU_ATMEGA3290,
    MCU_ATMEGA64,
    MCU_ATMEGA649,
    MCU_ATMEGA6490,
    MCU_ATMEGA640,
    MCU_ATMEGA644,
    MCU_ATMEGA128,
    MCU_ATMEGA1280,
    MCU_ATMEGA1281,
    MCU_LPC1768,
    MCU_LPC2478,
    MCU_STM32F103RBT6,
    MCU_LPC4088,
    MCU_STM32F405RG,
    MCU_STM32L051,
    MCU_STM32F407VET6,
    MCU_STM32F429IGT6,
    MCU_LAST
  );

type cmbStruct = Record
      str: PChar;
      sign: Array[0..2] of Byte;
      FSize: Integer;
      ESize: Integer;
      PSize: Word;
end;

type TheadFile = Record
    Id: Array[0..4] of Byte;   // Firmware file identifier (AVRFW)
    sign: Array[0..2] of Byte; // Controller signature (MCU Id from the table below)
    device: Word;              // Device signature (Id from the table below)
    baud1:  Word;              // Initial baudrate (Value from the table below
    baud2:  Word;              // Transfer baudrate (Value from the table below
    psize:  Word;              // Block size for writing to device
    foffs:  Integer;           // Binary data offset (from the start of file, usually 0x00000020)
    fsize:  Integer;           // Binary data size
    eoffs:  Integer;           // Flash data offset (from the start of file), used for external Flash-memory
    esize:  Integer;           // Flash data size
    fstart: Integer;
    estart: Integer;
    majVer: Byte;
    minVer: Byte;
end;

const cmbBaud: Array[0..10] of PChar =
(
    '1200',
    '2400',
    '4800',
    '9600',
    '14400',
    '19200',
    '38400',
    '56000',
    '57600',
    '115200',
    NIL
);

const cmbDevices: Array[0..43] of PChar =
(
    'ZTx20',                   //0
    'SVM/TA/TAR',
    'DT72x',
    'AR',
    'DHC1500',
    'FRL',                     //5
    'SP',
    'Old TA/TAR',
    'ZTBox',
    'ST35',
    'iBasket',                 //10
    'TBH',
    'GTL',
    'GTB',
    'DT-Uni',
    'MediPrep',                //15
    'AutoCal',
    'TFT-Console',
    'ZT Heater',
    'DT Austosampler',
    'DT Lifter',               //20
    'Accu Board',
    'Piston Pump',
    'DHQ',
    'DT Cleaner',
    'Accu Board 2',            //25
    'P-Mux',
    'HKP Ext.(SMC)',
    'PPump RS232',
    'Printer Board',
    'TFT Board',               //30
    'ZT Main board',
    'ZT Basket',
    'ZT Heater',
    'VDT',
    'ZT Basket wireless',      //35
    'ZT Basket Hall wireless',
    'CAN_RS232 board',
    'DTS8',
    'UniMain',
    'SVM II Main',            //40
    'SVM II Touch',           //41
    'TBH II',                 //42
    NIL
);

const cmbType: Array[0..32] of cmbStruct =
(
    (str: 'ATmega8'; sign: ($1E, $93, $07); FSize: 8192; ESize: 512; PSize: 64),
    (str: 'ATmega8515'; sign: ($1E, $93, $06); FSize: 8192; ESize: 512; PSize: 64),
    (str: 'ATmega8535'; sign: ($1E, $93, $08); FSize: 8192; ESize: 512; PSize: 64),
    (str: 'ATmega88'; sign: ($1E, $93, $0A); FSize: 8192; ESize: 512; PSize: 64),
    (str: 'ATmega16'; sign: ($1E, $94, $03); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega162'; sign: ($1E, $94, $04); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega163'; sign: ($1E, $94, $02); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega164'; sign: ($1E, $94, $0A); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega165'; sign: ($1E, $94, $07); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega168'; sign: ($1E, $94, $06); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega169'; sign: ($1E, $94, $05); FSize: 16384; ESize: 512; PSize: 128),
    (str: 'ATmega32'; sign: ($1E, $95, $02); FSize: 32768; ESize: 1024; PSize: 128),
    (str: 'ATmega323'; sign: ($1E, $95, $01); FSize: 32768; ESize: 1024; PSize: 128),
    (str: 'ATmega324'; sign: ($1E, $95, $08); FSize: 32768; ESize: 1024; PSize: 128),
    (str: 'ATmega329'; sign: ($1E, $95, $03); FSize: 32768; ESize: 1024; PSize: 128),
    (str: 'ATmega3290'; sign: ($1E, $95, $04); FSize: 32768; ESize: 1024; PSize: 128),
    (str: 'ATmega64'; sign: ($1E, $96, $02); FSize: 65536; ESize: 2048; PSize: 256),
    (str: 'ATmega649'; sign: ($1E, $96, $03); FSize: 65536; ESize: 2048; PSize: 256),
    (str: 'ATmega6490'; sign: ($1E, $96, $04); FSize: 65536; ESize: 2048; PSize: 256),
    (str: 'ATmega640'; sign: ($1E, $96, $08); FSize: 65536; ESize: 4096; PSize: 256),
    (str: 'ATmega644'; sign: ($1E, $96, $0A); FSize: 65536; ESize: 2048; PSize: 256),
    (str: 'ATmega128'; sign: ($1E, $97, $02); FSize: 131072; ESize: 4096; PSize: 256),
    (str: 'ATmega1280'; sign: ($1E, $97, $03); FSize: 131072; ESize: 4096; PSize: 256),
    (str: 'ATmega1281'; sign: ($1E, $97, $04); FSize: 131072; ESize: 4096; PSize: 256),
    (str: 'LPC1768'; sign: ($20, $01, $05); FSize: 524288; ESize: 4096; PSize: 256),
    (str: 'LPC2478'; sign: ($21, $01, $05); FSize: 524288; ESize: 4096; PSize: 256),
    (str: 'STM32F103RBT6'; sign: ($22, $01, $01); FSize: 131072; ESize: 0; PSize: 256),
    (str: 'LPC4088+NOR-Flash'; sign: ($23, $01, $05); FSize: 524288; ESize: 67108864; PSize: 256),
    (str: 'STM32F405RG'; sign: ($22, $02, $01); FSize: 1048576; ESize: 0; PSize: 256),
    (str: 'STM32L051'; sign: ($22, $03, $01); FSize: 131072; ESize: 0; PSize: 256),
    (str: 'STM32F407VET6'; sign: ($22, $01, $01); FSize: 1048576; ESize: 0; PSize: 256),
    (str: 'STM32F429IGT6'; sign: ($22, $01, $01); FSize: 2097152; ESize: 0; PSize: 256),
    (str: NIL)
);

const FILE_IDENT: PChar = 'AVRFW';

var
  mainForm: TmainForm;

implementation

function toint_new(datab: Char): Byte;
var
   input: Byte;
begin
   input := 0;

   if ((Byte(datab)>=$30)and(Byte(datab)<=$39)) then
     input := Byte(datab) - $30
   else
   if ((Byte(datab)>=$41)and(Byte(datab)<=$46)) then
     input := Byte(datab) - $37
   else
   if ((Byte(datab)>=$61)and(Byte(datab)<=$66)) then
     input := Byte(datab) - $57;
   Result := input;
end;

function hex2char(str: PChar): Byte;
var
   retval: Byte;
   byteH, byteL: Char;
begin

   // get a two-byte ASCII representation of a char from the UART
   byteH := str[0];
   Inc(str);
   byteL := str[0];
   Inc(str);

   // convert to a single 8 bit result
   retval := toint_new(byteH) * 16 + toint_new(byteL);
   Result := retval;
end;

{$R *.lfm}

{ TmainForm }

procedure TmainForm.btnConvertClick(Sender: TObject);
var
  f_src: TextFile;
  //f_dest: File;
  len: Byte;                     // holds the HEX record length field
  record_type: Byte;             // holds the HEX record type field
  offset: Longword;              // holds the HEX record offset field
  first_offset: Longword;        // holds the first offset
  CRC16: Word = 0;
  CRC32: Longword = $FFFFFFFF;
  bwrite: Byte;
  msg: string;
  headFile: TheadFile;
  //destHandle: THandle;
  ix: Integer;
  add_segment: Longword;
  curPos: Integer;
  outStream: TFileStream;
  FlashFlag: Boolean;
  first_offset_flag: Boolean;
  headOffs: Integer;
  Counter: Integer = 0;

begin
  // start convertion
  if (edFlash.FileName='') then
  begin
       ShowMessage('Please enter Flash HEX-file first!');
       Exit;
  end;
  if (not(FileExists(edFlash.FileName))) then
  begin
       ShowMessage('Cann''t open Flash file ' + edFlash.FileName + '!');
       Exit;
  end;

  if (edResult.FileName='') then
  begin
       ShowMessage('Please enter the name of the result file first!');
       Exit;
  end;

  add_segment := 0;
  first_offset := 0;
  first_offset_flag := true;
  FlashFlag := false;

  AssignFile(f_src, edFlash.FileName);
  Reset(f_src);

  //AssignFile(f_dest, edResult.FileName);
  //Rewrite(f_dest, SizeOf(Byte));
  //destHandle := GetFileHandle(f_dest);
  //destHandle := FileOpen(edResult.FileName, fmOpenReadWrite);
  //FileSeek(destHandle, 0, fsFromBeginning);
  outStream := TFileStream.Create(edResult.FileName, fmCreate);

  Move(FILE_IDENT^, headFile.Id, IDENT_SIZE);
  Move(cmbType[cbType.ItemIndex].sign, headFile.sign, 3);
  headFile.device := cbDevices.ItemIndex;
  headFile.psize  := cmbType[cbType.ItemIndex].PSize;
  headFile.baud1  := cbBaud1.ItemIndex;
  headFile.baud2  := cbBaud2.ItemIndex;
  headFile.foffs  := SizeOf(TheadFile);
  headFile.majVer := cbMajVer.ItemIndex;
  headFile.minVer := cbMinVer.ItemIndex;

  headFile.fstart := 0;
  headFile.estart := 0;
  headFile.eoffs  := 0;
  headFile.esize  := 0;

  //FileSeek(destHandle, headFile.foffs, fsFromBeginning);
  outStream.Seek(headFile.foffs, soFromBeginning);

  StatusBar1.Panels.Items[0].Text := 'Converting...';
  Application.ProcessMessages;

  headOffs := headFile.foffs;
  headFile.fstart := 0;

  if (cbType.ItemIndex=Ord(MCU_LPC1768)) then
  begin
    headFile.fstart := LPC1768_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_LPC2478)) then
  begin
    headFile.fstart := LPC2478_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_STM32F103RBT6)) then
  begin
    headFile.fstart := STM32F103_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_LPC4088)) then
  begin
    headFile.fstart := LPC4088_ADDR_START;
    headFile.estart := LPC4088_NOR_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_STM32F405RG)) then
  begin
    headFile.fstart := STM32F405_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_STM32F407VET6)) then
  begin
    headFile.fstart := STM32F407_ADDR_START;
  end;
  if (cbType.ItemIndex=Ord(MCU_STM32F429IGT6)) then
  begin
    headFile.fstart := STM32F429_ADDR_START;
  end;

  while not Eof(f_src) do
  begin
    readln(f_src, msg);
    ix := 1;
    while ((msg[ix]<>':')and(msg[ix]<>chr(0))) do Inc(ix);
    if (msg[ix]=chr(0)) then continue;
    Inc(ix);

    // get the record length
    len := hex2char(@msg[ix]);
    ix := ix + 2;

    // get the starting address (offset field in HEX record)
    offset := hex2char(@msg[ix]);               // get the MSB
    ix := ix + 2;
    offset := offset<<8;
    offset := offset + hex2char(@msg[ix]);      // get the LSB
    ix := ix + 2;

    // get the record type
    record_type := hex2char(@msg[ix]);
    ix := ix + 2;

    if (record_type=Ord(HEX_SEG)) then
    begin
      //get segment offset
      offset := hex2char(@msg[ix]);              // get the MSB
      ix := ix + 2;
      offset := offset<<8;
      offset := offset + hex2char(@msg[ix]);     // get the LSB
      ix := ix + 2;
      add_segment := offset<<4;
      continue;
    end;

    if (record_type=Ord(HEX_ADDADDR)) then
    begin
      //get additional address
      offset := hex2char(@msg[ix]);             // get the MSB
      ix := ix + 2;
      offset := offset<<8;
      offset := offset + hex2char(@msg[ix]);    // get the LSB
      ix := ix + 2;
      add_segment := offset<<16;
    end;

    if (record_type<>Ord(HEX_DATA)) then
      continue;

    offset := offset + add_segment;

    if (first_offset_flag=true) then
    begin
       first_offset := offset;
       first_offset_flag := false;
       headFile.fstart := first_offset;
    end;

    if (offset=LPC4088_NOR_ADDR_START) then
    begin
        FlashFlag := true;
        first_offset := offset;
    end;

    {if (cbType.ItemIndex=Ord(MCU_LPC1768)) then
    begin
      if (offset<LPC1768_ADDR_START) then
        continue
      else
        offset := offset - LPC1768_ADDR_START;
    end;
    if (cbType.ItemIndex=Ord(MCU_LPC2478)) then
    begin
      if (offset<LPC2478_ADDR_START) then
        continue
      else
        offset := offset - LPC2478_ADDR_START;
    end;
    if (cbType.ItemIndex=Ord(MCU_STM32F103RBT6)) then
    begin
      if (offset<STM32F103_ADDR_START) then
        continue
      else
        offset := offset - STM32F103_ADDR_START;
    end;
    if (cbType.ItemIndex=Ord(MCU_LPC4088)) then
    begin
      if (offset<LPC4088_ADDR_START) then
        continue
      else if (offset=LPC4088_NOR_ADDR_START) then
        FlashFlag := true;
      if (offset>=LPC4088_NOR_ADDR_START) then
        offset := offset - LPC4088_NOR_ADDR_START
      else
        offset := offset - LPC4088_ADDR_START;
    end;
    if (cbType.ItemIndex=Ord(MCU_STM32F405RG)) then
    begin
      if (offset<STM32F405_ADDR_START) then
        continue
      else
        offset := offset - STM32F405_ADDR_START;
    end;}

    if (offset<first_offset) then
      continue
    else
      offset := offset - first_offset;

    if (FlashFlag=true) then
    begin
      curPos := outStream.Position;
      headFile.fsize := (curPos - headFile.foffs);
      if (cbType.ItemIndex=Ord(MCU_LPC4088)) then
      begin
        CRC32 := CRC32 xor $FFFFFFFF;
        outStream.Write(CRC32, SizeOf(Longword));
        curPos := curPos + 4;
      end else
      begin
        outStream.Write(CRC16, SizeOf(Word));
        curPos := curPos + 2;
      end;
      headFile.eoffs := curPos;
      CRC16 := 0;
      CRC32 := $FFFFFFFF;
      FlashFlag := false;
      Counter := 0;
      headOffs := headFile.eoffs;
    end;

    offset := offset + headOffs;
    //FileSeek(destHandle, offset, fsFromBeginning);
    outStream.Seek(offset, soFromBeginning);

    while (ix<(len*2+9)) do
    begin
       bwrite := hex2char(@msg[ix]);   // write one byte to file
       outStream.Write(bwrite, SizeOf(Byte));
       //CRC16 := CalcCRC16(CRC16, bwrite);
       CRC32 := CalcCRC32(CRC32, bwrite);    //?? same problem with HEX-records alignment?
       Inc(Counter);
       ix := ix + 2;
    end;

  end;

  CloseFile(f_src);

  if (cbType.ItemIndex<>Ord(MCU_LPC4088)) then
  begin
    curPos := outStream.Position;
    headFile.fsize := (curPos - headFile.foffs);
    // calculate CRC
    outStream.Seek(headFile.foffs, soFromBeginning);
    for ix := 0 to (headFile.fsize-1) do
    begin
      outStream.Read(bwrite, SizeOf(Byte));
      CRC16 := CalcCRC16(CRC16, bwrite);
    end;
    outStream.Seek(headFile.foffs + headFile.fsize, soFromBeginning);
    outStream.Write(CRC16, SizeOf(Word));
    curPos := curPos + 2;
    headFile.eoffs := curPos;

    if (headFile.fsize>(cmbType[cbType.ItemIndex].FSize-LOADER_SIZE)) then
    begin
      // firmware too big, loader overwritting is possible, stop!
      ShowMessage('Firmware is too big and will overwrite the loader!\r\nGeneration terminated.');
      outStream.Free;
      // delete file, because this firmware can damage the loader
      DeleteFile(edResult.FileName);
      StatusBar1.Panels.Items[0].Text := 'Error detected!';
      Exit;
    end;
  end;

  curPos := outStream.Position;
  headFile.esize := curPos - headFile.eoffs;//headFile.foffs - headFile.fsize - 2;
  if (headFile.esize=0) then
    CRC16 := 0;

  // calculate CRC
  CRC16 := 0;
  outStream.Seek(headFile.eoffs, soFromBeginning);
  for ix := 0 to (headFile.esize-1) do
  begin
    outStream.Read(bwrite, SizeOf(Byte));
    CRC16 := CalcCRC16(CRC16, bwrite);
  end;
  outStream.Seek(headFile.eoffs + headFile.esize, soFromBeginning);

  //outStream.Write(CRC16, SizeOf(Word));
  if (cbType.ItemIndex=Ord(MCU_LPC4088)) then
  begin
    CRC32 := CRC32 xor $FFFFFFFF;
    outStream.Write(CRC32, SizeOf(Longword));
    curPos := curPos + 4;
  end else
  begin
    outStream.Write(CRC16, SizeOf(Word));
    curPos := curPos + 2;
  end;

  outStream.Seek(0, soFromBeginning);
  outStream.Write(&headFile, SizeOf(headFile));
  outStream.Free;

  StatusBar1.Panels.Items[0].Text := 'Ready!';
  lbl_FlashSize.Caption:= cmbType[cbType.ItemIndex].FSize.ToString;
  lbl_PageSize.Caption:=  headFile.psize.ToString;
  lbl_FWStart.Caption:=   headFile.fstart.ToHexString();
  lblFWSize.Caption:=     headFile.fsize.ToString;

end;

procedure TmainForm.cbDevicesChange(Sender: TObject);
begin
     lblDeviceNb.Caption:= cbDevices.ItemIndex.ToString;
end;


procedure TmainForm.FormCreate(Sender: TObject);
Var
  MajorNum : String;
  MinorNum : String;
  RevisionNum : String;
  BuildNum : String;
  Info: TVersionInfo;
  MyVersion: String;
  ix: Integer;
begin
  // check software version
    Info := TVersionInfo.Create;
    Info.Load(HINSTANCE);
    MajorNum := IntToStr(Info.FixedInfo.FileVersion[0]);
    MinorNum := IntToStr(Info.FixedInfo.FileVersion[1]);
    RevisionNum := IntToStr(Info.FixedInfo.FileVersion[2]);
    BuildNum := IntToStr(Info.FixedInfo.FileVersion[3]);
    Info.Free;

    MyVersion := MajorNum+'.'+MinorNum+'.'+RevisionNum+'.'+BuildNum;
    mainForm.Caption := 'ERWEKA Firmware Generator ver.' + MyVersion;

    ix := 0;
    while (cmbType[ix].str<>NIL) do
    begin
      cbType.Items.Add(cmbType[ix].str);
      Inc(ix);
    end;
    cbType.ItemIndex := 26;

    ix := 0;
    while (cmbBaud[ix]<>NIL) do
    begin
      cbBaud1.Items.Add(cmbBaud[ix]);
      cbBaud2.Items.Add(cmbBaud[ix]);
      Inc(ix);
    end;
    cbBaud1.ItemIndex := 3;
    cbBaud2.ItemIndex := 9;
    ix := 0;
    while (cmbDevices[ix]<>NIL) do
    begin
      cbDevices.Items.Add(cmbDevices[ix]);
      Inc(ix);
    end;
    cbDevices.ItemIndex := 40;
    lblDeviceNb.Caption:= cbDevices.ItemIndex.ToString();

    for ix := 0 to 99 do
    begin
      cbMajVer.Items.Add(IntToStr(ix));
      cbMinVer.Items.Add(IntToStr(ix));
    end;
    cbMajVer.ItemIndex := 1;
    cbMinVer.ItemIndex := 0;

end;



end.

