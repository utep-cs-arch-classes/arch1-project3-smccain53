  
#include <msp430.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <welcome_screen.h>
#include <state.h>

void welcome_screen_render()
{
  drawString5x7(50, 20, "PONG", 
    COLOR_BLACK, COLOR_RED);

  drawString5x7(20, 130, "By Stacy McCain", 
    COLOR_BLACK, COLOR_RED);
}

void welcome_screen_update() 
{
  unsigned int buttonState = p2sw_read();
  unsigned int delta = buttonState >> 8;
  unsigned int iterator = 0;

  static int first_pass = 1;

  if(first_pass) {
    first_pass = 0;
    return;
  }

  for(iterator = 0; iterator < 4; iterator++) {
    unsigned int has_changed = delta & (0x1 << iterator);
    unsigned int is_set = buttonState & (0x1 << iterator);

    if(is_set && has_changed) {
      set_state(1);
      break;
    }
  }
}
