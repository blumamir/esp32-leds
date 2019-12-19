#include <animations/hue_shift_cycle.h>

HueShiftCycleAnimation::~HueShiftCycleAnimation()
{
    delete shift_amount;
    shift_amount = NULL;
}

void HueShiftCycleAnimation::InitFromJson(const JsonObject &animation_params) {
  shift_amount = DiscreteFloatFuncFactory(animation_params["shiftAmount"], this->repeatNum);
}

void HueShiftCycleAnimation::Render(float rel_time, int cycle_index) {

    if(shift_amount == NULL)
        return;

    float curr_shift_amount = shift_amount->GetValue(cycle_index);
    for(std::vector<HSV *>::const_iterator it = pixels->begin(); it != pixels->end(); ++it) {
        HSV *pixel = *it;
        pixel->hue += curr_shift_amount;
    }
}
