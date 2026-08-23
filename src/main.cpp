// ESP32 Mosquito/Fly Tracker + Laser Turret
// AI-Thinker ESP32-CAM, fully on-device (no PC/server needed).
//
// Pipeline each control-loop tick:
//   1. Grab a small grayscale frame from the camera.
//   2. Frame-diff against the previous frame to find the largest
//      moving blob (MotionDetector).
//   3. Reject blobs outside the expected insect pixel-size range, or
//      that jumped in size too fast (likely a hand/face/large object
//      entering frame) -> disarm-for-this-target as a safety measure.
//   4. PID-steer the pan/tilt gimbal to center the blob in frame.
//   5. Once centered & stable for MOTION_LOCK_FRAMES frames, and the
//      hardware ARM key is on, fire one short laser pulse.
//
// See README.md before powering the laser driver for the first time.

#include <Arduino.h>
#include "esp_camera.h"
#include "camera_pins.h"
#include "config.h"
#include "motion_detect.h"
#include "pid_controller.h"
#include "gimbal.h"
#include "laser_control.h"

static MotionDetector detector;
static Gimbal gimbal;
static LaserControl laser;
static PID pidPan(PID_KP, PID_KI, PID_KD, PID_OUTPUT_LIMIT_DEG);
static PID pidTilt(PID_KP, PID_KI, PID_KD, PID_OUTPUT_LIMIT_DEG);

static int frameW = 0, frameH = 0;
static int lockFrames = 0;
static int lastCx = -1, lastCy = -1;
static int lastPixelCount = 0;
static unsigned long lastLoopMs = 0;

static bool initCamera() {
    camera_config_t c = {};
    c.ledc_channel = LEDC_CHANNEL_0;
    c.ledc_timer   = LEDC_TIMER_0;
    c.pin_d0 = Y2_GPIO_NUM;  c.pin_d1 = Y3_GPIO_NUM;
    c.pin_d2 = Y4_GPIO_NUM;  c.pin_d3 = Y5_GPIO_NUM;
    c.pin_d4 = Y6_GPIO_NUM;  c.pin_d5 = Y7_GPIO_NUM;
    c.pin_d6 = Y8_GPIO_NUM;  c.pin_d7 = Y9_GPIO_NUM;
    c.pin_xclk = XCLK_GPIO_NUM;
    c.pin_pclk = PCLK_GPIO_NUM;
    c.pin_vsync = VSYNC_GPIO_NUM;
    c.pin_href = HREF_GPIO_NUM;
    c.pin_sccb_sda = SIOD_GPIO_NUM;
    c.pin_sccb_scl = SIOC_GPIO_NUM;
    c.pin_pwdn = PWDN_GPIO_NUM;
    c.pin_reset = RESET_GPIO_NUM;
    c.xclk_freq_hz = 20000000;
    c.pixel_format = PIXFORMAT_GRAYSCALE;   // we only need luma for motion diff
    c.frame_size = CAM_FRAME_SIZE;
    c.jpeg_quality = CAM_JPEG_QUALITY;
    c.fb_count = 2;
    c.fb_location = CAMERA_FB_IN_PSRAM;
    c.grab_mode = CAMERA_GRAB_LATEST;

    esp_err_t err = esp_camera_init(&c);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed: 0x%x\n", err);
        return false;
    }
    return true;
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("\n=== ESP32 Mosquito Tracker/Turret booting ===");

    if (!initCamera()) {
        Serial.println("FATAL: camera init failed, halting.");
        while (true) delay(1000);
    }

    // Read one frame just to learn the actual buffer dimensions.
    camera_fb_t *fb = esp_camera_fb_get();
    if (fb) {
        frameW = fb->width;
        frameH = fb->height;
        esp_camera_fb_return(fb);
    } else {
        frameW = 160; frameH = 120; // QQVGA fallback
    }
    Serial.printf("Camera frame: %dx%d\n", frameW, frameH);

    if (!detector.begin(frameW, frameH)) {
        Serial.println("FATAL: motion detector alloc failed, halting.");
        while (true) delay(1000);
    }

    gimbal.begin(PIN_SERVO_PAN, PIN_SERVO_TILT);
    laser.begin(PIN_LASER_GATE, PIN_ARM_SWITCH, PIN_STATUS_LED);

    Serial.println("Ready. Laser will only fire when the hardware ARM key is on.");
    lastLoopMs = millis();
}

void loop() {
    const unsigned long periodMs = 1000UL / CONTROL_LOOP_HZ;
    unsigned long now = millis();
    if (now - lastLoopMs < periodMs) return;
    float dt = (now - lastLoopMs) / 1000.0f;
    lastLoopMs = now;

    laser.update(); // services auto-off, arm-switch read, duty cycle window

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) return;

    MotionTarget target = detector.process(fb->buf, fb->width, fb->height);
    esp_camera_fb_return(fb);

    if (!target.found) {
        lockFrames = 0;
        lastCx = lastCy = -1;
        return;
    }

    // --- Safety gate #1: reject implausible insect sizes -------------
    if (target.pixelCount > MOTION_MAX_BLOB_PX) {
        Serial.printf("Blob too large (%d px) - ignoring, not tracking.\n", target.pixelCount);
        lockFrames = 0;
        lastCx = lastCy = -1;
        lastPixelCount = 0;
        return;
    }

    // --- Safety gate #2: reject sudden size jumps ---------------------
    // (something big just moved into frame, e.g. a hand near the muzzle)
    if (lastPixelCount > 0 &&
        (target.pixelCount - lastPixelCount) > LASER_ABORT_ON_SIZE_JUMP_PX) {
        Serial.println("SAFETY: blob size jumped sharply - aborting any fire, resetting lock.");
        laser.abort();
        lockFrames = 0;
        lastPixelCount = target.pixelCount;
        return;
    }
    lastPixelCount = target.pixelCount;

    // --- Steer gimbal toward target ------------------------------------
    float errX = (float)target.cx - (frameW / 2.0f);
    float errY = (float)target.cy - (frameH / 2.0f);
    float panCorrection  = pidPan.update(errX, dt);
    float tiltCorrection = pidTilt.update(errY, dt);
    // image x grows right -> pan servo direction depends on your mount;
    // flip signs here if the turret drives away from the target.
    gimbal.nudge(-panCorrection, tiltCorrection);

    // --- Lock-on: has the blob been ~stationary near center? ----------
    bool nearCenter = (fabsf(errX) < (frameW * 0.08f)) && (fabsf(errY) < (frameH * 0.08f));
    bool stationary = (lastCx >= 0) &&
        (abs(target.cx - lastCx) < MOTION_LOCK_TOLERANCE_PX) &&
        (abs(target.cy - lastCy) < MOTION_LOCK_TOLERANCE_PX);
    lastCx = target.cx;
    lastCy = target.cy;

    if (nearCenter && stationary) {
        lockFrames++;
    } else {
        lockFrames = 0;
    }

    if (lockFrames >= MOTION_LOCK_FRAMES) {
        if (laser.requestFire()) {
            Serial.printf("FIRE. blob=%dpx pos=(%d,%d)\n", target.pixelCount, target.cx, target.cy);
        }
        lockFrames = 0; // require a fresh lock before firing again
    }
}
