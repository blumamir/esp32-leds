#ifndef __HUE_SHIFT_H__
#define __HUE_SHIFT_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class HueShiftAnimation : public IAnimation {

public:
    ~HueShiftAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    IFloatFunc *shift_amount = NULL;
  
};


#endif // __HUE_SHIFT_H__
