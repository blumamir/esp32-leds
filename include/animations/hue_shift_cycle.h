#ifndef __HUE_SHIFT_CYCLE_H__
#define __HUE_SHIFT_CYCLE_H__

#include <animations/i_animation.h>
#include <discrete_float_func/i_discrete_float_func.h>

class HueShiftCycleAnimation : public IAnimation {

public:
    ~HueShiftCycleAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    IDiscreteFloatFunc *shift_amount = NULL;
  
};


#endif // __HUE_SHIFT_CYCLE_H__
