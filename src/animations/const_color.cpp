#include <animations/const_color.h>

ConstColorAnimation::~ConstColorAnimation() {

}

void ConstColorAnimation::InitFromJson(const JsonObject &animation_params) {
  JsonArray hsvColor = animation_params["hsv"];
  colorToFill = HSV(hsvColor);
}

void ConstColorAnimation::Render(float rel_time, int cycle_index) {
  for(std::vector<HSV *>::const_iterator it = pixels->begin(); it != pixels->end(); ++it) {
    HSV *pixel = *it;
    *pixel = colorToFill;
  }
}
