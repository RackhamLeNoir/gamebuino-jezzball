#pragma once

#include <Arduino.h>

class Ball;

class Line {
  public:
    enum line_state {
      LINEIDLE,
      LINEEXPANDING
    };

    Line();

    void draw() const;

    void start(uint8_t x, uint8_t y, uint8_t h, uint8_t board);

    inline void grow() { _l++; }

    bool finished();

    bool collision(Ball &ball);

    inline line_state getState () { return _state; }
    inline void setState(line_state s) { _state = s; }

    inline uint8_t getBoard() { return _board; }
    inline bool getH() { return _h; }
    inline uint8_t getX() { return _x; }
    inline uint8_t getY() { return _y; }

  private:
    line_state _state;
    uint8_t _x, _y, _l;
    bool _h;
    uint8_t _board;
};

