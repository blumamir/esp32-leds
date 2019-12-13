#include <protobuf_infra.h>

bool FileRead_callback(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    File *file = (File*)stream->state;
    bool status;

    if (buf == nullptr) // TODO: undertans when this might happen
    {
        // Notice - orig code:
        // while (count-- && fgetc(file) != EOF);
        // return count == 0; 

        // modified code:
        bool available = file->available();
        file->seek(count, SeekCur);
        return (available == count);
    }

    size_t bytesRead = file->read(buf, count);
    status = (bytesRead == count);

    if (file->available() == 0)
    {
        stream->bytes_left = 0;
    }

   return status;
}

pb_istream_t FileToPbStream(File &f)
{
    pb_istream_t stream = {&FileRead_callback, &f, SIZE_MAX};
    return stream;
}