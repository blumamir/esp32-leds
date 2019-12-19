#ifndef __ALTERNATE_H__
#define __ALTERNATE_H__

#include <animations/i_animation.h>

#include <boolean_func/i_boolean_func.h>
#include <float_func/i_float_func.h>

class AlternateAnimation : public IAnimation {

public:
  ~AlternateAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    IBooleanFunc *alternateState;
    int numberOfPixels;
    IFloatFunc *hueShift;
};


#endif // __ALTERNATE_H__
