#include <animations/fill.h>

#include <math.h>

FillAnimation::~FillAnimation()
{
    delete fillStartPos;
    fillStartPos = NULL;

    delete fillEndPos;
    fillEndPos = NULL;
}

void FillAnimation::InitFromJson(const JsonObject &animation_params) {
  fillStartPos = FloatAnimationFactory(animation_params["fill_start_pos"]);
  fillEndPos = FloatAnimationFactory(animation_params["fill_end_pos"]);
}

void FillAnimation::Render(float rel_time, int cycle_index) {

    if(fillStartPos == NULL)
        return;
    if(fillEndPos == NULL)
        return;

    float currFillStartPos = fillStartPos->GetValue(rel_time);
    float currFillStartIndex = currFillStartPos * pixels->size();

    float currFillEndPos = fillEndPos->GetValue(rel_time);
    float currFillEndIndex = currFillEndPos * pixels->size();

    // make a smooth transition on the changing index
    int changeStartIndex = (int)floor(currFillStartIndex);
    if(changeStartIndex >= 0 && changeStartIndex < pixels->size()) {
        float changeAmount = fmod(currFillStartIndex, 1.0);
        (*pixels)[changeStartIndex]->val *= (1.0 - changeAmount);
    }

    int changeEndIndex = (int)floor(currFillEndIndex);
    if(changeEndIndex >= 0 && changeEndIndex < pixels->size()) {
        float changeAmount = fmod(currFillEndIndex, 1.0);
        (*pixels)[changeEndIndex]->val *= changeAmount;
    }

    // set pixels before start and after end to zero brightness
    for(int i = 0; i < changeStartIndex; i++) {
        (*pixels)[i]->val = 0.0;
    }
    for(int i = changeEndIndex + 1; i < pixels->size(); i++) {
        (*pixels)[i]->val = 0.0;
    }    

}
