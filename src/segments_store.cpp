#include <segments_store.h>

#include <pb_common.h>
#include <pb_decode.h>
#include <object_config.pb.h>

bool DecodeSegmentIndex_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    if(stream == NULL || field->tag != Segment_indices_tag)
        return false;

    std::vector<uint32_t> *arr = (std::vector<uint32_t> *)(*arg);

    uint32_t index;
    if (!pb_decode_varint32(stream, &index))
        return false;
    arr->push_back(index);

    return true;
}

struct SegmentCallbackData
{
    HSV *ledsArr;
    SegmentsStore::SegmentsMap &segmentsMapToFill;
};

/*
Decode a single proto segment

arg - hold the 'this' pointer to the SegmentStore object
*/
bool DecodeSegment_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    if(stream == NULL || field->tag != ControllerObjectsConfig_segments_tag)
        return false;

    struct SegmentCallbackData *cbData = (struct SegmentCallbackData *)(*arg);

    Segment segment = {};

    std::vector<uint32_t> indicesArr;
    indicesArr.reserve(NUM_LEDS);
    segment.indices.funcs.decode = &DecodeSegmentIndex_callback;
    segment.indices.arg = &indicesArr;

    if (!pb_decode(stream, Segment_fields, &segment))
        return false;

    // add this segment to the map
    std::string segmentName(segment.name);
    std::vector<HSV *> &pixelsVec = cbData->segmentsMapToFill[segmentName];

    // fill the pixels in the map, from the indices and the pixels array
    size_t numberOfPixelsInSegment = indicesArr.size();
    pixelsVec.reserve(numberOfPixelsInSegment);
    for(int i=0; i<numberOfPixelsInSegment; i++)
    {
        uint32_t index = indicesArr[i];
        HSV *pixelPtr = &(cbData->ledsArr[index]);
        pixelsVec.push_back(pixelPtr);
    }

    return true;    
}

int SegmentsStore::InitFromFile(HSV ledsArr[], File &f)
{
    uint8_t buffer[4096];
    int msgSize = f.read(buffer, 4096);
    pb_istream_t stream = pb_istream_from_buffer(buffer, msgSize);

    ControllerObjectsConfig message = ControllerObjectsConfig_init_zero;

    struct SegmentCallbackData segmentCallbackData = {.ledsArr = ledsArr, .segmentsMapToFill = m_segmentsMap};
    message.segments.funcs.decode = &DecodeSegment_callback;
    message.segments.arg = &segmentCallbackData;

    if(!pb_decode(&stream, ControllerObjectsConfig_fields, &message))
    {
        return 0;
    }

    uint32_t totalPixels = message.number_of_pixels;
    if(totalPixels > MAX_SUPPORTED_PIXELS) {
        return 0;
    }
    if(totalPixels == 0) {
        return 0;
    }

    return m_segmentsMap.size();
}