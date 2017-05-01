#include "line.h"

#include <Gamebuino.h>
extern Gamebuino gb;

#include "board.h"
extern int nbboards;
extern Board **boards;

Line::Line()
: _x(0), _y(0), _l(0), _h(true), _board(0)
{ 
}

void Line::start(uint8_t x, uint8_t y, uint8_t h, uint8_t board)
{
  _x = x;
  _y = y;
  _h = h;
  _l = 0;
  _board = board;
  _state = LINEEXPANDING;
}

bool Line::collision(Ball &ball)
{
  if (_h)
  {
    uint8_t min = boards[_board]->getX();
    uint8_t max = boards[_board]->getX() + boards[_board]->getW() - 1;
    int pos = constrain(_x - _l, min, max);
    int length = constrain(2 * _l + 1, 0, max - min);
    return gb.collideRectRect(pos, _y, length, 1, ball.getX(), ball.getY(), Ball::ballsize, Ball::ballsize);
  }
  else
  {
    uint8_t min = boards[_board]->getY();
    uint8_t max = boards[_board]->getY() + boards[_board]->getH() - 1;
    int pos = constrain(_y - _l, min, max);
    int length = constrain(2 * _l + 1, 0, max - min);
    return gb.collideRectRect(_x, pos, 1, length, ball.getX(), ball.getY(), Ball::ballsize, Ball::ballsize);
  }
}

bool Line::finished()
{
  Board *b = boards[_board];
  return ((_h && _x - _l <= b->getX() && _x + _l >= b->getX() + b->getW()) || (!_h && _y - _l <= b->getY() && _y + _l >= b->getY() + b->getH()));
}

void Line::draw() const
{
  if (_state == LINEIDLE)
    return;
  if (_h)
  {
    uint8_t min = boards[_board]->getX();
    uint8_t max = boards[_board]->getX() + boards[_board]->getW() - 1;
    int pos = constrain(_x - _l, min, max);
    int length = constrain(2 * _l + 1, 0, max - min);
    gb.display.drawFastHLine(pos, _y, length);
  }
  else
  {
    uint8_t min = boards[_board]->getY();
    uint8_t max = boards[_board]->getY() + boards[_board]->getH() - 1;
    int pos = constrain(_y - _l, min, max);
    int length = constrain(2 * _l + 1, 0, max - min);
    gb.display.drawFastVLine(_x, pos, length);
  }
}

