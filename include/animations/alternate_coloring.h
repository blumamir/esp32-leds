#ifndef __ALTERNATE_COLORING_H__
#define __ALTERNATE_COLORING_H__

#include <animations/i_animation.h>

class AlternateColoringAnimation : public IAnimation {

public:
  ~AlternateColoringAnimation();

public:
  void InitFromJson(const JsonObject &animation_params);
  void Render(float rel_time, int cycle_index);

private:
    HSV color1, color2;
    int numberOfPixels = 3;
};


#endif // __ALTERNATE_COLORING_H__
