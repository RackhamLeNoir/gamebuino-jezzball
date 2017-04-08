#include <math.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

extern const byte font5x7[];
extern const byte font3x5[];

struct ball_t {
  int x;
  int y;
  int vx;
  int vy;
};
#define MAXBALLS 15
#define BALLSIZE 2

struct cursor_t {
  int x;
  int y;
  bool h;
};
cursor_t cursor;
#define CURSORSIZE 3

struct board_t {
  int x;
  int y;
  int w;
  int h;
  ball_t **balls;
  int nbballs;
};
board_t **boards;
int nbboards;

#define LINEIDLE      0
#define LINEEXPANDING 1

struct line_t {
  uint8_t state;
  int x;
  int y;
  int l;
  bool h;
  int board;
};
line_t line;

int score = 0;
int lives = 5;
int numballs = 0;
int levelscore = 0;
#define BOARDWIDTH (LCDWIDTH - 22)
#define BOARDHEIGHT LCDHEIGHT


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
  if (numballs < MAXBALLS)
    numballs++;
  levelscore = 0;
     
  ball_t **balls = (ball_t **)malloc(numballs * sizeof(ball_t *));
  memset(balls, 0, sizeof(balls));
  int i;
  for (i = 0 ; i < numballs ; i++)
  {
    balls[i] = (ball_t *)malloc(sizeof(ball_t));
    *(balls[i]) = { (int)random(BOARDWIDTH) - BALLSIZE, (int)random(BOARDHEIGHT) - BALLSIZE, 1, 1 };
  }

  //each board has at least one ball
  //there cannot be more boards than balls
  boards = (board_t **)malloc(numballs * sizeof(board_t *));
  nbboards = 1;
  memset(boards, 0, sizeof(boards));
  boards[0] = (board_t *)malloc(sizeof(board_t));
  *(boards[0]) = {0, 0, BOARDWIDTH, BOARDHEIGHT, balls, numballs};

  cursor = { BOARDWIDTH/2 - (CURSORSIZE - 1), BOARDHEIGHT/2 };
  line = { LINEIDLE, 0, 0, 0, 0, 0};
}

void setup()
{
  gb.begin();
  gb.titleScreen(F("JezzBall"), logo);
  //gb.display.persistence = false;
  gb.pickRandomSeed();

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

void draw()
{
  int i, j;
  gb.display.fillScreen(BLACK);
  //sidebar
  gb.display.setColor(WHITE);
  gb.display.fillRect(LCDWIDTH - 21, 0, 21, LCDHEIGHT);
  gb.display.setColor(BLACK);
  gb.display.cursorX = LCDWIDTH - 20;
  gb.display.cursorY = 5;
  gb.display.setFont(font3x5);
  gb.display.println("Score");
  gb.display.cursorX = LCDWIDTH - 4 * numlength(score);
  gb.display.println(score);
  gb.display.cursorX = LCDWIDTH - 20;
  gb.display.cursorY = 20;
  for (i = 0 ; i < lives ; i++)
    gb.display.print("\03");
  unsigned long percent = (unsigned long)levelscore * 100 / (BOARDWIDTH * BOARDHEIGHT);
  gb.display.cursorX = LCDWIDTH - 4 * (numlength(percent) + 1);
  gb.display.cursorY = 30;
  gb.display.print(percent);
  gb.display.print("%");

  //boards
  gb.display.setColor(WHITE);
  for (i = 0 ; i < nbboards ; i++)
    gb.display.fillRect(boards[i]->x, boards[i]->y, boards[i]->w, boards[i]->h);
  //line
  gb.display.setColor(BLACK);
  if (line.state == LINEEXPANDING)
  {
    if (line.h)
    {
      int pos = constrain(line.x - line.l, boards[line.board]->x, boards[line.board]->x + boards[line.board]->w - 1);
      int width = constrain(2 * line.l + 1, 0, boards[line.board]->w - (pos - boards[line.board]->x));
      gb.display.drawFastHLine(pos, line.y, width);
    }
    else
    {
      int pos = constrain(line.y - line.l, boards[line.board]->y, boards[line.board]->y + boards[line.board]->h - 1);
      int width = constrain(2 * line.l + 1, 0, boards[line.board]->h - (pos - boards[line.board]->y));
      gb.display.drawFastVLine(line.x, pos, width);
    }
  }
  //cursor
  gb.display.setColor(INVERT);
  if (cursor.h)
    gb.display.drawFastHLine(cursor.x - (CURSORSIZE/2), cursor.y, CURSORSIZE);
  else
    gb.display.drawFastVLine(cursor.x, cursor.y - (CURSORSIZE/2), CURSORSIZE);
  //balls
  gb.display.setColor(BLACK);
  for (i = 0 ; i < nbboards ; i++)
  {
    for (j = 0 ; j < boards[i]->nbballs ; j++)
      gb.display.fillRect(boards[i]->balls[j]->x, boards[i]->balls[j]->y, BALLSIZE, BALLSIZE);
  }
}

void manageinputs()
{
  if (gb.buttons.pressed(BTN_A))
    cursor.h = !cursor.h;
  if (gb.buttons.pressed(BTN_B))
  {
    int i;
    for (i = 0 ; i < nbboards ; i++)
    {
      if (gb.collidePointRect(cursor.x, cursor.y, boards[i]->x, boards[i]->y, boards[i]->w, boards[i]->h))
      {
        line.state = LINEEXPANDING;
        line.x = cursor.x;
        line.y = cursor.y; 
        line.l = 0;
        line.h = cursor.h;
        line.board = i;
        break;
      }
    }
  }
  if (gb.buttons.repeat(BTN_UP, 1))
    cursor.y -= 1;
  else if (gb.buttons.repeat(BTN_DOWN, 1))
    cursor.y += 1;
  if (gb.buttons.repeat(BTN_LEFT, 1))
    cursor.x -= 1;
  else if (gb.buttons.repeat(BTN_RIGHT, 1))
    cursor.x += 1;

  cursor.x = constrain(cursor.x, 0, LCDWIDTH - 1);
  cursor.y = constrain(cursor.y, 0, LCDHEIGHT - 1);
}

void updategame()
{
  int i, j;
  ball_t *ball;
  //move balls
  for (i = 0 ; i < nbboards ; i++)
  {
    for (j = 0 ; j < boards[i]->nbballs ; j++)
    {
      ball = boards[i]->balls[j];
      ball->x = ball->x + ball->vx;
      ball->y = ball->y + ball->vy;

      //bouncings with board
      if (ball->x < boards[i]->x)
      {
       ball->vx = -ball->vx;
        gb.sound.playTick();
      }
      else if ((ball->x + BALLSIZE) > boards[i]->x + boards[i]->w)
      {
        ball->vx = -ball->vx;
        gb.sound.playTick();
      }
      if (ball->y < boards[i]->y)
      {
        ball->vy = -ball->vy;
        gb.sound.playTick();
      }
      else if ((ball->y + BALLSIZE) > boards[i]->y + boards[i]->h)
      {
        ball->vy = -ball->vy;
        gb.sound.playTick();
      }
    }
  }

  if (line.state == LINEEXPANDING)
  {
    line.l++;

    //check collision with a ball
    int i;
    for (i = 0 ; i < boards[line.board]->nbballs ; i++)
    {
      if ((line.h && gb.collideRectRect(line.x - line.l, line.y, 2 * line.l + 1, 1, boards[line.board]->balls[i]->x, boards[line.board]->balls[i]->y, BALLSIZE, BALLSIZE)) || 
        (!line.h && gb.collideRectRect(line.x, line.y - line.l, 1, 2 * line.l + 1, boards[line.board]->balls[i]->x, boards[line.board]->balls[i]->y, BALLSIZE, BALLSIZE)))
      {
        //lose life
        lives--;
        ///if no life : game over
        line.state = LINEIDLE;
        return;
      }
    }

    ball_t **tempballs = boards[line.board]->balls;
    int tempnbballs = boards[line.board]->nbballs;
    board_t *newboard;

    //finished horizontal line
    if (line.h && line.x - line.l <= boards[line.board]->x && line.x + line.l >= boards[line.board]->x + boards[line.board]->w)
    {
      newboard = (board_t *)malloc(sizeof(board_t));
      newboard->x = boards[line.board]->x;
      newboard->y = line.y + 1;
      newboard->w = boards[line.board]->w;
      newboard->h = boards[line.board]->h + boards[line.board]->y - line.y - 1;
      newboard->balls = (ball_t **)malloc(numballs * sizeof(ball_t *));
      newboard->nbballs = 0;

      boards[line.board]->x = boards[line.board]->x;
      boards[line.board]->y = boards[line.board]->y;
      boards[line.board]->w = boards[line.board]->w;
      boards[line.board]->h = line.y - boards[line.board]->y;
      boards[line.board]->balls = (ball_t **)malloc(numballs * sizeof(ball_t *));
      boards[line.board]->nbballs = 0;
    }
    //finished vertical line
    else if (!line.h && line.y - line.l <= boards[line.board]->y && line.y + line.l >= boards[line.board]->y + boards[line.board]->h)
    {
      newboard = (board_t *)malloc(sizeof(board_t));
      newboard->x = line.x + 1;
      newboard->y = boards[line.board]->y;
      newboard->w = boards[line.board]->w + boards[line.board]->x - line.x - 1;
      newboard->h = boards[line.board]->h;
      newboard->balls = (ball_t **)malloc(numballs * sizeof(ball_t *));
      newboard->nbballs = 0;

      boards[line.board]->x = boards[line.board]->x;
      boards[line.board]->y = boards[line.board]->y;
      boards[line.board]->w = line.x - boards[line.board]->x;
      boards[line.board]->h = boards[line.board]->h;
      boards[line.board]->balls = (ball_t **)malloc(numballs * sizeof(ball_t *));
      boards[line.board]->nbballs = 0;
    }
    //line not finished
    else
      return;

    for (i = 0 ; i < tempnbballs ; i++)
    {
        if (gb.collideRectRect(boards[line.board]->x, boards[line.board]->y, boards[line.board]->w, boards[line.board]->h, 
          tempballs[i]->x, tempballs[i]->y, BALLSIZE, BALLSIZE))
        {
          boards[line.board]->balls[boards[line.board]->nbballs] = tempballs[i];
          boards[line.board]->nbballs++;
        }
        else
        {
          newboard->balls[newboard->nbballs] = tempballs[i];
          newboard->nbballs++;
        }
    }
    free(tempballs);
    if (newboard->nbballs == 0)
    {
      score += newboard->w * newboard->h;
      levelscore += newboard->w * newboard->h;
      free(newboard->balls);
      free(newboard);
    }
    else if (boards[line.board]->nbballs == 0)
    {
      score += boards[line.board]->w * boards[line.board]->h;
      levelscore += boards[line.board]->w * boards[line.board]->h;
      free(boards[line.board]->balls);
      free(boards[line.board]);
      boards[line.board] = newboard;
    }
    else
    {
      if (line.h)
      {
        score += boards[line.board]->w;
        levelscore += boards[line.board]->w;
      }
      else
      {
        score += boards[line.board]->h;
        levelscore += boards[line.board]->h;
      }
      boards[nbboards] = newboard;
      nbboards++;
    }

    line.state = LINEIDLE;
  }
}

void loop(){
  if(gb.update())
  {
    if (lives <= 0)
    {
      gb.display.cursorX = (LCDWIDTH - 53) / 2;
      gb.display.cursorY = 10;
      gb.display.setFont(font5x7);
      gb.display.print("Game Over");

      gb.display.cursorX = (LCDWIDTH - 29) / 2;
      gb.display.cursorY = 30;
      gb.display.println("Score");
      gb.display.cursorX = (LCDWIDTH - (6 * numlength(score) - 1)) / 2;
      gb.display.println(score);
    }
    else
    {
      updategame();
      manageinputs();
      draw();
    }
  }
}

