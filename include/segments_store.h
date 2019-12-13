#ifndef __SEGMENTS_STORE_H__
#define __SEGMENTS_STORE_H__

#include <map>
#include <string>
#include <vector>
#include <SPIFFS.h>
#include <hsv.h>

#ifdef NUM_LEDS
#define MAX_SUPPORTED_PIXELS NUM_LEDS
#else
#define MAX_SUPPORTED_PIXELS 1500
#endif // NUM_LEDS


class SegmentsStore
{
    public:
        int InitFromFile(HSV ledsArr[], File &f);

    public:
        typedef std::map<std::string, std::vector<HSV *> > SegmentsMap;
        SegmentsMap m_segmentsMap;
};

#endif // __SEGMENTS_STORE_H__