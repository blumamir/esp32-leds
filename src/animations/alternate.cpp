#include <animations/alternate.h>

AlternateAnimation::~AlternateAnimation() {
  
  delete alternateState;
  alternateState = NULL;

  delete hueShift;
  hueShift = NULL;
}

void AlternateAnimation::InitFromJson(const JsonObject &animation_params) {
    numberOfPixels = animation_params["numPix"];
    alternateState = BooleanFuncFactory(animation_params["stateFunc"]);
    hueShift = FloatAnimationFactory(animation_params["hueShiftFunc"]);
}

void AlternateAnimation::Render(float rel_time, int cycle_index) {

    bool currState = alternateState->GetValue(rel_time);
    double currHueShift = hueShift->GetValue(rel_time);

    int i=0;
    for(std::vector<HSV *>::const_iterator it = pixels->begin(); it != pixels->end(); ++it) {
        bool pixelState = (i % (2 * numberOfPixels)) < numberOfPixels;
        if(currState ^ pixelState) {
            HSV *pixel = *it;
            pixel->hue += currHueShift;
        }
    }
}
