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
        bool InitFromFile(HSV ledsArr[], File &f);
        const std::vector<HSV *> *GetPixelsVecBySegmentName(const std::string &segmentName) const;

    public:
        bool IsInitialized() const { return m_initialized; }
        const char *GetErrorDescription() const { return m_errorDesc; }

    public:
        typedef std::vector<uint32_t> IndicesVec;

    public:
        bool AddSegment(const std::string &segmentName, HSV ledsArr[], IndicesVec &indicesVecStorage, uint32_t totalPixels);

    private:
        bool ValidateNumberOfPixels(uint32_t totalPixels);

    private:
        // the vector is stored as pointer, so it will be allocated once and not move around
        typedef std::map<std::string, const std::vector<HSV *> *> SegmentsMap;
        SegmentsMap m_segmentsMap;

    private:

        // this member will point to a static const char * string 
        // with human description of an error if occured.
        // it is also an indicator for success or failure of class initialization
        const char *m_errorDesc = NULL;

        bool m_initialized = false;
};

#endif // __SEGMENTS_STORE_H__