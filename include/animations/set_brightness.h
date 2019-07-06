#ifndef __SET_BROIGHTNESS_H__
#define __SET_BROIGHTNESS_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class SetBrightnessAnimation : public IAnimation {

public:
    ~SetBrightnessAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time);

private:
    IFloatFunc *brightness = NULL;
  
};


#endif // __SET_BROIGHTNESS_H__