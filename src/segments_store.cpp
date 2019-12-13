#include <segments_store.h>

#include <pb_common.h>
#include <pb_decode.h>
#include <object_config.pb.h>

typedef std::vector<uint32_t> IndicesVec;

/*
This callback is called once per index.
It stores the index at the end of the IndicesVec vector, givin as *arg.
When the iteration is complete - the vector holds the list of indices for the segment
*/
bool DecodeSegmentIndex_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    if(stream == NULL || field->tag != Segment_indices_tag)
        return false;

    IndicesVec *arr = (IndicesVec *)(*arg);

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
    IndicesVec &indicesVecStorage;
};

/*
Decode a single proto segment

arg - holds 'struct SegmentCallbackData', with data needed to add this segment to the mapping
*/
bool DecodeSegment_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    if(stream == NULL || field->tag != ControllerObjectsConfig_segments_tag)
        return false;

    struct SegmentCallbackData *cbData = (struct SegmentCallbackData *)(*arg);
    IndicesVec &indicesVecStorage = cbData->indicesVecStorage;

    Segment segment = {};

    indicesVecStorage.resize(0);
    segment.indices.funcs.decode = &DecodeSegmentIndex_callback;
    segment.indices.arg = &indicesVecStorage;

    if (!pb_decode(stream, Segment_fields, &segment))
        return false;

    size_t numberOfPixelsInSegment = indicesVecStorage.size();

    // convert the indices vector, to pixels ptr vector
    std::vector<HSV *> *pixelsVecPtr = new std::vector<HSV *>(numberOfPixelsInSegment);
    for(int i=0; i<numberOfPixelsInSegment; i++)
    {
        uint32_t index = indicesVecStorage[i];
        HSV *pixelPtr = &(cbData->ledsArr[index]);
        pixelsVecPtr->at(i) = pixelPtr;
    }

    // add this segment to the map
    std::string segmentName(segment.name);
    cbData->segmentsMapToFill[segmentName] = pixelsVecPtr;

    return true;    
}

int SegmentsStore::InitFromFile(HSV ledsArr[], File &f)
{
    uint8_t buffer[4096];
    int msgSize = f.read(buffer, 4096);
    pb_istream_t stream = pb_istream_from_buffer(buffer, msgSize);

    ControllerObjectsConfig message = ControllerObjectsConfig_init_zero;

    // the vector is a temporary storage for the indices while parsed from pb.
    // used so we do not need to reallocate the vector everytime
    std::vector<uint32_t> indicesVecStorage;
    indicesVecStorage.reserve(NUM_LEDS);

    // describe how to parse the `repeated Segments` field,
    // with callback to the function that does the job, and data it needs
    struct SegmentCallbackData segmentCallbackData = {
        .ledsArr = ledsArr, 
        .segmentsMapToFill = m_segmentsMap,
        .indicesVecStorage = indicesVecStorage
    };
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