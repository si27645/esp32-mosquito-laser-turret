# ESP32 Mosquito / Fly Tracker & Laser Turret

An on-device (no PC/server needed) camera-based insect tracker built on
a cheap **AI-Thinker ESP32-CAM** module. It watches for small moving
blobs, steers a two-axis pan/tilt gimbal to keep the target centered,
and — only when a physical key switch is turned on — fires a short
laser pulse once the target has been locked and stationary for a few
frames.

**Firmware builds clean** (`pio run`, verified in this repo).

---

## ⚠️ Safety — read this before you power the laser

A laser strong enough to be lethal to an insect at a few feet is also
strong enough to **permanently damage a human or pet's retina**, and
can ignite flammable materials. This is the single most dangerous part
of the whole project. Do not skip this section.

1. **Two independent interlocks, not one.** The firmware's `LaserControl`
   class is a *software* interlock — it will refuse to fire unless a
   GPIO reads "armed." That is not enough on its own (firmware can crash,
   brown out, or have a bug). Wire the physical **ARM key switch as a
   second pole in series with the laser driver's actual power/enable
   line**, so the laser is physically incapable of receiving power with
   the key off, independent of what the ESP32 is doing. A cheap
   two-pole keyswitch or a DPST toggle gives you both poles.
2. **Enclosure + beam containment.** Run the turret inside a box with a
   viewing window (e.g. IR-passing acrylic, or just an open front only
   used in a controlled test area) so a stray shot can't leave the room.
   Never aim it anywhere near eye level of people/pets, and never test
   it with the enclosure open and the key armed.
3. **Class of laser matters a lot.** Do not reach for a "burns things"
   laser diode (Class 3B/4, typically >5mW, often hundreds of mW–watts)
   without eye protection rated for that specific wavelength, for
   *everyone* who could possibly be in the room, plus warning
   signage. If you're not already comfortable with laser safety
   practice, start with the "detect + track only" build (see below)
   and treat the firing stage as a separate, later, much more careful
   project — this firmware's laser firing code is provided but you do
   not have to populate `PIN_LASER_GATE` at all to get a fully working
   tracker.
4. **Software limits already in `config.h`** (do not weaken these
   without understanding why they're there):
   - `LASER_MAX_ON_MS` — hard cap on a single pulse (400ms default).
   - `LASER_COOLDOWN_MS` — forced gap between pulses.
   - `LASER_MAX_PULSES_PER_MIN` — duty-cycle limiter.
   - `MOTION_MAX_BLOB_PX` — refuses to track/fire on anything bigger
     than a plausible insect-sized blob at the mount's working
     distance (tune this for your setup — see Calibration).
   - `LASER_ABORT_ON_SIZE_JUMP_PX` — if the tracked blob suddenly grows
     (e.g. a hand moves toward the muzzle), firing aborts immediately
     and the lock resets.
5. **Legal/local rules.** Laser device regulations (power limits,
   labeling) vary by country and even by state/province. Check local
   rules before building anything above a low-power (<5mW) laser.
6. Consider a **non-lethal deterrent first**: the exact same
   detect+track pipeline can drive an air-puff valve, a net,
   an ultrasonic emitter, or just a data logger ("how many mosquitoes
   pass through this spot") instead of a laser. Swapping
   `laser_control.cpp` for a different actuator is a small change.

---

## Bill of materials

| Part | Notes | Approx. price |
|---|---|---|
| AI-Thinker ESP32-CAM (OV2640) | Cheapest common ESP32 board with an onboard camera | $6–10 |
| USB-TTL / FTDI adapter (3.3V logic) | Needed to flash — the board has no onboard USB | $3–5 |
| 2x SG90 (or MG90S for more torque) micro servos | Pan + tilt | $4–8 |
| Pan-tilt bracket kit | Any generic 2-axis SG90 bracket | $3–6 |
| 5V 2A+ separate power supply for servos | **Do not power servos from the ESP32's onboard 3.3V regulator** — it will brown out the camera. Share ground with the ESP32. | $5 |
| Laser diode module + MOSFET driver board (only if doing the fire stage) | Logic-level N-MOSFET (e.g. AO3400) or a ready-made driver board, gate resistor ~100–220Ω, flyback/snubber per driver datasheet | varies a lot by power |
| 2-pole keyswitch or DPST toggle | Physical ARM interlock — one pole to GPIO, one pole in series with laser driver power | $3–5 |
| Enclosure with viewing window | Beam containment | varies |
| (optional) IR-cut removed / IR laser + IR-pass filter | Some insect-turret projects use IR instead of visible red so the beam isn't distracting/visible; adds complexity, skip for v1 | — |

---

## Wiring (AI-Thinker ESP32-CAM pin map)

The AI-Thinker board dedicates almost every GPIO to the camera. Free
pins (once you give up the microSD slot, which this project doesn't
use) are limited — see [include/config.h](include/config.h) for the
authoritative pin table:

| Signal | GPIO |
|---|---|
| Pan servo signal | 12 |
| Tilt servo signal | 13 |
| Laser MOSFET gate (through gate resistor) | 14 |
| ARM key switch input (other pole goes to laser power, see Safety) | 2 (active LOW, internal pull-up) |
| Status LED (reuses the onboard flash LED) | 4 |

Power:
- ESP32-CAM: 5V in via the FTDI adapter's 5V pin (or a separate 5V
  supply) — **do not** try to run it off the FTDI adapter's 3.3V rail,
  the camera needs the onboard 5V→3.3V regulator's headroom.
- Servos: separate 5V supply, **grounds tied together** with the
  ESP32-CAM ground.
- Laser driver: per its own datasheet; gate driven by GPIO14 through
  the MOSFET, physically gated by the keyswitch as described above.

Flashing: GPIO0 to GND during flashing only (jumper/button), remove
after flashing and power-cycle to run normally.

---

## Building & flashing

```bash
# from this directory
pio run                 # build
pio run -t upload       # flash (GPIO0 must be grounded first)
pio device monitor -b 115200   # serial log after flashing
```

## Calibration

1. Start with the laser driver **unpowered / disconnected** and just
   get tracking working.
2. Point the camera at a plain, evenly lit background. Release an
   insect (or wave something insect-sized) in frame and watch the
   serial log for blob sizes (`pixelCount`).
3. Set `MOTION_MIN_BLOB_PX` / `MOTION_MAX_BLOB_PX` in
   [config.h](include/config.h) to bracket the pixel counts you
   actually see for real insects at your mount's working distance —
   this is your main safety filter against firing at anything insect-
   sized in the frame that isn't actually near the muzzle's kill zone.
4. Tune `PID_KP/KI/KD` if the gimbal oscillates (reduce `PID_KP`) or
   is too sluggish (increase it a bit, add a little `PID_KD` to damp).
5. Only after tracking looks solid and size-gating is dialed in, wire
   up the laser stage behind its full enclosure + keyswitch interlock.

---

## Project layout

```
include/
  config.h          all tunables + pin map, read this first
  camera_pins.h     AI-Thinker OV2640 pin map (standard, don't change)
  motion_detect.h
  pid_controller.h
  gimbal.h
  laser_control.h
src/
  main.cpp          ties everything together, control loop
  motion_detect.cpp frame-diff + connected-component blob finder
  gimbal.cpp        pan/tilt servo driver with angle clamping
  laser_control.cpp software interlock + duty-cycle limiter
```

## Adapting from a plain Arduino/ESP32 pan-tilt gimbal sketch

If you're coming from a bare servo pan-tilt sketch (joystick- or
serial-driven, no camera), the pieces you're keeping are `gimbal.h/cpp`
— same `Servo`/`ESP32Servo` calls, just wrapped with angle clamping and
a `nudge()` helper. Everything upstream of it (camera capture, motion
detection, PID) is new; everything downstream (`laser_control.cpp`) is
new. Swap in your own gimbal code by matching `Gimbal`'s public
interface (`begin`, `setAngles`, `nudge`, `center`) if your bracket's
axis directions differ.

## Known limitations / next steps

- Frame-differencing has no idea what's actually in the blob — a leaf
  blowing in the wind will look the same as a fly to it. This is why
  size gating and lock-on-stability matter so much; a real deployment
  benefits a lot from a controlled backdrop (indoor use, plain wall)
  vs. outdoor foliage.
- No ML classification on-device yet. An ESP32-S3 (more RAM/CPU, and
  more free GPIOs than the AI-Thinker board) could add a small TFLite
  Micro insect-vs-not classifier on top of the motion detector for far
  fewer false triggers — worth a v2 if false-positives are a problem.
- PID gains and pixel-size thresholds are per-mount — retune whenever
  you change camera distance/angle.
