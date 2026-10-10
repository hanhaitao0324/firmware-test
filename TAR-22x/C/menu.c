#include "external.h"
#include "global.h"
#include "kbd.h"
#include "led.h"   
#include "menu.h"
#include "platform.h" 
#include "sound.h"

static __flash char str_exit[]="EXIT";

TMenuItemStr CHOOSE_LIST[TIME_UNIT_LAST]=
{
  "TIME",
  "STRK"
};

//------------------------------------------------------------------------------
//run menu
//------------------------------------------------------------------------------
void MENU_Run(TMenu *menu)
{
  TByte key;
  TByte menuIndex;
    
  menuIndex=0;
  for(;;)
  {
    LED_Print_P(LED_DISPLAY_LEFT,false,menu->title);
    LED_Print_P(LED_DISPLAY_RIGHT,false,menu->items[menuIndex]);
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
        } else
        {
          SND_Play(ssCancel);
          return;
        }
        break;
      case KBD_CODE_STOP:
        SND_Play(ssCancel);
        return;

      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if (menuIndex)
        {
          menuIndex--;
          SND_Play(ssSlowDown);  //repeatable
        } else
        {
          SND_Play(ssError);     //repeatable
        }
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        menuIndex++;
        if (menuIndex==menu->numItems)
        {
          SND_Play(ssError);     //repeatable
          menuIndex--;
        } else
        {
          SND_Play(ssSlowUp);    //repeatable
        }
        break;
      default:
        SND_Play(ssError);
        break;
    }   
  }
}                          

//------------------------------------------------------------------------------
//run list
//------------------------------------------------------------------------------
signed char MENU_RunList(TList *list)
{
  TByte key;
  TByte index;
    
  index=list->current;
  for(;;)
  {
    LED_Print_P(LED_DISPLAY_LEFT,false,list->title);
    if (index<list->numItems)
      LED_Print_P(LED_DISPLAY_RIGHT,false,list->items[index]);
    else
      LED_Print_P(LED_DISPLAY_RIGHT,false,str_exit);
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
      case KBD_CODE_FAST_UP:
      case KBD_CODE_SLOW_UP:
        if (index)
        {
          index--;
          SND_Play(ssSlowDown);     //repeatable
        } else
        {
          SND_Play(ssError);        //repeatable
        }
        break;
      case KBD_CODE_SLOW_DOWN:
      case KBD_CODE_FAST_DOWN:
        index++;
        if (index>list->numItems)
        {
          index=list->numItems;
          SND_Play(ssError);        //repeatable
        } else
        {
          SND_Play(ssSlowUp);       //repeatable
        }
        break;
      default:
        SND_Play(ssError);
        break;
    }   
  }
}                          
