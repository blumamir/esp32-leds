#ifndef __RAND_BRIGHTNESS_H__
#define __RAND_BRIGHTNESS_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class RandBrightnessAnimation : public IAnimation {

public:
    ~RandBrightnessAnimation();

public:
    void InitFromJson(const JsonObject &animation_params);
    void Render(float rel_time, int cycle_index);
    void NewCycle(int cycleIndex);

private:
    void RecalculateValues();

private:
    float *brightPerPixel = nullptr;
  
};


#endif // __RAND_BRIGHTNESS_H__
