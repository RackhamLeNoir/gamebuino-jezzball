#include "board.h"

#include <Gamebuino.h>
extern Gamebuino gb;

extern unsigned int level;
extern unsigned int score;
extern unsigned int levelscore;

extern int nbboards;
extern Board **boards;

uint8_t Board::totalballs = 0;
Ball ** Board::ballsarray = NULL;

Board::Board(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
:_x(x), _y(y), _w(w), _h(h), _nbballs(0), _balls(NULL)
{
}

void Board::initBalls(uint8_t nb)
{
  if (ballsarray)
    clearBalls();

  totalballs = nb;
  ballsarray = (Ball **)malloc(totalballs * sizeof(Ball *));
  _nbballs = nb;
  _balls = ballsarray;
  memset(ballsarray, 0, sizeof(ballsarray));
  for (uint8_t i = 0 ; i < totalballs ; i++)
    ballsarray[i] = new Ball(random(_w - Ball::ballsize), random(_h - Ball::ballsize));
}

void Board::clearBalls()
{
  if (!ballsarray)
    return;
  for (uint8_t i = 0 ; i < totalballs ; i++)
      delete ballsarray[i];
  free(ballsarray);
  ballsarray = NULL;
  totalballs = 0;
}

void Board::moveBalls()
{
  for (uint8_t i = 0 ; i < _nbballs ; i++)
  {
    _balls[i]->move(_x, _y, _w, _h);

    //bounce balls against each other
    for (uint8_t j = i + 1 ; j < _nbballs ; j++)
      _balls[i]->collide(*_balls[j]);
  }
}

Board *Board::split(Line &line)
{
    uint8_t tempnbballs = _nbballs;

    if (!line.finished())
      return;

    if (line.getH())
    {
      boards[nbboards] = new Board(_x, line.getY() + 1, _w, _h + _y - line.getY() - 1);
      boards[nbboards]->_balls = _balls + _nbballs;
      _h = line.getY() - _y;
    }
    else
    {
      boards[nbboards] = new Board(line.getX() + 1, _y, _w + _x - line.getX() - 1, _h);
      boards[nbboards]->_balls = _balls + _nbballs;
      _w = line.getX() - _x;
    }
    _nbballs = 0;

    while (_nbballs + boards[nbboards]->_nbballs < tempnbballs)
    {
        if (gb.collideRectRect(_x, _y, _w, _h, _balls[_nbballs]->getX(), _balls[_nbballs]->getY(), Ball::ballsize, Ball::ballsize))
          _nbballs++;
        else
        {
          Ball *temp = _balls[_nbballs];
          _balls[_nbballs] = _balls[tempnbballs - boards[nbboards]->_nbballs - 1];
          _balls[tempnbballs - boards[nbboards]->_nbballs - 1] = temp;
          boards[nbboards]->_balls--;
          boards[nbboards]->_nbballs++;
        }
    }
    //Check for empty boards
    if (boards[nbboards]->_nbballs == 0)
    {
      if (line.getH())
      {
        uint16_t s = boards[nbboards]->_w * (boards[nbboards]->_h + 1);
        score += s;
        levelscore += s;
      }
      else
      {
        uint16_t s = (boards[nbboards]->_w + 1) * boards[nbboards]->_h;
        score += s;
        levelscore += s;
      }
      boards[nbboards]->_balls = NULL;
      delete boards[nbboards];
    }
    else if (_nbballs == 0)
    {
      if (line.getH())
      {
        uint16_t s = _w * (_h + 1);
        score += s;
        levelscore += s;
      }
      else
      {
        uint16_t s = (_w + 1) * _h;
        score += s;
        levelscore += s;
      }
      _x = boards[nbboards]->_x;
      _y = boards[nbboards]->_y;
      _w = boards[nbboards]->_w;
      _h = boards[nbboards]->_h;
      _balls = boards[nbboards]->_balls;
      _nbballs = boards[nbboards]->_nbballs;
      boards[nbboards]->_balls = NULL;
      delete boards[nbboards];
    }
    else
    {
      if (line.getH())
      {
        score += _w;
        levelscore += _w;
      }
      else
      {
        score += _h;
        levelscore += _h;
      }
      nbboards++;
    }
}

void Board::draw() const
{
  gb.display.fillRect(_x, _y, _w, _h);
}

void Board::drawBalls() const
{
  for (uint8_t i = 0 ; i < _nbballs ; i++)
    _balls[i]->draw();
}

