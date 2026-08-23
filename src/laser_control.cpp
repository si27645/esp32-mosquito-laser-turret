#include "laser_control.h"
#include "config.h"
#include <Arduino.h>

void LaserControl::begin(int gatePin, int armSwitchPin, int statusLedPin) {
    _gatePin = gatePin;
    _armPin = armSwitchPin;
    _ledPin = statusLedPin;

    pinMode(_gatePin, OUTPUT);
    digitalWrite(_gatePin, LOW);          // laser OFF at boot, always
    pinMode(_armPin, INPUT_PULLUP);       // switch pulls LOW when armed
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);

    _minuteWindowStartMs = millis();
}

void LaserControl::update() {
    // Arm switch is active-LOW (pressed/closed = pulled to GND = armed).
    _hwArmed = (digitalRead(_armPin) == LOW);

    unsigned long now = millis();

    // Auto-off: never let a pulse exceed LASER_MAX_ON_MS, even if
    // something upstream forgot to call abort().
    if (_firing && (now - _fireStartMs >= LASER_MAX_ON_MS)) {
        digitalWrite(_gatePin, LOW);
        _firing = false;
        _lastFireEndMs = now;
    }

    // Roll the per-minute duty-cycle budget.
    if (now - _minuteWindowStartMs >= 60000UL) {
        _minuteWindowStartMs = now;
        _pulsesThisMinute = 0;
    }

    // Status LED: solid = armed & idle, and it's already visibly ON
    // during a firing pulse via the same physical LED wiring most
    // AI-Thinker boards use — here we just mirror armed state when
    // not firing (firing itself is driven by requestFire()).
    if (!_firing) {
        digitalWrite(_ledPin, _hwArmed ? HIGH : LOW);
    }

    // Hardware disarmed mid-pulse (key turned off while firing) -> cut now.
    if (!_hwArmed && _firing) {
        abort();
    }
}

bool LaserControl::requestFire() {
    if (!_hwArmed) return false;
    if (_firing) return false;

    unsigned long now = millis();
    if (now - _lastFireEndMs < LASER_COOLDOWN_MS) return false;
    if (_pulsesThisMinute >= LASER_MAX_PULSES_PER_MIN) return false;

    _firing = true;
    _fireStartMs = now;
    _pulsesThisMinute++;
    digitalWrite(_gatePin, HIGH);
    digitalWrite(_ledPin, HIGH);
    return true;
}

void LaserControl::abort() {
    digitalWrite(_gatePin, LOW);
    _firing = false;
    _lastFireEndMs = millis();
}
