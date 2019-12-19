#ifndef __RAND_SAT_H__
#define __RAND_SAT_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class RandSatAnimation : public IAnimation {

public:
    ~RandSatAnimation();

public:
    void InitFromJson(const JsonObject &animation_params);
    void Render(float rel_time, int cycle_index);
    void NewCycle(int cycleIndex);

private:
    void RecalculateValues();

private:
    float *satPerPixel = nullptr;
  
};


#endif // __RAND_SAT_H__
