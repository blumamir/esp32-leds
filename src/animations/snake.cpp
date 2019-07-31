#include <animations/snake.h>

#include <math.h>

SnakeAnimation::~SnakeAnimation()
{
    delete headPos;
    headPos = NULL;

    delete length;
    length = NULL;

    delete directionForward;
    directionForward = NULL;
}

void SnakeAnimation::InitFromJson(const JsonObject &animation_params) {
  headPos = FloatAnimationFactory(animation_params["headPos"]);
  length = FloatAnimationFactory(animation_params["length"]);
  directionForward = BooleanFuncFactory(animation_params["dir"]);
}

void SnakeAnimation::Render(float rel_time) {

    if(headPos == NULL || length == NULL || directionForward == NULL)
        return;

    bool currDir = directionForward->GetValue(rel_time);
    bool makeBackwardMirror = true;
    boolean useMirror = (directionForward == false && makeBackwardMirror);
    if(useMirror) {
        currDir = true;
    }
    float relLength = length->GetValue(rel_time);
    float relHeadPos = headPos->GetValue(rel_time);
    float absLength = relLength * pixels->size();
    float absHeadPos = relHeadPos * pixels->size();
    float absEndPos = (directionForward || makeBackwardMirror) ? (absHeadPos - absLength) : (absHeadPos + absLength);

    CalcAndSetBrightnessPerPixel(currDir, absEndPos, absLength, useMirror);
    // SmoothHeadIndex(currDir, absHeadPos, useMirror);        
}

void SnakeAnimation::CalcAndSetBrightnessPerPixel(bool currDir, float absEndArr, float absLength, bool useMirror) {
    for(int i=0; i<pixels->size(); i++) {
        // calculate the brightness at the current index
        float distanceFromEnd = currDir ? (i - absEndArr) : (absEndArr - i);
        float relPositionInSnake = distanceFromEnd / absLength;
        if(relPositionInSnake < 0.0 || relPositionInSnake > 1.0) {
            relPositionInSnake = 0.0;
        }

        // calculate mirror if needed and apply the brightness on the spike
        int indexToApply = useMirror ? pixels->size() - 1 - i : i;
        (*pixels)[indexToApply]->val *= relPositionInSnake;
    }
}

int SnakeAnimation::GetSmoothIndex(bool currDir, float absHeadPos, bool useMirror) {
    if(currDir) {
        int firstIndex = (int)floor(absHeadPos);
        return useMirror ? (pixels->size() - (firstIndex + 1)) : (firstIndex + 1);
    }
    else {
        int firstIndex = (int)ceil(absHeadPos);
        return (firstIndex - 1);
    }
}

void SnakeAnimation::SmoothHeadIndex(bool currDir, float absHeadPos, bool useMirror) {
    if(currDir) {
        int firstIndex = (int)floor(absHeadPos);
        float nextIndexAmount = fmod(absHeadPos, 1.0); // if the snake head covers a lot of the next index, this number will be high
        int indexToSmooth = useMirror ? (pixels->size() - (firstIndex + 1)) : (firstIndex + 1);
        if (indexToSmooth >= 0 && indexToSmooth < pixels->size()) {
            (*pixels)[indexToSmooth]->val *= nextIndexAmount;
        }
    }
    else {
        int firstIndex = (int)ceil(absHeadPos);
        double nextIndexAmount = 1.0 - fmod(absHeadPos, 1.0); // if the snake head covers a lot of the next index, this number will be high
        int indexToSmooth = (firstIndex - 1);
        if (indexToSmooth >= 0 && indexToSmooth < pixels->size()) {
            (*pixels)[indexToSmooth]->val *= nextIndexAmount;
        }
    }
}