#pragma once

#include <Arduino.h>

class Cursor {
  public:
    Cursor();

    void draw() const;

    uint8_t getX() const { return _x; }
    uint8_t getY() const { return _y; }
    bool getH() const { return _horizontal; }

    inline void up() { _y = constrain(_y - 1, 0, gameh - 1); }
    inline void down() { _y = constrain(_y + 1, 0, gameh - 1); }
    inline void right() { _x = constrain(_x + 1, 0, gamew - 1); }
    inline void left() { _x = constrain(_x - 1, 0, gamew - 1); }
    inline void rotate() { _horizontal = !_horizontal; }
    
  private:
    uint8_t _x, _y;
    bool _horizontal;

    static uint8_t gamew, gameh;
    static uint8_t cursorsize;
};

