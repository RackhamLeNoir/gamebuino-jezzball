#include <math.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

#include "cursor.h"
Cursor cursor;

#include "ball.h"
#include "board.h"
int nbboards;
Board **boards;

#include "line.h"
Line line;


extern const byte font5x7[];
extern const byte font3x5[];

unsigned int highscore = 0;
unsigned int score = 0;
unsigned int lives = 5;
unsigned int level = 0;
unsigned int levelscore = 0;

#define BOARDWIDTH (LCDWIDTH - 22)
#define BOARDHEIGHT LCDHEIGHT
#define LEVELCLEAR 2300//((int)((unsigned long)(BOARDWIDTH * BOARDHEIGHT) * 75/ 100))

const uint8_t logo[] PROGMEM =
{
  64,30, //width and height
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000111,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B01001111,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001, B11001111,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00110001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00000000, B11110001, B11110001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00001111, B11110011, B11110001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00011110, B11110110, B01110001, B11001110,
  B00000000, B00000000, B00000000, B00000000, B00011100, B11100110, B01110001, B11001000,
  B00000000, B00000000, B00000000, B00000000, B00011111, B11100111, B01110011, B10000000,
  B00000000, B00000000, B00000000, B00000001, B10011111, B11110111, B01010000, B00000000,
  B00000000, B00000000, B00000000, B00000111, B10011100, B11110111, B11000000, B00000000,
  B00000000, B00000000, B00000011, B00011110, B00111100, B11100011, B10000000, B00000000,
  B00000000, B11000000, B00001111, B01111110, B00111111, B11000000, B00000000, B00000000,
  B00000011, B11000000, B00111110, B00001100, B00111111, B00000000, B00000000, B00000000,
  B00001111, B10011100, B11111110, B00001111, B10111000, B00000000, B00000000, B00000000,
  B00111111, B00111110, B00011100, B00011111, B10000000, B00000000, B00000000, B00000000,
  B01111110, B01110011, B00011000, B00111100, B00000000, B00000000, B00000000, B00000000,
  B11111110, B01111111, B00011111, B01100000, B00000000, B00000000, B00000000, B00000000,
  B11001110, B11111100, B00111110, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00001110, B11110000, B01110000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00001110, B11111111, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00011110, B01111100, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00111100, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B01111000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B01110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
};

void preparelevel()
{
  level++;
  levelscore = 0;

  int numballs = constrain(level, 1, Ball::maxballs);

  //each board has at least one ball
  //there cannot be more boards than balls
  boards = (Board **)malloc(numballs * sizeof(Board *));
  nbboards = 1;
  memset(boards, 0, sizeof(boards));
  boards[0] = new Board(0, 0, BOARDWIDTH, BOARDHEIGHT);
  boards[0]->initBalls(numballs);
  line.setState(Line::LINEIDLE);
}

void clearlevel()
{
  for (uint8_t i = 0 ; i < nbboards ; i++)
    delete boards[i];
  free(boards);
  Board::clearBalls();
  line.setState(Line::LINEIDLE);
}

unsigned char magic[3] = {42, 12, 28};

unsigned int get_highscore()
{
  unsigned char temp;
  for (uint8_t i = 0 ; i < 3 ; i++)
  {
    EEPROM.get(i, temp);
    if (magic[i] != temp)
    {
      for (int j = 0 ; j < 3 ; j++)
        EEPROM.put(0, magic[j]);
      highscore = 0;
      return;
    }
  }
  EEPROM.get(3, highscore);
}

void set_highscore()
{
  EEPROM.put(3, highscore);
}

void setup()
{
  gb.begin();
  gb.titleScreen(F(""), logo);
  gb.pickRandomSeed();
  gb.battery.show = false;

  get_highscore();

  preparelevel();
}

int numlength(int number)
{
  if (number == 0)
    return 1;
  else if (number > 0)
    return floor(log10(number)) + 1;
  return floor(log10(number)) + 2;
}

void drawgame()
{
  gb.display.fillScreen(BLACK);
  //sidebar
  gb.display.setColor(WHITE);
  gb.display.fillRect(LCDWIDTH - 21, 0, 21, LCDHEIGHT);
  gb.display.setColor(BLACK);
  gb.display.cursorX = LCDWIDTH - 20;
  gb.display.cursorY = 1;
  gb.display.setFont(font3x5);
  gb.display.println("Level");
  gb.display.cursorX = LCDWIDTH - 4 * numlength(level);
  gb.display.println(level);
  gb.display.cursorX = LCDWIDTH - 20;
  gb.display.println("Score");
  gb.display.cursorX = LCDWIDTH - 4 * numlength(score);
  gb.display.println(score);
  gb.display.cursorX = LCDWIDTH - 20;
  for (uint8_t i = 0 ; i < lives ; i++)
    gb.display.print("\03");
  unsigned long percent = (unsigned long)levelscore * 100 / (BOARDWIDTH * BOARDHEIGHT);
  gb.display.cursorX = LCDWIDTH - 4 * (numlength(percent) + 1);
  gb.display.cursorY = 31;
  gb.display.print(percent);
  gb.display.print("%");

  //boards
  gb.display.setColor(WHITE);
  for (uint8_t i = 0 ; i < nbboards ; i++)
    boards[i]->draw();
    
  //line
  gb.display.setColor(BLACK);
  line.draw();
  
  //cursor
  gb.display.setColor(INVERT);
  cursor.draw();
  
  //balls
  gb.display.setColor(BLACK);
  for (uint8_t i = 0 ; i < nbboards ; i++)
    boards[i]->drawBalls();
}

void drawgameover()
{
  gb.display.cursorX = (LCDWIDTH - 53) / 2;
  gb.display.cursorY = 10;
  gb.display.setFont(font5x7);
  gb.display.print("Game Over");

  if (score > highscore)
  {
    gb.display.cursorX = (LCDWIDTH - 59) / 2;
    gb.display.cursorY = 30;
    gb.display.println("High score");
    gb.display.cursorX = (LCDWIDTH - (6 * (numlength(score) + 2) - 1)) / 2;
    gb.display.print("\17");
    gb.display.print(score);
    gb.display.print("\17");
  }
  else
  {
    gb.display.cursorX = (LCDWIDTH - 29) / 2;
    gb.display.cursorY = 30;
    gb.display.println("Score");
    gb.display.cursorX = (LCDWIDTH - (6 * numlength(score) - 1)) / 2;
    gb.display.println(score);
  }
}

void drawlevelclear()
{
  gb.display.cursorX = (LCDWIDTH - 29) / 2;
  gb.display.cursorY = 5;
  gb.display.setFont(font5x7);
  gb.display.println("Level");
  gb.display.cursorX = (LCDWIDTH - (6 * numlength(level) - 1)) / 2;
  gb.display.println(level);
  gb.display.cursorX = (LCDWIDTH - 41) / 2;
  gb.display.println("cleared");

  gb.display.cursorX = (LCDWIDTH - 41) / 2;
  gb.display.cursorY = 35;
  gb.display.println("Press \25");
}

void inputsgame()
{
  if (gb.buttons.pressed(BTN_A) && line.getState() == Line::LINEIDLE)
  {
    int i;
    for (i = 0 ; i < nbboards ; i++)
    {
      if (gb.collidePointRect(cursor.getX(), cursor.getY(), boards[i]->getX(), boards[i]->getY(), boards[i]->getW(), boards[i]->getH()))
        
      {
        if (cursor.getH())
          line.start(cursor.getX(), cursor.getY(), true, i);
        else
          line.start(cursor.getX(), cursor.getY(), false, i);
        break;
      }
    }
  }

  if (gb.buttons.pressed(BTN_B))
    cursor.rotate();

  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F(""), logo);

  if (gb.buttons.repeat(BTN_UP, 1))
    cursor.up();
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cursor.down();
  if (gb.buttons.repeat(BTN_LEFT, 1))
    cursor.left();
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cursor.right();
}

void inputsgameover()
{
  if (gb.buttons.pressed(BTN_A))
  {
    if (score > highscore)
    {
      highscore = score;
      set_highscore();
    }
    score = 0;
    level = 0;
    lives = 5;
    clearlevel();
    preparelevel();
  }
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F(""), logo);
}

void inputslevelclear()
{
  if (gb.buttons.pressed(BTN_A))
  {
    clearlevel();
    preparelevel();
  }
  if(gb.buttons.pressed(BTN_C))
    gb.titleScreen(F(""), logo);
}

void updategame()
{
  for (uint8_t i = 0 ; i < nbboards ; i++)
    boards[i]->moveBalls();

  if (line.getState() == Line::LINEEXPANDING)
  {
    line.grow();

    //check collision with a ball
    for (uint8_t i = 0 ; i < boards[line.getBoard()]->getNbBalls() ; i++)
    {
      if (boards[line.getBoard()]->getBall(i)->collide(line))
      {
        lives--;
        line.setState(Line::LINEIDLE);
        return;
      }
    }

    if (line.finished())
    {
      Board *newboard = boards[line.getBoard()]->split(line);
  
      if (newboard)
      {
        boards[nbboards] = newboard;
        nbboards++;
      }
      line.setState(Line::LINEIDLE);
    }
  }
}

void loop(){
  if(gb.update())
  {
    if (lives <= 0)
    {
      drawgameover();
      inputsgameover();
    }
    else if (levelscore >= LEVELCLEAR)
    {
      drawlevelclear();
      inputslevelclear();
    }
    else
    {
      updategame();
      inputsgame();
      drawgame();
    }
  }
}

