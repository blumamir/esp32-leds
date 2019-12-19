#include <animations/hue_shift.h>

HueShiftAnimation::~HueShiftAnimation()
{
    delete shift_amount;
    shift_amount = NULL;
}

void HueShiftAnimation::InitFromJson(const JsonObject &animation_params) {
  shift_amount = FloatAnimationFactory(animation_params["shiftAmount"]);
}

void HueShiftAnimation::Render(float rel_time, int cycle_index) {

    if(shift_amount == NULL)
        return;

    float curr_shift_amount = shift_amount->GetValue(rel_time);
    for(std::vector<HSV *>::const_iterator it = pixels->begin(); it != pixels->end(); ++it) {
        HSV *pixel = *it;
        pixel->hue += curr_shift_amount;
    }
}
