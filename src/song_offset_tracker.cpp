#include "song_offset_tracker.h"

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>

SongOffsetTracker::SongOffsetTracker() : songDataMutex(xSemaphoreCreateMutex())
{

}

void SongOffsetTracker::setup(const IPAddress &timeServerIP, uint16_t timeServerPort) {
    timesync.setup(timeServerIP, 123);
}

void SongOffsetTracker::loop() {
    timesync.loop();
}

void SongOffsetTracker::HandleCurrentSongMessage(char *data) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, data);

    xSemaphoreTake(songDataMutex, portMAX_DELAY);
    {
        isSongPlaying = doc["song_is_playing"];
        if(isSongPlaying) {
            songStartTimeEpoch = doc["start_time_millis_since_epoch"];
            const char *fileIdPtr = doc["file_id"];
            fileName = fileIdPtr;

            int dotIndex = fileName.indexOf('.');
            if(dotIndex >= 0) {
                fileName.remove(dotIndex);
            }
        }
    }
    xSemaphoreGive(songDataMutex);
}

bool SongOffsetTracker::GetCurrentSongDetails(unsigned long currentEspMillis, CurrentSongDetails *outSongDetails) {

    outSongDetails->valid = false;

    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&mux);
    if(!timesync.m_isTimeValid) {
        portEXIT_CRITICAL(&mux);
        return false;
    }

    xSemaphoreTake(songDataMutex, portMAX_DELAY);

    // when esp's millis() function returned this time (songStartTime), the song started
    uint32_t songStartTime = (uint32_t)(songStartTimeEpoch - timesync.m_espStartTimeMs);
    portEXIT_CRITICAL(&mux);

    outSongDetails->valid = true;
    outSongDetails->offsetMs = currentEspMillis - songStartTime;
    outSongDetails->songName = fileName;

    xSemaphoreGive(songDataMutex);

    return true;
}
