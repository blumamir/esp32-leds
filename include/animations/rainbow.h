#ifndef __RAINBOW_H__
#define __RAINBOW_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>


class RainbowAnimation : public IAnimation {

public:
    ~RainbowAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
  IFloatFunc *start_hue = NULL;
  IFloatFunc *end_hue = NULL;
};


#endif // __RAINBOW_H__
