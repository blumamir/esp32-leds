#ifndef __I_ANIMATIONS_H__
#define __I_ANIMATIONS_H__

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <hsv.h>

class IAnimation {

public: virtual ~IAnimation() { }

public:
  virtual void InitFromJson(const JsonObject &animation_params) = 0;

protected:
  virtual void Render(float rel_time) = 0;

  // animation can be notified via this virtual method on a new cycle that starts.
  // each derived class can choose to implement this function if it likes, or just ignore the event
  virtual void NewCycle(int cycleIndex) {}

public:

  void Render(unsigned long curr_time) {

    float rel_time = ((float)(curr_time - start_time) / (float)(end_time - start_time));  
    if(this->repeatNum > 0.0f) { // has repeats - change relTime

      float cycleTime = rel_time * this->repeatNum;
      rel_time = fmod(cycleTime, 1.0f);
      int currCycleIndex = floor(cycleTime);

      if(currCycleIndex != this->lastCycleIndex) {
        this->lastCycleIndex = currCycleIndex;
        NewCycle(currCycleIndex);
      }

      // check if a "repeat" function should be rendered for this time
      if(rel_time < this->repeatStart || rel_time > this->repeatEnd)
        return;

    }
    Render(rel_time);
  }

  void InitAnimation(const std::vector<HSV *> *pixels, const JsonObject &animationAsJsonObj) {
    this->pixels = pixels;
    this->start_time = animationAsJsonObj["s"];
    this->end_time = animationAsJsonObj["e"];
    this->repeatNum = animationAsJsonObj["rep_num"]; // will be 0.0f if missing
    this->repeatStart = animationAsJsonObj["rep_s"]; // will be 0.0f if missing
    this->repeatEnd = animationAsJsonObj["rep_e"]; // will be 0.0f if missing
  }

  bool IsActive(unsigned long curr_time) {
    return curr_time >= this->start_time && curr_time < this->end_time;
  }

protected:
  unsigned long start_time, end_time;
  float repeatNum, repeatStart, repeatEnd;
  const std::vector<HSV *> *pixels;

  int lastCycleIndex = -1;
};

#endif // __I_ANIMATIONS_H__
