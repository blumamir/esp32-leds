#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>
#include <boolean_func/i_boolean_func.h>

class SnakeAnimation : public IAnimation {

public:
    ~SnakeAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    int GetSmoothIndex(bool currDir, float absHeadPos);
    void CalcAndSetBrightnessPerPixel(bool directionForward, float absEndArr, float absLength);
    void SmoothHeadIndex(bool currDir, float absHeadPos);

private:
    IFloatFunc *headPos = NULL;
    IFloatFunc *length = NULL;

    // head is the place where the max brightness pixel of the snake is.
    // then the snake tail can go either forward or backwards
    IBooleanFunc *directionForward = NULL;
  
};


#endif // __SNAKE_H__
