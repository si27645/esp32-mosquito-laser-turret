#include "gimbal.h"
#include "config.h"
#include <algorithm>

void Gimbal::begin(int panPin, int tiltPin) {
    // ESP32Servo needs the LEDC timer allocated; do it once here.
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);

    _panServo.setPeriodHertz(50);
    _tiltServo.setPeriodHertz(50);
    _panServo.attach(panPin, 500, 2400);
    _tiltServo.attach(tiltPin, 500, 2400);

    center();
}

void Gimbal::setAngles(float panDeg, float tiltDeg) {
    _pan  = std::min((float)SERVO_PAN_MAX_DEG,  std::max((float)SERVO_PAN_MIN_DEG,  panDeg));
    _tilt = std::min((float)SERVO_TILT_MAX_DEG, std::max((float)SERVO_TILT_MIN_DEG, tiltDeg));
    _panServo.write(_pan);
    _tiltServo.write(_tilt);
}

void Gimbal::nudge(float panDeltaDeg, float tiltDeltaDeg) {
    setAngles(_pan + panDeltaDeg, _tilt + tiltDeltaDeg);
}

void Gimbal::center() {
    setAngles(SERVO_PAN_CENTER_DEG, SERVO_TILT_CENTER_DEG);
}
