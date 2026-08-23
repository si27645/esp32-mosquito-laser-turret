# Parts list (Amazon.es)

Companion to the BOM in [README.md](README.md), matched to real Amazon.es
listings. Prices and stock fluctuate and several of these are generic
dropship listings with multiple sellers behind the same ASIN (quality
varies) — treat links as a starting point, not a guarantee, and check
current price/reviews on the page before buying.

## Stage 1 — Tracker (camera + gimbal, no laser)

| Part | Amazon.es listing | Notes |
|---|---|---|
| AI-Thinker ESP32-CAM w/ OV2640 | [HiLetgo 2-pack](https://www.amazon.es/HiLetgo-ESP32-CAM-Development-Bluetooth-Raspberry/dp/B07RXPHYNM) · [KeeYees w/ antenna](https://www.amazon.es/KeeYees-ESP32-CAM-Bluetooth-Desarrollo-inal%C3%A1mbrica/dp/B07QS7VFMJ) | HiLetgo/KeeYees are reliable brands for this board; 2-pack is worth it for spares |
| FTDI USB-TTL programmer (3.3V/5V) | [DSD Tech FT232RL](https://www.amazon.es/DSD-TECH-Adaptador-FT232RL-Compatible/dp/B07BBPX8B8) · [AZDelivery FT232RL](https://www.amazon.es/AZDelivery-FTDI-Adapter-USB-Parent/dp/B082FMKG5C) | Needs a jumper/switch to ground GPIO0 during flashing |
| MG90S metal-gear servos | [AZDelivery 5-pack](https://www.amazon.es/AZDelivery-Servo-MG90S-Micro-engranajes/dp/B086V8RKZR) · [DiGiYes 2-pack](https://www.amazon.es/DiGiYes-unidades-servomotor-helic%C3%B3ptero-Controls/dp/B0BM4NXGXD) | You only need 2, but a 5-pack is often cheaper per-unit and gives spares |
| Pan-tilt bracket kit | [Pan/Tilt Kit Frame + 2×SG90](https://www.amazon.es/Kit-Tilt-Estructura-Servos-SG90/dp/B01LWKKLMO) | Comes bundled with SG90s — swap in the MG90S above if you buy this |
| Jumper wires / Dupont kit | [ELEGOO 120pc M-F/M-M/F-F](https://www.amazon.es/Macho-Hembra-Macho-Macho-Hembra-Hembra-Prototipo-Protoboard/dp/B01NGTXASZ) | |
| 5V power supply for servos | [BeMatik 220VAC→5V/2A USB](https://www.amazon.es/Cablematic-Fuente-alimentaci%C3%B3n-220VAC-hembra/dp/B00C26UP50) | Keep this **separate** from the ESP32-CAM's own supply, share ground only |
| Electrolytic capacitor (power smoothing) | [400pc electrolytic assortment](https://www.amazon.es/condensadores-electrol%C3%ADticos-TOPINCN-valores-surtido/dp/B07GGYPKYD) | Grab a 1000µF+ one from the assortment for the camera's 5V rail — fixes most brownout-on-boot issues |

## Stage 2 — Laser firing stage (only after tracking works, enclosure ready)

| Part | Amazon.es listing | Notes |
|---|---|---|
| Laser diode module | [650nm 200mW w/ TTL control](https://www.amazon.es/Industrial-Diodo-L%C3%A1ser-M%C3%B3dulo-circuito/dp/B07WL7C4FP) | TTL input means you may be able to drive it straight from a GPIO (via the MOSFET below for current headroom) — Class 3B, treat with full precautions |
| Logic-level MOSFET | [AO3400 40-pc SMD kit](https://www.amazon.es/MOSFET-N-Channel-P-Channel-AO3400-AO3401/dp/B0C3S4R3HH) | **SMD/SOT-23 — hand-soldering is fiddly.** If you're not comfortable with SMD, search "IRLZ44N TO-220" instead: same logic-level behavior, but through-hole legs that plug straight into a breadboard |
| Resistor assortment (gate resistor + pulldown) | [2600pc resistor kit](https://www.amazon.es/condensadores-resistencias-pel%C3%ADcula-met%C3%A1lica-Interruptor/dp/B0CZNXCXQN) | Need ~100–220Ω (gate) and ~10kΩ (pulldown), both included in this range of assortments |
| 2-pole key switch (hardware ARM interlock) | [22mm 2-position keyed switch, 1NO+1NC](https://www.amazon.es/Interruptor-Aislamiento-Confiabilidad-Giratorio-Posiciones/dp/B0DFQ8SX49) | The 1NO+1NC contact set is exactly what you need: one contact → GPIO2, the other in series with the laser driver's power |
| Laser safety glasses | [TWINSWOLF OD4+ 190-450nm & 800-1100nm](https://www.amazon.es/TWINSWOLF-seguridad-protecci%C3%B3n-completa-infrarrojo/dp/B07LH3Y3LT) | **Match the wavelength band to your actual diode (650nm here) — verify the listing explicitly covers 650nm before buying**, don't assume from the title alone |
| Enclosure w/ viewing window | [JinZeYou IP65 box, clear lid](https://www.amazon.es/Derivaci%C3%B3n-Conexiones-Electr%C3%B3nica-Impermeable-Instrumentos/dp/B0C1BXTCLM) | Clear polycarbonate lid works as a basic beam-containment window for a low-power test rig |

## Optional — upgrade path

| Part | Search term | Why |
|---|---|---|
| ESP32-S3-CAM | "ESP32-S3 CAM board" | More RAM/CPU + free GPIOs, room for a TFLite Micro insect classifier later |
| PIR motion sensor | "HC-SR501 PIR sensor" | Cheap pre-filter to wake the camera pipeline only when something's moving nearby |

See [README.md](README.md) for the full safety write-up, wiring table, and
calibration steps — read that before wiring anything in Stage 2.
