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

int board_x = 0;
int board_y = 0;
int board_w = LCDWIDTH;
int board_h = LCDHEIGHT;

#define LINEIDLE      0
#define LINEEXPANDING 1
uint8_t linestate = LINEIDLE;
bool line_h = true;
int line_x, line_y;
int line_width = 0;

const uint8_t logo[] PROGMEM =
{
  64,30, //width and height
  B00000011, B00000000, B00000000, B00000000, B01100000, B00000000, B00000000, B00000000, 
  B00000011, B00000000, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10001100, B00000000, B00110000, B00000000, B00000000, 
  B00000000, B00000000, B00000000, B10001100, B01100000, B00110000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B11100000, B00000000, B00000000, B11100000, B11000000, 
  B00000000, B00000000, B00000000, B11100000, B11000000, B00000000, B00000000, B11000000, 
  B01100000, B00000000, B00000000, B10000000, B11000000, B00000000, B00000000, B00000000, 
  B01100000, B00000000, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, 
  B00000011, B00000000, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00000011, B00000000, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10001100, B00000000, B00110000, B00000000, B00000000, 
  B00000000, B00000000, B00000000, B10001100, B01100000, B00110000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B10000000, B00000000, B00000000, B11100000, B11000000, 
  B00000000, B00000000, B00000000, B10000000, B11000000, B00000000, B00000000, B11000000, 
  B01100000, B00000000, B00000000, B10000000, B11000000, B00000000, B00000000, B00000000, 
  B01100000, B00000000, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, 
  B00000011, B00000000, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00000011, B00000000, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10000000, B00000000, B00000000, B00000000, B00000000, 
  B00011000, B00000011, B00000000, B10001100, B00000000, B00110000, B00000000, B00000000, 
  B00000000, B00000000, B00000000, B10001100, B01100000, B00110000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B10000000, B01100000, B00000000, B00000000, B00000000, 
  B00000000, B00001100, B00000000, B11100000, B00000000, B00000000, B11100000, B11000000, 
  B00000000, B00000000, B00000000, B01100000, B11000000, B00000000, B00000000, B11000000, 
  B01100000, B00000000, B00000000, B00000000, B11000000, B00000000, B00000000, B00000000, 
  B01100000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, 
};

void setup()
{
  gb.begin();
  gb.titleScreen(F("JezzBall"), logo);
  gb.display.persistence = false;
}

void draw()
{
  gb.display.fillScreen(BLACK);
  //board
  gb.display.setColor(WHITE);
  gb.display.fillRect(board_x, board_y, board_w, board_h);
  //line
  gb.display.setColor(BLACK);
  if (linestate == LINEEXPANDING)
  {
    if (line_h)
    {
      int pos = constrain(line_x - line_width, 0, LCDWIDTH - 1);
      int width = constrain(2 * line_width + 1, 0, LCDWIDTH);
      gb.display.drawFastHLine(pos, line_y, width);
    }
    else
    {
      int pos = constrain(line_y - line_width, 0, LCDHEIGHT - 1);
      int width = constrain(2 * line_width + 1, 0, LCDHEIGHT);
      gb.display.drawFastVLine(line_x, pos, width);
    }
  }
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
  if (gb.buttons.pressed(BTN_B))
  {
    linestate = LINEEXPANDING;
    line_x = cursor_x;
    line_y = cursor_y;
    line_h = cursor_h;
    line_width = 0;
  }
  if (gb.buttons.repeat(BTN_UP, 1))
    cursor_y -= 1;
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cursor_y += 1;
  if (gb.buttons.repeat(BTN_LEFT, 1))
    cursor_x -= 1;
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cursor_x += 1;

  cursor_x = constrain(cursor_x, 0, LCDWIDTH - 1);
  cursor_y = constrain(cursor_y, 0, LCDHEIGHT - 1);
}
/*
 * ---H
 * 
 * --y+h
 * 
 * --ly
 * 
 * --y
 * 
 * ---*
 */
void updategame()
{
  ball_x = ball_x + ball_vx;
  ball_y = ball_y + ball_vy;

  if (ball_x < board_x)
  {
    ball_vx = -ball_vx;
    gb.sound.playTick();
  }
  else if ((ball_x + ball_size) > board_x + board_w)
  {
    ball_vx = -ball_vx;
    gb.sound.playTick();
  }
  if (ball_y < board_y)
  {
    ball_vy = -ball_vy;
    gb.sound.playTick();
  }
  else if ((ball_y + ball_size) > board_y + board_h)
  {
    ball_vy = -ball_vy;
    gb.sound.playTick();
  }

  if (linestate == LINEEXPANDING)
  {
    line_width++;

    if ((line_h && line_y >= ball_y && line_y <= ball_y + ball_size && ball_x > line_x - line_width && ball_x < line_x + line_width) ||
      (!line_h && line_x >= ball_x && line_x <= ball_x + ball_size && ball_y > line_y - line_width && ball_y < line_y + line_width))
    {
      //lose life
      ///if no life : game over
      linestate = LINEIDLE;
    }
    else if (line_h && line_x - line_width <= board_x && line_x + line_width >= board_x + board_w)
    {
      if (line_y > board_y && line_y < board_y + board_h)
      {
        if (ball_y > line_y)
        {
          board_h = board_h + board_y - line_y - 1;
          board_y = line_y + 1;
        }
        else
          board_h = line_y - board_y;
      }
      //update score
      linestate = LINEIDLE;
    }
    else if (!line_h && line_y - line_width <= board_y && line_y + line_width >= board_y + board_h)
    {
      if (line_x > board_x && line_x < board_x + board_w)
      {
        if (ball_x > line_x)
        {
          board_w = board_w + board_x - line_x - 1;
          board_x = line_x + 1;
        }
        else
          board_w = line_x - board_x;
      }
      //update score
      linestate = LINEIDLE;
    }
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

