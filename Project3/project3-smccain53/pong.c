  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include <state.h>
#include <audio.h>

#include <welcome_screen.h>
#include <play_screen.h>

#define WELCOME_SCREEN 0
#define PLAY_SCREEN 1

int redrawScreen = 1;
int decisecond_count = 0;

extern void advance_song(void);
void drawScreen();

void reset_count() {
  decisecond_count = 0;
}

void decisecond() 
{
  static char cnt = 0;		/* # deciseconds/frequecy change */
  if (++cnt > 2) {		
    cnt = 0;
  }
}

void main()
{
  configureClocks();
  lcd_init();
  p2sw_init(0xF);
  audio_init();
  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */

  clearScreen(bgColor);
  redrawScreen = 1;

  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      or_sr(0x10);	      /**< CPU OFF */
    }
    drawScreen();
  }
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  count++;
  if (count >= 15) {
    
    switch(get_state()) {
      case WELCOME_SCREEN: default:
        welcome_screen_update();
        redrawScreen = 0;
        break;
      case PLAY_SCREEN:
        play_screen_update();
	if(p2sw_read())
          redrawScreen = 1;
      break;
    }
    
    count = 0;
  }

  static char second_count = 0;
  if (++second_count == 250) {
    second_count = 0;
  }
  if (++decisecond_count == 25) {
    decisecond_count = 0;
    advance_song();
    play();
  }
}

void drawScreen(){

  switch(get_state()) {
    case WELCOME_SCREEN: default:
      welcome_screen_render();
    break;
    case PLAY_SCREEN:
      play_screen_render();
    break;
  }

  redrawScreen = 0;
}
