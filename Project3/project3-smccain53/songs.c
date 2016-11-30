#include <songs.h>
#include <audio.h>

#define NUMBER_OF_NOTES 8
static short periods[] = {3830, 3400, 3038, 2864, 2550, 2272, 2028, 1912};
static char symbols[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};

extern char* songs[];
short current_note = 0;
short current_song = 0;

void
play() {
  int i;
  for(i = 0; i < NUMBER_OF_NOTES; i++) {
    if(songs[current_song][current_note] == symbols[i]) {
      audio_set_period(periods[i]);
      return;
    }
  }
  audio_set_period(0);
}

void
song_one() {
  current_song = 0;
  current_note = 0;
  reset_count(); 
  play();
}

void
song_two() {
  current_song = 1;
  current_note = 0;
  reset_count();
  play();
}

void
song_three() {
  current_song = 2;
  current_note = 0;
  reset_count();
  play();
}

void
song_four() {
  current_song = 3;
  current_note = 0;
  reset_count();
  play();
}
