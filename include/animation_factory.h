#ifndef __ANIMATION_FACTORY_H__
#define __ANIMATION_FACTORY_H__

#include <animations/i_animation.h>
#include <hsv.h>
#include <segments_store.h>

#include <list>
#include <ArduinoJson.h>

class AnimationFactory {

public:
  static std::list<IAnimation *> *AnimationsListFromJson(JsonDocument &doc, const SegmentsStore &segmentStore);
  static void CreateAnimationAndAppend(JsonObject anJsonConfig, const char *pixelsName, std::list<IAnimation *> *listToAppend, const SegmentsStore &segmentStore);
  static IAnimation *CreateAnimation(const JsonObject &animationAsJsonObj, const char *pixelsName, const SegmentsStore &segmentStore);

};

#endif // __ANIMATION_FACTORY_H__
