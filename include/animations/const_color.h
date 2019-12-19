#ifndef __CONST_COLOR_H__
#define __CONST_COLOR_H__

#include <animations/i_animation.h>


class ConstColorAnimation : public IAnimation {

public:
  ~ConstColorAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
  HSV colorToFill;
};


#endif // __CONST_COLOR_H__
