#include "animations_container.h"

#include <SPIFFS.h>
#include <animation_factory.h>

const AnimationsContainer::AnimationsList *AnimationsContainer::SetFromJsonFile(const String &songName, JsonDocument &docForParsing, const SegmentsStore &segmentStore) {

  bool success = InitJsonDocFromFile(songName, docForParsing);
  if(!success) {
    return nullptr;
  }
  return AnimationFactory::AnimationsListFromJson(docForParsing, segmentStore);
}

bool AnimationsContainer::InitJsonDocFromFile(const String &songName, JsonDocument &docForParsing) 
{
  String fileName = String("/music/") + songName;
  Serial.print("about to read file from FS: ");
  Serial.println(fileName);
  File file = SPIFFS.open(fileName.c_str());
  if(!file) {
    Serial.print("could not find file in FS: ");
    Serial.println(songName);
    return false;
  }

  DeserializationError err = deserializeJson(docForParsing, file);
  file.close();
  if(err) {
    Serial.print("error deserializing animations json: ");
    Serial.println(err.c_str());
    return false;
  }

  Serial.print("loaded json from FS. with ");
  Serial.print(docForParsing.memoryUsage());
  Serial.println(" bytes");

  return true;
}
