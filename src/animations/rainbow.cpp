#include <animations/rainbow.h>

RainbowAnimation::~RainbowAnimation() {

    delete start_hue;
    start_hue = NULL;
    
    delete end_hue;
    end_hue = NULL;
}

void RainbowAnimation::InitFromJson(const JsonObject &animation_params) {
  start_hue = FloatAnimationFactory(animation_params["startHue"]);
  end_hue = FloatAnimationFactory(animation_params["endHue"]);
}

void RainbowAnimation::Render(float rel_time, int cycle_index) {

  if(start_hue == NULL || end_hue == NULL) {
    return;
  }

  const float curr_start_hue = start_hue->GetValue(rel_time);
  const float curr_end_hue = end_hue->GetValue(rel_time);
  const float curr_hue_diff = curr_end_hue - curr_start_hue;

  float numberOfPixels = (float)pixels->size();
  for(int i=0; i<pixels->size(); i++) {
    HSV *pixel = (*pixels)[i];
    float rel_pos = ((float)i / numberOfPixels);
    pixel->hue = curr_start_hue + curr_hue_diff * rel_pos;
    pixel->sat = 1.0f;
    pixel->val = 1.0f;
  }
}
