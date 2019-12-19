#include <animations/rand_sat.h>

RandSatAnimation::~RandSatAnimation()
{
    delete[] satPerPixel;
    satPerPixel = nullptr;
}

void RandSatAnimation::InitFromJson(const JsonObject &animation_params) {
    satPerPixel = new float[pixels->size()];
    RecalculateValues();
}

void RandSatAnimation::Render(float rel_time, int cycle_index) {
    for(int i=0; i<pixels->size(); i++) {
        (*pixels)[i]->sat *= satPerPixel[i];
    }
}

void RandSatAnimation::NewCycle(int cycleIndex)
{
    RecalculateValues();
}

void RandSatAnimation::RecalculateValues()
{
    for(int i=0; i<pixels->size(); i++) {
        satPerPixel[i] = 0.5f + random(5) / 8.0f;
    }
}
