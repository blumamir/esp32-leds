/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.4 at Fri Dec 13 12:06:04 2019. */

#include "object_config.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t Segment_fields[3] = {
    PB_FIELD(  1, STRING  , REQUIRED, STATIC  , FIRST, Segment, name, name, 0),
    PB_FIELD(  2, UINT32  , REPEATED, CALLBACK, OTHER, Segment, indices, name, 0),
    PB_LAST_FIELD
};

const pb_field_t ControllerObjectsConfig_fields[3] = {
    PB_FIELD(  1, UINT32  , REQUIRED, STATIC  , FIRST, ControllerObjectsConfig, number_of_pixels, number_of_pixels, 0),
    PB_FIELD(  2, MESSAGE , REPEATED, CALLBACK, OTHER, ControllerObjectsConfig, segments, number_of_pixels, &Segment_fields),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */
