/*Versions Daten ab 100H 100Byte reserviert*/
/*
byte 0-4	aktuelle Versionsnummer
byte 5		;
byte 6-8	Sonderversionsnummer
byte 9		;
byte 10-12	Ger鋞etyp	100=TBH210
byte 13		;
byte 14-21	Datum Programm
byte 22		;
byte 23		E=ERWEKA; C=CALEVA
byte 24		;
byte 25-32	Datum Update
byte 33		;
byte 34-43	ID Einspieler Dongle
byte 44		;
byte 45		1=Durchmesser
byte 46		1=Dickenmesser
byte 47		0=Drucker deutsch 1=Drucker China
byte 48		not used
byte 49		0=kleiner Motor, 1=gro遝r Motor
byte 50		0=speed only 1=force
byte 52		not used
byte 54     0=ohne Licensemanagement 1=mit Licensemanagement
byte 56     not used
byte 58-63  Seriennummer
byte 65-84	Drivername

*/

//const unsigned char vers[100] __attribute__((at(0x1000)))=" 1.42;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.00;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";
//2016-8-3 星期三 14:37:49
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.01;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";
//2016/11/2 14:59:26
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.02;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";
//2016/12/5 10:38
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.03;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";
//2016-12-6 星期二 15:44:57 修改了打印机报告关于校正的单位
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.04;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";

// 2017-5-24 星期三 15:48 在厚度测试时因为小数部分四舍五入的问题，造成显示内容和判断结果又出入，因此改为测量结果先舍入再判断；
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.05;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";

// 2019-07-23 星期二 18:03:09 详见2019-06-04工程变更
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.06;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";

// 2019-09-17 星期二 18:31:44 密码显示为‘*’
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.07;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";

// 2019-11-20 星期三 09:42:24 打印多页测试报告时，在第一页以后的每页第三行出现乱码
//const unsigned char vers[100] __attribute__((at(0x1000)))=" 2.08;000;810;12.02.14;E;00.00.00;0000000000;111011;0;0;0;000000;RWKINSTTBH210       ;";

// 2020-03-07 星期六 17:15:04 恒力测试或恒速测试，明确方法
//const unsigned char vers[100] __attribute__((at(0X800F000)))=" 0.01;000;810;12.02.14;E;00.00.00;0000000000;111001;0;0;0;000000;RWKINSTTBH210       ;";

// 2021-06-03 星期四 16:19:52 恒力测试或恒速测试，明确方法
//const unsigned char vers[100] __attribute__((at(0X800F000)))=" 0.01;000;810;12.02.14;E;00.00.00;0000000000;110001;0;0;0;000000;RWKINSTTBH210       ;";

// 2021-07-15 星期四 13:03:11 恒力测试或恒速测试，明确方法，49=1（大电机）
//const unsigned char Init_vers[100] __attribute__((at(0X800F000)))=" 3.01;000;810;12.02.14;E;00.00.00;0000000000;110001;0;0;0;000000;RWKINSTTBH210       ;";

// 2021-09-28 星期二 22:51:35 长度单位毫米/英寸设置参数完成后，不能保存，重启系统后恢复成毫米；
//const unsigned char Init_vers[100] __attribute__((at(0X800F000)))=" 3.02;000;810;12.02.14;E;00.00.00;0000000000;110001;0;0;0;000000;RWKINSTTBH210       ;";

// 2021-10-15 星期五 10:03:38 硬度检查时单位出错的修改；
const unsigned char Init_vers[100] __attribute__((at(0X800F000)))=" 3.05;000;810;12.02.14;E;00.00.00;0000000000;110001;0;0;0;000000;RWKINSTTBH210       ;";

//1.02	
//1.03	01.06.09	Kontrollgewicht Bruchkalibration eingebbar
//1.04	07.09.09	Verbesserungen in Texten (躡ersetzungen)
//1.05	19.10.09	Dual mode f黵 ERWEKA Version eingebaut			(1. an Kunden ausgelieferte Version)
//1.06	09.11.09	Nullweg f黵 gro遝n Motor an Mechanik angepasst
//1.07	11.11.09	Kalibrator II, Terminologie Kalibrieren/justieren
//1.08	16.11.09	AutoDiameter
//1.09	17.11.09	Kalibrator PC Programm Anpassung
//1.10	15.02.10	Backenweg um 2mm verl鋘gert
//1.11	10.03.10	interner 32 Spaltendrucker eingebaut
//1.12	09.04.10	nicht ausgeliefert
//1.13	12.04.10	Fehler in Statistik Anzeige (kp,Sc) behoben,Drucker intern/extern Schltbar 
//1.14	16.04.10	Autodiameter auch f黵 Dicke, Dickenmessung mit Vorbremse wie Durchmesser
//1.15	20.04.10	CALEVA im Kopf bei Ausdrucken, Ergenisse bei kp,Sc mit 1 Nachkommastelle
//1.16	10.05.10	Anzeige von NA bei Fehler Dickenmessung
//1.17	11.05.10	Durchmesser/Dicke im Kalibrierprotokoll, ID Anzeige
//1.18	20.05.10	Baudrate auf 9600 f黵 Drucker, umschaltbar mit SB115200 f黵 VINC Programmierung
//1.19  04.06.10	Falgs bueb und dueb l鰏chen nach ende kalib
//1.20	11.06.10	Justierposition anfahren mit TLU
//1.21  13.07.10	Druckersprache ASCII/PCL, Kalibrator aktiviert
//1.22  05.08.10	amerikanisches datum auf Kalibreirprotokoll korrigiert , 22.08.2010 Stack vergr鲞ert
//1.23	05.10.10	Kommandos f黵 China Drucker 34 zeichen eingef黦t
//1.22  11.01.11	Taste lang gedr點kt halten f黵 Einstellungen, Kein BackenNullen, wenn ohne Durchmesser , Rundungsfehler bei Min-Max Anzeige
//1.23	13.01.11	Problem bei Backen Nullen mit gro遝m Motor
//1.24	24.02.11	Rundungsprobleme bei Sattistik
//1.25	29.03.11	Umstellung Vinculum 2
//1.26  13.05.11	Ausdruckfehler Kalibrierprotokoll interner Drucker
//1.27	18.07.11	Messpause wieder aktiviert
//1.28	29.08.11	backenl鋘genkorrektur von +-2.5mm eingef黨rt, Aufh鋘gen beim Drucken korrigiert
//1.29	20.09.11	Bei Messen von endmass an machen stellen keine FL Anzeige
//1.30	19.10.11	QTEST als Hersteller zugef黦t
//1.31	01.12.11	Toleranzgrenze auf Bruch auf 1 reduziert
//1.32  08.03.12	Chinadruckersteuerzeichen eingebaut, 鋘derbar mit Updateprogram stelle 47
//1.33	23.04.12	Bei 躡erpr黤ung/Kalibration Gewicht ist nullen mit den rechten oder linken Pfeiltatsen m鰃lich
//1.34	10.09.12	Auswerteberechung bei kp Sc korrigiert
//1.35	12.09.12	weitere Korrekturen an Auswertung
//1.36	26.09.12	Vinculum II Anpassungen
//1.37	04.02.13	Kalibrierdatum 黚erschreiben bei Kontrolle
//1.38	11.04.13	Datum in Kalibreirprotokoll korrigiert
//1.39	17.04.13	Ueberpr黤ung Neu im Ausdruck, Datumskorrekturen
//1.40	01.07.13	Bei FL min Auswertung falsch
//1.41  23.07.13	Bei einer Messung min Wert bei Dicke,Durchmesser falsch
//1.42  12.02.14    Datenausgabe 黚er RS232

