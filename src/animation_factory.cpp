#include <animation_factory.h>

#include <animations/const_color.h>
#include <animations/set_brightness.h>
#include <animations/hue_shift.h>
#include <animations/rainbow.h>
#include <animations/alternate.h>
#include <animations/fill.h>
#include <animations/snake.h>
#include <animations/alternate_coloring.h>
#include <animations/confetti.h>
#include <animations/rand_brightness.h>
#include <animations/rand_sat.h>
#include <animations/hue_shift_cycle.h>

#include <Arduino.h>

std::list<IAnimation *> *AnimationFactory::AnimationsListFromJson(JsonDocument &doc, const SegmentsStore &segmentStore) {

  JsonArray array = doc.as<JsonArray>();

  std::list<IAnimation *> *animationsList = new std::list<IAnimation *>();
  for(int i=0; i<array.size(); i++) {

    JsonObject anJsonConfig = array.getElement(i).as<JsonObject>();

    // allow pixels (json shortcut: "p") to be string, or list of string
    JsonVariant pixels = anJsonConfig["p"];
    if(pixels.is<const char*>()) {
      CreateAnimationAndAppend(anJsonConfig, pixels.as<const char *>(), animationsList, segmentStore);
    }

    else if(pixels.is<JsonArray>()) {
      JsonArray pixelsArr = pixels.as<JsonArray>();
      for (auto singlePixelsSegment : pixelsArr) {

        if(!singlePixelsSegment.is<const char*>()) 
          continue;

        const char *pixelsName = singlePixelsSegment.as<const char *>();
        CreateAnimationAndAppend(anJsonConfig, pixelsName, animationsList, segmentStore);
      }
    }
  }

  Serial.print("successfully read animations from file. found ");
  Serial.print(array.size());
  Serial.print(" animations. free heap size: ");
  Serial.println(esp_get_free_heap_size());

  return animationsList;
}

// void printHeap(const char *msg) {
//   static signed int lastHeap = 0;
//   int32_t currFreeHeap = esp_get_free_heap_size();
//   Serial.print(msg);
//   Serial.print(" free heap size: ");
//   Serial.print(currFreeHeap);
//   Serial.print(" diff: ");
//   Serial.println(lastHeap - currFreeHeap);
//   lastHeap = currFreeHeap;
// }

void AnimationFactory::CreateAnimationAndAppend(JsonObject anJsonConfig, const char *pixelsName, std::list<IAnimation *> *listToAppend, const SegmentsStore &segmentStore) 
{
  IAnimation *animationObj = CreateAnimation(anJsonConfig, pixelsName, segmentStore);
  if(animationObj == nullptr) 
    return;

  listToAppend->push_back(animationObj);
}

IAnimation *AnimationFactory::CreateAnimation(const JsonObject &animationAsJsonObj, const char *pixelsName, const SegmentsStore &segmentStore) {

  const std::vector<HSV *> *pixelsVec = segmentStore.GetPixelsVecBySegmentName(std::string(pixelsName));
  if(pixelsVec == nullptr) {
    Serial.print("animation ignored - pixels not in mapping: ");
    Serial.println(pixelsName);
    return nullptr;
  }


  IAnimation *generated_animation = NULL;

  const char * animation_name = animationAsJsonObj["t"];
  JsonObject animation_params = animationAsJsonObj["params"];

  if (strcmp(animation_name, "const") == 0) {
    generated_animation = new ConstColorAnimation();
  } else if(strcmp(animation_name, "brightness") == 0) {
    generated_animation = new SetBrightnessAnimation();
  } else if(strcmp(animation_name, "hue_shift") == 0) {
    generated_animation = new HueShiftAnimation();
  } else if(strcmp(animation_name, "rainbow") == 0) {
    generated_animation = new RainbowAnimation();
  } else if(strcmp(animation_name, "alternate") == 0) {
    generated_animation = new AlternateAnimation();
  } else if(strcmp(animation_name, "fill") == 0) {
    generated_animation = new FillAnimation();
  } else if(strcmp(animation_name, "snake") == 0) {
    generated_animation = new SnakeAnimation();
  } else if(strcmp(animation_name, "al") == 0) {
    generated_animation = new AlternateColoringAnimation();
  } else if(strcmp(animation_name, "confetti") == 0) {
    generated_animation = new ConfettiAnimation();
  } else if(strcmp(animation_name, "rand_brightness") == 0) {
    generated_animation = new RandBrightnessAnimation();
  } else if(strcmp(animation_name, "rand_sat") == 0) {
    generated_animation = new RandSatAnimation();
  } else if(strcmp(animation_name, "hue_shift_c") == 0) {
    generated_animation = new HueShiftCycleAnimation();
  } 
  

  if (generated_animation != NULL) {
    generated_animation->InitAnimation(pixelsVec, animationAsJsonObj);
    generated_animation->InitFromJson(animation_params);
  }



  return generated_animation;
}
