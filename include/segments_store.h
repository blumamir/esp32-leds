#ifndef __SEGMENTS_STORE_H__
#define __SEGMENTS_STORE_H__

#include <map>
#include <string>
#include <vector>
#include <pb_decode.h>
#include <hsv.h>

#ifdef NUM_LEDS
#define MAX_SUPPORTED_PIXELS NUM_LEDS
#else
#define MAX_SUPPORTED_PIXELS 1500
#endif // NUM_LEDS

/*
In our context, segment is a subset of the pixels which the controller operate,
along with there order, and a unique name.

This class stores the segments configured for the controller.
It knows to parse and initialize itself from a protobuf message,
handle possible errors that might float during this proccess,
and return the pixels vector associated with each segment.
*/
class SegmentsStore
{
    public:
        /*
        Init the segments from a protobuf message, taken from file `f`, 
        and set the pixels to point to HSV from buffer `ledsArr` with size NUM_LEDS
        */
        bool InitFromFile(HSV ledsArr[], pb_istream_t stream);

        /*
        Givin a segment name `segmentName`, return the pixels in the segment, 
        as vector with pointers to the pixel HSV color
        */
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
        const char *m_errorDesc = "initialize not called yet";

        bool m_initialized = false;
};

#endif // __SEGMENTS_STORE_H__