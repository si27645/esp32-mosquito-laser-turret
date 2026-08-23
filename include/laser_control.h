#pragma once
#include <stdint.h>

// ---------------------------------------------------------------------
// SAFETY-CRITICAL MODULE. Read README.md "Safety" section fully before
// changing any behavior here or bypassing any of these checks.
//
// This class is a *software* interlock layer. It is NOT a substitute
// for the hardware interlock: the physical ARM key switch must be
// wired in series with the laser driver's power/enable line so that,
// even if the firmware has a bug, the laser physically cannot receive
// power unless a human has turned the key. This class additionally
// reads that same switch as a GPIO input so firmware can make
// informed decisions and show status — it must never be the ONLY
// thing standing between "software says fire" and the laser
// receiving power.
// ---------------------------------------------------------------------
class LaserControl {
public:
    void begin(int gatePin, int armSwitchPin, int statusLedPin);

    // Call every control-loop iteration. Reads the arm switch, turns
    // off an expired pulse, and resets the per-minute pulse budget.
    void update();

    // Attempt to fire one pulse. Returns true if it actually fired.
    // Will refuse (return false) if disarmed, still cooling down,
    // already firing, or the per-minute pulse budget is exhausted.
    bool requestFire();

    // Immediately cut laser power, no matter what state we're in.
    // Call this the instant any safety condition is violated
    // (e.g. blob size jumped — something big entered the frame).
    void abort();

    bool isArmed() const  { return _hwArmed; }
    bool isFiring() const { return _firing; }

private:
    int _gatePin = -1, _armPin = -1, _ledPin = -1;
    bool _hwArmed = false;
    bool _firing = false;
    unsigned long _fireStartMs = 0;
    unsigned long _lastFireEndMs = 0;
    int _pulsesThisMinute = 0;
    unsigned long _minuteWindowStartMs = 0;
};
