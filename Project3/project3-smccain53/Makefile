# makfile configuration
CPU             	= msp430g2553
CFLAGS          	= -mmcu=${CPU} -Os -I./h -I./h
APP_NAME		= pong

#switch the compiler (for the internal make rules)
CC              = msp430-elf-gcc
AS              = msp430-elf-gcc -mmcu=${CPU} -c

all:$(APP_NAME).elf

#additional rules for files
$(APP_NAME).elf: ${COMMON_OBJECTS} $(APP_NAME).o songs.o advance_song.o state.o wdt_handler.o welcome_screen.o play_screen.o audio.o reset_game.o
	${CC} -mmcu=${CPU} -o $@ $^ -L./lib -lTimer -lLcd -lShape -lCircle -lp2sw

load: $(APP_NAME).elf
	mspdebug rf2500 "prog $^"

clean:
	rm -f *.o *.elf
