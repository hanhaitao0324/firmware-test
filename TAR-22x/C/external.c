#include "defines.h"
#include "external.h"
#include "global.h"
#include "parser.h"

//------------------------------------------------------------------------------
// EXTERNAL_Process - parsing procedure
//------------------------------------------------------------------------------
void EXTERNAL_Process(void)
{
  if (globalStatus.enableRxTx)
  {
    if (globalStatus.gotPackage)
    {
      //got a package
      globalStatus.gotPackage=false;
      PARSER_Comm();
    }       
  }
}
