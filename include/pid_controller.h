#pragma once

class PID {
public:
    PID(float kp, float ki, float kd, float outLimit)
        : _kp(kp), _ki(ki), _kd(kd), _outLimit(outLimit) {}

    void reset() {
        _integral = 0;
        _lastError = 0;
        _haveLast = false;
    }

    // error: current tracking error (e.g. pixels off-center)
    // dt: seconds since last call
    // Returns a bounded correction output.
    float update(float error, float dt) {
        if (dt <= 0) dt = 1e-3f;
        _integral += error * dt;
        // basic anti-windup: clamp integral contribution
        float iTerm = _ki * _integral;
        if (iTerm > _outLimit) { iTerm = _outLimit; _integral = iTerm / _ki; }
        if (iTerm < -_outLimit) { iTerm = -_outLimit; _integral = iTerm / _ki; }

        float derivative = _haveLast ? (error - _lastError) / dt : 0.0f;
        _lastError = error;
        _haveLast = true;

        float out = _kp * error + iTerm + _kd * derivative;
        if (out > _outLimit) out = _outLimit;
        if (out < -_outLimit) out = -_outLimit;
        return out;
    }

private:
    float _kp, _ki, _kd, _outLimit;
    float _integral = 0;
    float _lastError = 0;
    bool _haveLast = false;
};
