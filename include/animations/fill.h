#ifndef __FILL_H__
#define __FILL_H__

#include <animations/i_animation.h>
#include <float_func/i_float_func.h>

class FillAnimation : public IAnimation {

public:
    ~FillAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    IFloatFunc *fillStartPos = NULL;
    IFloatFunc *fillEndPos = NULL;
  
};


#endif // __FILL_H__
