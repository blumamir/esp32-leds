#include <animations/rand_brightness.h>

RandBrightnessAnimation::~RandBrightnessAnimation()
{
    delete[] brightPerPixel;
    brightPerPixel = NULL;
}

void RandBrightnessAnimation::InitFromJson(const JsonObject &animation_params) {
    brightPerPixel = new float[pixels->size()];
    RecalculateValues();
}

void RandBrightnessAnimation::Render(float rel_time, int cycle_index) {
    for(int i=0; i<pixels->size(); i++) {
        (*pixels)[i]->val *= brightPerPixel[i];
    }
}

void RandBrightnessAnimation::NewCycle(int cycleIndex)
{
    RecalculateValues();
}

void RandBrightnessAnimation::RecalculateValues()
{
    for(int i=0; i<pixels->size(); i++) {
        brightPerPixel[i] = random(5) / 4.0f;
    }
}
