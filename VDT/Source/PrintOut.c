#include "system.h"
#include "utils.h"

extern unsigned char prLang;
extern const unsigned char prTab[4];
extern char id[25],buf[1500];

char const textkp1a[4][85]={"                     %s GERAETEšBERPRšFUNG      Seite: %2d\n\r\n\r\n\r",
                          "                      %s INSTRUMENT CHECK     Page : %2d\n\r\n\r\n\r",
		          "                          %s PROTOCOLE DE CALIBRAGE  Page : %2d\n\r\n\r\n\r",
		          "                       %s PROTOCOLO DE CALIBRACION  Pagina: %2d\n\r\n\r\n\r"};

char const textkp1[4][85]={"                     %s KALIBRIERPROTOKOLL      Seite: %2d\n\r\n\r\n\r",
                          "                      %s CALIBRATIONPROTOCOL     Page : %2d\n\r\n\r\n\r",
		          "                          %s PROTOCOLE DE CALIBRAGE  Page : %2d\n\r\n\r\n\r",
		          "                       %s PROTOCOLO DE CALIBRACION  Pagina: %2d\n\r\n\r\n\r"};


char const textkp2[4][35]={"       Identnummer: %15s\n\r",
		          "       ID-Number: %15s\n\r",
		          "       Numero ID: %15s\n\r",
		          "       Numero ID: %15s\n\r"};

char const textkp2a[4][35]={ "Seriennummer: %20s    \n\r",
						  "Serialnumber: %20s    \n\r",
						  "Serialnumber: %20s    \n\r",
						  "Serialnumber: %20s    \n\r"};	
              
char const textkp3[4][45]={"       Datum: %-8s    Zeit: %-5s\n\n\r",
		          "       Date: %-8s    Time: %-5s\n\n\r",
 		          "       Date: %-8s    Heure: %-5s\n\n\r",
		          "       Fecha: %-8s    Hora: %-5s\n\n\r"};


char const textkp51[4][90]={"\n\n\n\n\n       Unterschrift:.........................................................\n\r",
		                    "\n\n\n\n\n       Signature:............................................................\n\r",
		                    "\n\n\n\n\n       Signature:............................................................\n\r",
		                    "\n\n\n\n\n       Firma:................................................................\n\r"};

char const textkp50a[4][80]={"       Kontrolldatum: %s\n\r",
		                    "       Control date:%s  \n\r",
	                        "       Date de control.:%s \n\r",
		                    "       Fecha de control: %s  \n\r"};




void druckkp(void)
{
	char aktdat[10],akttim[10];
  getdatstr(aktdat);
	gettimstr(akttim);
}

