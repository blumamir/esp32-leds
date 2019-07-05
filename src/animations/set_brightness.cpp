#include <animations/set_brightness.h>

void SetBrightnessAnimation::InitFromJson(const JsonObject &animation_params) {
  brightness = FloatAnimationFactory(animation_params["brightness"]);
}

void SetBrightnessAnimation::Render(float rel_time) {

    Serial.println(rel_time);
    if(brightness == NULL)
        return;

    float val_mult_factor = brightness->GetValue(rel_time);
    for(std::vector<HSV *>::const_iterator it = pixels.begin(); it != pixels.end(); ++it) {
        HSV *pixel = *it;
        pixel->val *= val_mult_factor;
    }
}
