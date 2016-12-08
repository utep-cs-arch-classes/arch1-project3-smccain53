This is a ping pong game played against CPU. First player to get to 7 points wins. The difficulty (speed of CPU following the ball) gradually increases until all 7 points are reached and resets after every game. The following is a brief description of the methods inside it.


audio.c: Utility library for audio functionality. The audio_init function configures the audio device for use in the msp430 while the audio_set_peroid function allows the programmer to control what sounds the audio device is playing.

songs.c: Facilitates playing complex audio on the msp430 by enabling the developer to play a series of notes in sequence using the provided functions (song_one, song_two, etc).

advance_song.s: Assembly code that forces the currently played song to advance a single note. If the function reaches the end of the song, it halts iterating over the song.

welcome_screen.c: Displays the name of the game along with the creator's name. Pressing any key and releasing the key will result in a transition to the play_screen.

play_screen.c: Displays the games interface to the user. CPU opponent increases in difficulty as the play wins consecutive matches. The CPU's difficulty resets when the player loses. Difficulty = CPU paddle moves faster. First player to reach a score of 7 wins. A sound plays when either the paddle or ball hit a wall (or paddle collides with ball). Score is displayed at the top of the screen. Game has three basic states:

-SERVING: pressing any key during this state will cause the ball to launch in a random direction based on a counter and a bit mask.

-IN_PLAY: Ball bounces around the screen. Player can move his or her paddle. CPU paddle also moves (speed based on # of consecutive player wins). When either player scores, their score will increment. State will change either to SERVING or GAME_OVER depending on if either player acheived a winning score (>= 7 points).

-GAME_OVER: Game halts and displays victory or defeat message depending on outcome of game. Pressing any key will reset the game causing the state machine to enter the SERVING state.

//mlAdvanceTwo is like mlAdvance but for box colliders instead of fence colliders. Function was not described in the code base.

state.c: Controls the master state of the application. Currently there are only two, one for the welcome screen and one for the play screen. Provides functions for getting and setting system state.

wdt_handler.c: contains the watch dog timer event handler. This was provided to us for the lab.