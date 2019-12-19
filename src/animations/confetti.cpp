#include <animations/confetti.h>

ConfettiAnimation::ConfettiAnimation() {
    brightPerPixel = new float[pixels->size()];
    for(int i=0; i<pixels->size(); i++) {
        brightPerPixel[i] = 1.0;
    }
}

ConfettiAnimation::~ConfettiAnimation() {

    delete[] brightPerPixel;
    brightPerPixel = nullptr;

    delete fadeAmount;
    fadeAmount = nullptr;
}

void ConfettiAnimation::InitFromJson(const JsonObject &animation_params) {
    fadeAmount = FloatAnimationFactory(animation_params["fade"]);
}

void ConfettiAnimation::Render(float rel_time, int cycle_index) {
    Fade(rel_time);

    for(int i=0; i<pixels->size(); i++) {
        HSV *pixel = (*pixels)[i];
        pixel->val *= brightPerPixel[i];
    }
}

void ConfettiAnimation::Fade(float relTime) {
    // float currFadeAmount = fadeAmount->GetValue(relTime);
    
}
