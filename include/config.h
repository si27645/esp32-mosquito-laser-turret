#pragma once
#include <stdint.h>

// =====================================================================
//  PIN MAP  —  AI-Thinker ESP32-CAM
// =====================================================================
// The AI-Thinker board uses almost every GPIO for the OV2640 camera.
// Only a handful of pins are free once you give up the SD card slot
// (which this project does — SD uses GPIO 2/4/12/13/14/15 in 1-bit
// mode, we reclaim them). GPIO0 is boot-mode select: never wire
// anything to it that could pull it low at power-on other than the
// deliberate flashing jumper.
//
//   GPIO 12  -> Pan servo signal   (must NOT be high at boot; ESP32Servo
//                                   starts pins low by default, so ok,
//                                   but double check with a logic probe
//                                   the first time you wire it up)
//   GPIO 13  -> Tilt servo signal
//   GPIO 14  -> Laser MOSFET gate (through gate resistor, see README)
//   GPIO 2   -> Physical ARM key switch input (active LOW, pull-up)
//   GPIO 4   -> Status LED (this is also the onboard white flash LED —
//                           reused here as the "armed/firing" indicator,
//                           which is convenient: it's bright and visible)
//   GPIO 16  -> free / reserved for future PIR or enclosure interlock
//
// If you move to an ESP32-S3 camera board instead (recommended for a
// v2 with more headroom / more free GPIOs), update these pin numbers
// and re-check your board's specific camera pin map in camera_pins.h.
// =====================================================================

#define PIN_SERVO_PAN     12
#define PIN_SERVO_TILT    13
#define PIN_LASER_GATE    14
#define PIN_ARM_SWITCH     2
#define PIN_STATUS_LED     4

// ---------------------------------------------------------------------
// Servo geometry / limits
// ---------------------------------------------------------------------
#define SERVO_PAN_MIN_DEG    20      // mechanical limits — set these to
#define SERVO_PAN_MAX_DEG   160      // whatever your gimbal can safely
#define SERVO_TILT_MIN_DEG   30      // reach without binding. Keep a
#define SERVO_TILT_MAX_DEG  150      // margin inside the true hard stops.
#define SERVO_PAN_CENTER_DEG  90
#define SERVO_TILT_CENTER_DEG 90

// ---------------------------------------------------------------------
// Camera frame settings — small + grayscale-equivalent for speed.
// FRAMESIZE_QQVGA = 160x120. We only need enough resolution to find a
// moving blob, not to classify it, so keep this small for frame rate.
// ---------------------------------------------------------------------
#define CAM_FRAME_SIZE      FRAMESIZE_QQVGA
#define CAM_JPEG_QUALITY    12

// ---------------------------------------------------------------------
// Motion detection tuning
// ---------------------------------------------------------------------
#define MOTION_DIFF_THRESHOLD     25     // per-pixel luma delta to count as "changed"
#define MOTION_MIN_BLOB_PX        6      // ignore blobs smaller than this (noise)
#define MOTION_MAX_BLOB_PX        400    // ignore blobs bigger than this
                                          // (SAFETY: a hand/face entering frame
                                          // is a huge blob relative to an insect
                                          // at the same distance — reject it)
#define MOTION_LOCK_FRAMES        4      // consecutive frames blob must be
                                          // ~stationary near center before firing
#define MOTION_LOCK_TOLERANCE_PX  6      // "stationary" = centroid moves less
                                          // than this between frames

// ---------------------------------------------------------------------
// PID gains for pan/tilt tracking (error is in pixels, output in degrees/step)
// ---------------------------------------------------------------------
#define PID_KP   0.06f
#define PID_KI   0.0015f
#define PID_KD   0.02f
#define PID_OUTPUT_LIMIT_DEG   4.0f   // max degrees of correction per control step

// ---------------------------------------------------------------------
// Laser firing safety limits — THESE ARE THE MOST IMPORTANT NUMBERS
// IN THIS FILE. Read README.md "Safety" section before changing them.
// ---------------------------------------------------------------------
#define LASER_MAX_ON_MS         400    // hard cap on a single firing pulse
#define LASER_COOLDOWN_MS      1500    // forced gap between pulses
#define LASER_MAX_PULSES_PER_MIN 20    // duty-cycle limiter, resets every 60s
#define LASER_ABORT_ON_SIZE_JUMP_PX 150 // if blob grows by more than this in
                                          // one frame while armed, treat as
                                          // "something big just entered frame"
                                          // and abort/disarm immediately

// Loop timing
#define CONTROL_LOOP_HZ   15
