	.arch			msp430g2553
	.p2align		1,0

	.weak in_progress
	.weak game_state
	.weak p1_score
	.weak p2_score
	.weak clearScreen
	.weak bgColor

	.text

	.globl reset_game
reset_game:	
	; void reset_game() {
	;  in_progress = 0;
	;  game_state = SERVING;
	;  p1_score = 0;
	;  p2_score = 0;
	;  clearScreen(bgColor);
	;}


	mov #0, &in_progress 	;in_progress = 0;
	mov #0, &game_state	;game_state = 0
	mov #0, &p1_score	;p1_score = 0
	mov #0, &p2_score	;p2_score = 0
	mov &bgColor, r12
	call #clearScreen	;clearScreen(bgColor)
endl:	ret			;return

