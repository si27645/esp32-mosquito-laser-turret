#pragma once
#include <ESP32Servo.h>

// Thin wrapper around two hobby servos (pan + tilt) with software
// angle clamping so the control loop can never command the gimbal
// past the mechanical limits set in config.h, and a helper for
// applying a relative correction (as produced by the PID loop).
class Gimbal {
public:
    void begin(int panPin, int tiltPin);
    void setAngles(float panDeg, float tiltDeg);
    void nudge(float panDeltaDeg, float tiltDeltaDeg);
    void center();

    float pan()  const { return _pan; }
    float tilt() const { return _tilt; }

private:
    Servo _panServo;
    Servo _tiltServo;
    float _pan = 90, _tilt = 90;
};
