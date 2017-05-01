#pragma once

#include <Arduino.h>

#include "ball.h"
#include "line.h"

class Board {
  public:
    Board(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

    inline uint8_t getX() { return _x; }
    inline uint8_t getY() { return _y; }
    inline uint8_t getW() { return _w; }
    inline uint8_t getH() { return _h; }
    inline uint8_t getNbBalls() { return _nbballs; }
    inline Ball * getBall(uint8_t i) { return _balls[i]; }

    void initBalls(uint8_t nb);
    static void clearBalls();

    void moveBalls();

    Board *split(Line &line);
    
    void draw() const;
    void drawBalls() const;
    
  private: 
    uint8_t _x, _y, _w, _h;
    Ball **_balls;
    uint8_t _nbballs;

    static Ball ** ballsarray;
    static uint8_t totalballs;
};

