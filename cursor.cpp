#include "cursor.h"

#include <Gamebuino.h>
extern Gamebuino gb;

uint8_t Cursor::cursorsize = 3;
uint8_t Cursor::gamew = LCDWIDTH - 22;
uint8_t Cursor::gameh = LCDHEIGHT;

Cursor::Cursor()
:_x(gamew/2 - cursorsize + 1), _y(gameh/2), _horizontal(true)
{ 
}

void Cursor::draw() const
{
  if (_horizontal)
    gb.display.drawFastHLine(_x - (cursorsize/2), _y, cursorsize);
  else
    gb.display.drawFastVLine(_x, _y - (cursorsize/2), cursorsize);
}

