#ifndef __ANIMATIONS_CONTAINER_H__
#define __ANIMATIONS_CONTAINER_H__

#include <list>
#include <map>

#include <ArduinoJson.h>

#include <hsv.h>
#include <animations/i_animation.h>
#include <segments_store.h>

class AnimationsContainer 
{

public:
    typedef std::list<IAnimation *> AnimationsList;

public:
    const AnimationsList *SetFromJsonFile(const String &songName, JsonDocument &docForParsing, const SegmentsStore &segmentStore);

private:
    static bool InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing);

};


#endif // __ANIMATIONS_CONTAINER_H__