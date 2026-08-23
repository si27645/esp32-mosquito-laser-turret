#pragma once
#include <stdint.h>

// Result of scanning one frame for a moving blob.
struct MotionTarget {
    bool     found;
    int      cx, cy;      // centroid, in frame pixel coordinates
    int      pixelCount;  // blob size (proxy for insect size/distance)
};

class MotionDetector {
public:
    // width/height must match the camera's grayscale frame buffer size.
    bool begin(int width, int height);

    // Feed one grayscale frame (1 byte/pixel). Returns the largest
    // qualifying moving blob, if any, using simple frame differencing
    // + connected-component-ish flood counting on a coarse grid.
    MotionTarget process(const uint8_t *gray, int width, int height);

private:
    uint8_t *_prevFrame = nullptr;
    int _w = 0, _h = 0;
};
