  
#include <msp430.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <play_screen.h>
#include <state.h>
#include <shape.h>
#include <abCircle.h>
#include <songs.h>

#define SERVING 0
#define IN_PLAY 1
#define GAME_OVER 2

#define WIN_CONDITION 7

int game_state = SERVING;
int in_progress = 0;
int dispose = 0;

int p1_score = 0,
  p2_score = 0;

int cpu_difficulty = 1;

u_int bgColor = COLOR_RED;
#define paddleWidth 15
#define paddleHeight 1
#define paddleOffset 20

extern void reset_game();

Region fieldFence = {
  {12, 12},
  {screenWidth-12, screenHeight-12}
};
Region paddleAsRegion = {
  {0, 0},
  {0, 0}
};

//Geometries

AbRectOutline playingField = {
  abRectOutlineGetBounds,
  abRectOutlineCheck,
  {(screenWidth>>1)-10, (screenHeight>>1)-10}
};

AbRect paddle = {
  abRectGetBounds,
  abRectCheck,
  {paddleWidth, paddleHeight}
};

//Layers

Layer layer1 = {
  (AbShape*)&circle4,
  {(screenWidth>>1),(screenHeight>>1)},
  {0, 0},
  {0, 0},
  COLOR_BLUE,
  0
};

Layer layer0_5 = {
  (AbShape*)&paddle,
  {screenWidth>>1, paddleOffset},
  {0, 0},
  {0, 0},
  COLOR_BLUE,
  &layer1
};

Layer layer0 = {
  (AbShape*)&paddle,
  {screenWidth>>1, screenHeight-paddleOffset},
  {0, 0},
  {0, 0},
  COLOR_BLUE,
  &layer0_5
};

Layer bg = {
  (AbShape*)&playingField,
  {screenWidth>>1, screenHeight>>1},
  {0 ,0},
  {0, 0},
  COLOR_BLACK,
  &layer0
};

typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

MovLayer ml1 = {&layer1, {-1,1}, 0};
MovLayer ml0_5 = {&layer0_5, {0,0}, &ml1};
MovLayer ml0 = {&layer0, {0,0}, &ml0_5};

void movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      
      int condition = (shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
        (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]);
      
      if (condition) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
	song_two();
      }	/**< if outside of fence */
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}

void mlAdvanceTwo(MovLayer *ml, Region *fence)
{
  Vec2 newPos, widthVect, heightVect;
  const Vec2 zeroVect = {0, 0};
  Region shapeBoundary;
  u_int verts;
  
  for (; ml; ml = ml->next) {
    
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);

    int width = shapeBoundary.botRight.axes[0] - shapeBoundary.topLeft.axes[0];
    int height = shapeBoundary.botRight.axes[1] - shapeBoundary.topLeft.axes[1];
    widthVect.axes[0] = width;
    heightVect.axes[1] = height;
    widthVect.axes[1] = heightVect.axes[0] = 0;
    
    Vec2 vertArray[4];

    vertArray[0] = shapeBoundary.topLeft;
    vertArray[1] = shapeBoundary.botRight;
    
    //Top right
    vec2Add(&vertArray[2], &shapeBoundary.topLeft, &widthVect);
    //Bottom left
    vec2Add(&vertArray[3], &shapeBoundary.topLeft, &heightVect);
      
    for(verts = 0; verts < 4; verts++) {
      int condition = fence->topLeft.axes[0] <= vertArray[verts].axes[0] &&
	fence->topLeft.axes[1] <= vertArray[verts].axes[1] &&
	fence->botRight.axes[0] >= vertArray[verts].axes[0] &&
	fence->botRight.axes[1] >= vertArray[verts].axes[1];

      if (condition) {
        int velocity = ml->velocity.axes[1] = -ml->velocity.axes[1];
        newPos.axes[1] += (velocity>>1);
	song_two();
	break;
      }
    }  

    ml->layer->posNext = newPos;
  } /**< for ml */
}

void run_ai(){
  if(layer0_5.pos.axes[0] < layer1.pos.axes[0]) {
    ml0_5.velocity.axes[0] = cpu_difficulty;
  } else {
    ml0_5.velocity.axes[0] = -1 * cpu_difficulty;
  }
}

char int_to_char(int digit) {
  return '0' + digit;
}

void draw_score() {
  static char string[] = "Score: 0-0";
  string[7] = int_to_char(p1_score);
  string[9] = int_to_char(p2_score);
  drawString5x7(36, 2, string,
		COLOR_BLACK, COLOR_RED);
}

void play_screen_render()
{
  if(!in_progress && game_state != GAME_OVER) {
    layerInit(&bg);
    layerDraw(&bg);
    draw_score();
    in_progress = 1;
  }
  
  movLayerDraw(&ml0, &layer0);

  //for undrawing the ball when a score occurs
  if(dispose) {
    dispose = 0;
    layer1.color = COLOR_BLUE;
    draw_score();
  }

  //when the player wins
  if(in_progress && game_state == GAME_OVER) {
    if(p1_score >= p2_score) {
      drawString5x7(40, screenHeight>>1, "YOU WIN!",
		    COLOR_BLACK, COLOR_RED);
    } else {
      drawString5x7(38, screenHeight>>1, "YOU LOSE!",
		    COLOR_BLACK, COLOR_RED);
    }
  }
}

void play_screen_update() 
{
  static int counter = 0;
  unsigned int buttonState = p2sw_read();
  unsigned int delta = buttonState >> 8;
  int iterator = 0;
  
  switch(game_state) {
    case IN_PLAY:
      abRectGetBounds(&paddle, &layer0.pos, &paddleAsRegion);
      mlAdvanceTwo(&ml1, &paddleAsRegion);

      abRectGetBounds(&paddle, &layer0_5.pos, &paddleAsRegion);
      mlAdvanceTwo(&ml1, &paddleAsRegion);

      run_ai();

      if(layer1.pos.axes[1] <= 20 ||
	 layer1.pos.axes[1] >= (screenHeight - 20)) {
	
	dispose = 1;
	layer1.color = COLOR_RED;

	if(layer1.pos.axes[1] <= 20)
	  p1_score++;
	else
	  p2_score++;

	if(p1_score >= WIN_CONDITION ||
	   p2_score >= WIN_CONDITION) {
	  game_state = GAME_OVER;
          if(p1_score > p2_score) {
            song_four();
	    cpu_difficulty++;
	  } else {
	    cpu_difficulty = 1;
	  }
	} else {
	  game_state = SERVING;
	}
      }
  
      mlAdvance(&ml0, &fieldFence);

      for(iterator = 0; iterator < 4; iterator++) {
	int has_changed = delta & (0x1 << iterator);
	int is_set = buttonState & (0x1 << iterator);
	int is_left = iterator < 2;
    
	if(!is_set) {
	  ml0.velocity.axes[0] = is_left ? -4 : 4;
	  return;
	}
      }

      ml0.velocity.axes[0] = 0;
      break;
      
    case SERVING:
      ml1.velocity.axes[0] = ml1.velocity.axes[1] = 0;
      ml0_5.velocity.axes[0] = ml0.velocity.axes[0] = 0;
      layer1.pos.axes[0] = screenWidth>>1;
      layer1.pos.axes[1] = screenHeight>>1;
      layer1.posLast = layer1.posNext = layer1.pos;

      if((buttonState & 0xF) != 0xF) { 
	ml1.velocity.axes[0] = counter & 0x1 ? -1 : 1;
	ml1.velocity.axes[1] = counter & 0x2 ? -1 : 1;
	game_state = IN_PLAY;
      }
      
      break;
    case GAME_OVER:
      if((buttonState & 0xF) != 0xF) {
	reset_game();
      }
      break;
    default:
      break;
  }

  counter++;
}
