#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"   
#include "menu.h"
#include "platform.h" 
#include "sound.h"

static __flash char str_exit[]="EXIT";

//------------------------------------------------------------------------------
// MENU_Run - run menu
//------------------------------------------------------------------------------
void MENU_Run(TMenu *menu)
{
  TByte key;
  TByte menuIndex;
  //TByte nextIndex;
    
  menuIndex=0;
  for(;;)
  {
    LED_Print_P(LED_DISPLAY_TEMP,false,menu->title);
    LED_Print_P(LED_DISPLAY_SPEED,false,menu->items[menuIndex]);
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();
    switch(key)
    {
      case KBD_CODE_START:
        if (menuIndex<menu->numItems)
        {
          SND_Play(ssOk);
          if (!(menu->handler[menuIndex]())) return;
        }
        else
        {
          SND_Play(ssCancel);
          return;
        }
        break;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return;
      case KBD_CODE_SPEED_UP:
        if (menuIndex)
        {
          /*nextIndex=menuIndex;
          do
          {
            nextIndex--;
          }
          while ((nextIndex)&&(!(menu->active[nextIndex])));
          if (menu->active[nextIndex])
          {*/  
            menuIndex--;
            //menuIndex=nextIndex;
            SND_Play(ssSlowDown);
          //} else SND_Play(ssError);
        } else SND_Play(ssError);
        break;
      case KBD_CODE_SPEED_DOWN:
        /*nextIndex=menuIndex;
        do
        {
          nextIndex++;
        }
        while ((nextIndex<menu->numItems)&&(!(menu->active[nextIndex])));*/
        menuIndex++;
        if (menuIndex==menu->numItems)
        //if (nextIndex==menu->numItems)
        {
          SND_Play(ssError);
          menuIndex--;
        } else 
        {  
          SND_Play(ssSlowUp);
          //menuIndex=nextIndex;
        }
        break;
      default:
        SND_Play(ssError);
        break;
    }   
  }
}                          

//------------------------------------------------------------------------------
// MENU_RunList - run list of menus
//------------------------------------------------------------------------------
signed char MENU_RunList(TList *list)
{
  TByte key;
  TByte index;
    
  index=list->current;
  for(;;)
  {
    LED_Print_P(LED_DISPLAY_TEMP,false,list->title);
    if (index<list->numItems)
      LED_Print_P(LED_DISPLAY_SPEED,false,list->items[index]);
    else
      LED_Print_P(LED_DISPLAY_SPEED,false,str_exit);
    while(!KBD_KeyPressed())
    {
      EXTERNAL_Process();
    }
    key=KBD_GetKey();
    switch(key)
    {
      case KBD_CODE_START:
        SND_Play(ssOk);
        if (index!=list->numItems) 
        {
          list->current=index;
          return index;
        } else return -1;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        if (index<list->numItems)
          list->current=index;
        return -1;
      case KBD_CODE_SPEED_UP:
        if (index)
        {
          index--;
          SND_Play(ssSlowDown);
        } else SND_Play(ssError);
        break;
      case KBD_CODE_SPEED_DOWN:
        index++;
        if (index>list->numItems)
        {
          index=list->numItems;
          SND_Play(ssError);
        } else SND_Play(ssSlowUp);
        break;
      default:
        SND_Play(ssError);
        break;
    }   
  }
}                          
