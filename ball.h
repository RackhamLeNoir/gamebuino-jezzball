#pragma once

#include <Arduino.h>

class Line;

class Ball {
  public:
    Ball(uint8_t x, uint8_t y);

    void draw() const;
    
    void move(uint8_t gamex, uint8_t gamey, uint8_t gamew, uint8_t gameh);

    void collide(Ball &ball);
    bool collide(Line &ball);

    inline uint8_t getX() { return _x; }
    inline uint8_t getY() { return _y; }

    static uint8_t ballsize;
    static uint8_t maxballs;

  private:
    uint8_t _x, _y;
    int8_t _vx, _vy;
};

