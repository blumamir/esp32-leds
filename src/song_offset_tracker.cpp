#include "song_offset_tracker.h"

#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <Arduino.h>

#ifndef TIME_SYNC_SERVER_PORT
#define TIME_SYNC_SERVER_PORT 12321
#endif // TIME_SYNC_SERVER_PORT


void SongOffsetTracker::setup() {
    IPAddress timeServerHost;
    timeServerHost.fromString(TIME_SERVER_IP);
    timesync.setup(timeServerHost, TIME_SYNC_SERVER_PORT);
}

void SongOffsetTracker::loop(bool *isClockChanged, bool *clockFirstValid) {
    timesync.loop(isClockChanged, clockFirstValid);
}

bool SongOffsetTracker::HandleCurrentSongMessage(char *data) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, data);

    bool currSongChanged = false;

    bool wasSongPlaying = isSongPlaying;
    isSongPlaying = doc["song_is_playing"];
    if(isSongPlaying) {
        // saving this value as int64 for easier calculations later
        songStartTimeEpoch = doc["start_time_millis_since_epoch"];
        const char *fileIdPtr = doc["file_id"];
        currSongChanged = strcmp(fileIdPtr, fileNameFromPlayer.c_str()) != 0;
        fileNameFromPlayer = fileIdPtr;

        fileName = fileNameFromPlayer;
        int dotIndex = fileName.indexOf('.');
        if(dotIndex >= 0) {
            fileName.remove(dotIndex);
        }
    }
    else  {
        fileName = "";
        currSongChanged = (wasSongPlaying != isSongPlaying);
    }

    return currSongChanged;
}

int32_t SongOffsetTracker::GetSongStartTime() {
    if(!timesync.isTimeValid()) {
        return 0;
    }

    int32_t songStartTime = (int32_t)(songStartTimeEpoch - timesync.getEspStartTimeMs());
    return songStartTime;
}

