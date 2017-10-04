#ifndef TRACKINGDATA_H
#define TRACKINGDATA_H

#include <ProcessorHeaders.h>

struct TrackingData {
    int64 timestamp;
    float x;
    float y;
    float width;
    float height;
};

#endif // TRACKINGDATA_H
