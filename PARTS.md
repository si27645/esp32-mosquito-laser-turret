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

## Local pickup — Mauser.pt (Portugal, exact products)

**Note on naming:** the domain `mouser.pt` (no 'a') actually 301-redirects
to `mauser.pt` — they are not the same company as Mouser Electronics
(the global distributor at pt.mouser.com). The table below is Mauser,
the Portuguese electronics chain with 7 physical stores (Lisboa,
Portela, Corroios, Porto, Leiria, Funchal — full list/hours at
[mauser.pt/lojas](https://mauser.pt/lojas)). Everything below is a real,
in-stock product page on their site, buyable online or picked up
in-store.

| Part | Mauser.pt product | Note |
|---|---|---|
| AI-Thinker ESP32-CAM (OV2640) | [Módulo ESP32 com câmara OV2640](https://mauser.pt/096-7762/modulo-esp32-com-camara-ov2640-esp32-cam) | |
| MG90S servo (×2) | [Motor micro servo MG90s 2.8Kg, Tower Pro](https://mauser.pt/096-7784/motor-micro-servo-4-8v-6v-dc-mg90s-2-8kg-tower-pro-360) | 360° variant; a 180° version also exists in-catalog if you prefer |
| IRLZ44N MOSFET | [Transistor IRLZ44N](https://mauser.pt/002-1190/transistor-irlz44n) | TO-220, logic-level, easy to hand-solder/breadboard |
| Gate resistor (220Ω) | [Resistência de filme metálico 220R 0.6W ±1%](https://mauser.pt/104-7454/resistencia-de-filme-metalico-220r-0-6w-1-2-5x6-5mm) | 0.6W rating is fine for a gate resistor (overkill vs 1/4W, not a problem) |
| Pull-down resistor (10kΩ) | [Resistência de filme metálico 10kR 0.6W ±1%](https://mauser.pt/104-7048/resistencia-de-filme-metalico-10kr-0-6w-1-2-5x6-8mm) | |
| Flyback/snubber diode | [Diodo Comutação 100V 200mA - 1N4148 (DO-35)](https://mauser.pt/catalog/product_info.php?products_id=007-0205) | |
| Electrolytic capacitor (1000µF 16V, power smoothing) | [Condensador Electrolítico 1000uF 16V 105º Ø10x16mm](https://mauser.pt/004-0172/condensador-electrolitico-1000uf-16v-105-10x16mm) | A low-impedance Panasonic version also exists in-catalog if you want tighter specs |
| Jumper wires (M-M) | [Conjunto de 40 cabos Jumper Dupont macho-macho, 150mm](https://mauser.pt/096-7940/conjunto-de-40-cabos-de-ligacao-jumper-dupont-macho-macho-150mm) | |
| 2-key switch (ARM interlock) | [Interruptor de chave SPDT ON-ON Ø19mm](https://mauser.pt/010-0377/interruptor-de-chave-spdt-on-on-24vdc-1a-19mm) | 2 stable positions, 2 keys, panel-mount |

## Optional — upgrade path

| Part | Search term | Why |
|---|---|---|
| ESP32-S3-CAM | "ESP32-S3 CAM board" | More RAM/CPU + free GPIOs, room for a TFLite Micro insect classifier later |
| PIR motion sensor | "HC-SR501 PIR sensor" | Cheap pre-filter to wake the camera pipeline only when something's moving nearby |

See [README.md](README.md) for the full safety write-up, wiring table, and
calibration steps — read that before wiring anything in Stage 2.
