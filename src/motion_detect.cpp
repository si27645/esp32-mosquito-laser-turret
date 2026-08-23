#include "motion_detect.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>

// Simple, embedded-friendly moving-blob finder:
//   1. Frame differencing against the previous frame (per-pixel luma delta).
//   2. Connected-component flood fill (iterative, explicit stack — no
//      recursion) over the diff mask to find the largest contiguous
//      blob of "changed" pixels.
//   3. Report its centroid and pixel count.
//
// This deliberately does NOT try to classify "is this actually an
// insect" — it just finds the biggest thing that moved. Size-based
// gating (reject too-small/too-large blobs, abort on sudden size
// jumps) is done by the caller, since those are safety-relevant
// decisions that belong in one visible place (main.cpp).

bool MotionDetector::begin(int width, int height) {
    if (_prevFrame) free(_prevFrame);
    _w = width;
    _h = height;
    _prevFrame = (uint8_t *)malloc((size_t)width * height);
    if (!_prevFrame) return false;
    memset(_prevFrame, 0, (size_t)width * height);
    return true;
}

MotionTarget MotionDetector::process(const uint8_t *gray, int width, int height) {
    MotionTarget result = {false, 0, 0, 0};
    if (!_prevFrame || width != _w || height != _h) {
        return result;
    }

    const size_t n = (size_t)width * height;

    // Diff mask: 1 = pixel changed enough to count as motion.
    // Reuse a static scratch buffer sized to the frame to avoid
    // malloc/free churn every frame.
    static uint8_t *mask = nullptr;
    static size_t maskCap = 0;
    if (maskCap < n) {
        free(mask);
        mask = (uint8_t *)malloc(n);
        maskCap = n;
    }
    if (!mask) return result;

    for (size_t i = 0; i < n; i++) {
        int d = (int)gray[i] - (int)_prevFrame[i];
        if (d < 0) d = -d;
        mask[i] = (d > MOTION_DIFF_THRESHOLD) ? 1 : 0;
    }

    // Flood fill to find the largest connected blob (4-connectivity).
    static int32_t *stack = nullptr;
    static size_t stackCap = 0;
    if (stackCap < n) {
        free(stack);
        stack = (int32_t *)malloc(n * sizeof(int32_t));
        stackCap = n;
    }

    long bestCount = 0;
    long bestSumX = 0, bestSumY = 0;

    for (size_t start = 0; start < n; start++) {
        if (!mask[start]) continue;

        // BFS/DFS this component, clearing mask as we go (visited marker).
        size_t sp = 0;
        stack[sp++] = (int32_t)start;
        mask[start] = 0;

        long count = 0, sumX = 0, sumY = 0;
        while (sp > 0) {
            int32_t idx = stack[--sp];
            int x = idx % width;
            int y = idx / width;
            count++;
            sumX += x;
            sumY += y;

            // 4-neighbors
            if (x > 0 && mask[idx - 1])            { mask[idx - 1] = 0; stack[sp++] = idx - 1; }
            if (x < width - 1 && mask[idx + 1])     { mask[idx + 1] = 0; stack[sp++] = idx + 1; }
            if (y > 0 && mask[idx - width])         { mask[idx - width] = 0; stack[sp++] = idx - width; }
            if (y < height - 1 && mask[idx + width]){ mask[idx + width] = 0; stack[sp++] = idx + width; }
        }

        if (count > bestCount) {
            bestCount = count;
            bestSumX = sumX;
            bestSumY = sumY;
        }
    }

    memcpy(_prevFrame, gray, n);

    if (bestCount >= MOTION_MIN_BLOB_PX) {
        result.found = true;
        result.cx = (int)(bestSumX / bestCount);
        result.cy = (int)(bestSumY / bestCount);
        result.pixelCount = (int)bestCount;
    }
    return result;
}
