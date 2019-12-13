#include <segments_store.h>

#include <pb_common.h>
#include <pb_decode.h>
#include <object_config.pb.h>

/*
This callback is called once per index.
It stores the index at the end of the IndicesVec vector, givin as *arg.
When the iteration is complete - the vector holds the list of indices for the segment
*/
bool DecodeSegmentIndex_callback(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
    if(stream == NULL || field->tag != Segment_indices_tag)
        return false;

    SegmentsStore::IndicesVec *arr = (SegmentsStore::IndicesVec *)(*arg);

    uint32_t index;
    if (!pb_decode_varint32(stream, &index))
        return false;
    arr->push_back(index);

    return true;
}

struct SegmentCallbackData
{
    SegmentsStore *segmentsStore;
    HSV *ledsArr;
    SegmentsStore::IndicesVec &indicesVecStorage;
    const uint32_t *totalPixels;
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
    SegmentsStore::IndicesVec &indicesVecStorage = cbData->indicesVecStorage;

    Segment segment = {};

    indicesVecStorage.resize(0);
    segment.indices.funcs.decode = &DecodeSegmentIndex_callback;
    segment.indices.arg = &indicesVecStorage;

    if (!pb_decode(stream, Segment_fields, &segment))
        return false;

    bool addSegmentSuccess = cbData->segmentsStore->AddSegment(std::string(segment.name), cbData->ledsArr, indicesVecStorage, *(cbData->totalPixels));    
    return addSegmentSuccess;
}

bool SegmentsStore::ValidateNumberOfPixels(uint32_t totalPixels)
{
    if(totalPixels > MAX_SUPPORTED_PIXELS) {
        m_errorDesc = "total pixels are larger than supported";
        return false;
    }
    if(totalPixels == 0) {
        m_errorDesc = "total pixels is zero";
        return false;
    }
}

bool SegmentsStore::AddSegment(const std::string &segmentName, HSV ledsArr[], IndicesVec &indicesVecStorage, uint32_t totalPixels)
{
    if(!ValidateNumberOfPixels(totalPixels))
        return false;

    size_t numberOfPixelsInSegment = indicesVecStorage.size();

    // convert the indices vector, to pixels ptr vector
    std::vector<HSV *> *pixelsVecPtr = new std::vector<HSV *>(numberOfPixelsInSegment);
    for(int i=0; i<numberOfPixelsInSegment; i++)
    {
        uint32_t index = indicesVecStorage[i];
        if(index >= totalPixels)
        {
            m_errorDesc = "segment index out of bound ( >= number_of_pixels)";
            return false;
        }
        HSV *pixelPtr = &(ledsArr[index]);
        pixelsVecPtr->at(i) = pixelPtr;
    }

    // add this segment to the map
    std::pair<SegmentsStore::SegmentsMap::iterator ,bool> insertRes = m_segmentsMap.insert({segmentName, pixelsVecPtr});
    if(!insertRes.second)
    {
        m_errorDesc = "found segment with same name more than once";
        return false;
    }

    return true;
}

bool SegmentsStore::InitFromFile(HSV ledsArr[], File &f)
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
        .segmentsStore = this,
        .ledsArr = ledsArr, 
        .indicesVecStorage = indicesVecStorage,
        .totalPixels = &message.number_of_pixels
    };
    message.segments.funcs.decode = &DecodeSegment_callback;
    message.segments.arg = &segmentCallbackData;

    if(!pb_decode(&stream, ControllerObjectsConfig_fields, &message))
    {
        if(m_errorDesc == NULL)
        {
            m_errorDesc = "error in parsing segments configuration message";
        }
        return false;
    }

    m_initialized = true;
    m_errorDesc = NULL;

    return true;
}