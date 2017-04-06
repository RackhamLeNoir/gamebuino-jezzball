#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

int ball_x = LCDWIDTH/2; //set the horizontal position to the middle of the screen
int ball_y = LCDHEIGHT/2; //vertical position
int ball_vx = 1; //horizontal velocity
int ball_vy = 1; //vertical velocity
int ball_size = 2; //the size of the ball in number of pixels

int cursor_s = 3;
int cursor_x = LCDWIDTH/2 - (cursor_s - 1);
int cursor_y = LCDHEIGHT/2;
bool cursor_h = true;

#define LINEIDLE      0
#define LINEEXPANDING 1
int linestate = LINEIDLE;
bool line_h = true;
int line_x, line_y;
int line_width;

void setup()
{
  gb.begin();
  gb.titleScreen(F("Jezz Ball"));
}

void draw()
{
  //cursor
  gb.display.setColor(INVERT);
  if (cursor_h)
    gb.display.drawFastHLine(cursor_x - (cursor_s/2), cursor_y, cursor_s);
  else
    gb.display.drawFastVLine(cursor_x, cursor_y - (cursor_s/2), cursor_s);
  //balls
  gb.display.setColor(BLACK);
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
}

void manageinputs()
{
  if (gb.buttons.pressed(BTN_A))
    cursor_h = !cursor_h;
  /*if (gb.buttons.pressed(BTN_B))
    createline*/
  if (gb.buttons.repeat(BTN_UP, 1))
    cursor_y -= 1;
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cursor_y += 1;
  else if (gb.buttons.repeat(BTN_LEFT, 1))
    cursor_x -= 1;
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cursor_x += 1;

  if (cursor_x < 1)
    cursor_x = 1;
  else if (cursor_x > LCDWIDTH - (cursor_s/2))
    cursor_x = LCDWIDTH/2 - (cursor_s/2);
  if (cursor_y < 1)
    cursor_y = 1;
  else if (cursor_y > LCDHEIGHT - (cursor_s/2))
    cursor_x = LCDHEIGHT/2 - (cursor_s/2);
}

void updategame()
{
    ball_x = ball_x + ball_vx;
    ball_y = ball_y + ball_vy;
    
    //check that the ball is not going out of the screen
    //if the ball is touching the left side of the screen
    if(ball_x < 0){
      //change the direction of the horizontal speed
      ball_vx = -ball_vx;
      //play a preset "tick" sound when the ball hits the border
      gb.sound.playTick();
    }
    //if the ball is touching the right side
    if((ball_x + ball_size) > LCDWIDTH){
      ball_vx = -ball_vx;
      gb.sound.playTick();
    }
    //if the ball is touching the top side
    if(ball_y < 0){
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
    //if the ball is touching the down side
    if((ball_y + ball_size) > LCDHEIGHT){
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
}

void loop(){
  if(gb.update())
  {
    updategame();
    manageinputs();
    draw();
  }
}

