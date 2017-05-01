#include "ball.h"

#include <Gamebuino.h>
extern Gamebuino gb;

#include "line.h"

uint8_t Ball::ballsize = 2;
uint8_t Ball::maxballs = 15;

Ball::Ball(uint8_t x, uint8_t y)
: _x(x), _y(y)
{
  if (random(2))
    _vx = 1;
  else
    _vx = -1;
  if (random(2))
    _vy = 1;
  else
    _vy = -1;
}

void Ball::move(uint8_t gamex, uint8_t gamey, uint8_t gamew, uint8_t gameh)
{
  if ((_x == gamex + gamew - ballsize && _vx > 0) || (_x == gamex && _vx < 0))
  {
    _vx = -_vx;
    gb.sound.playTick();
  }
  else
    _x += _vx;

  if ((_y == gamey + gameh - ballsize && _vy > 0) || (_y == gamey && _vy < 0))
  {
    _vy = -_vy;
    gb.sound.playTick();
  }
  else
    _y += _vy;
}

void Ball::collide(Ball &ball)
{
  if (!gb.collideRectRect(_x, _y, ballsize, ballsize, ball._x, ball._y, ballsize, ballsize))
    return;
  //ball on the right
  if (_vx > 0 && ball._x == _x + ballsize - 1)
  {
    _vx = -1;
    ball._vx = 1;
  }
  //ball on the left
  else if (_vx < 0 && _x == ball._x + ballsize - 1)
  {
    _vx = 1;
    ball._vx = -1;
  }
  //ball on the top
  if (_vy > 0 && ball._y == _y + ballsize - 1)
  {
    _vy = -1;
    ball._vy = 1;
  }
  //ball on the bottom
  else if (_vy < 0 && _y == ball._y + ballsize - 1)
  {
    _vy = 1;
    ball._vy = -1;
  }
  gb.sound.playTick();
}

bool Ball::collide(Line &line)
{
  if (!line.collision(*this))
    return false;
    
  if (line.getH())
      _vy *= -1;
  else
      _vx *= -1;
  gb.sound.playTick();
  return true;
}


void Ball::draw() const
{
  gb.display.fillRect(_x, _y, ballsize, ballsize);
}

