# Simon_pocket – A pocket version of the classic Simon says game

![Finished PCB Photo](docs/images/hero-photo.png)

Simon_pocket is a pocked sized memory training game. It works by having players repeat button sequences that get longer each round.

---

## Gameplay

![Block Diagram](Media/Simon_pocket_gameplay_diagram.drawio.svg)

---

## Hardware Architecture

![Block Diagram](Media/Simon_pocket_hardware_block_diagram.svg)

---

## Conponent Selection and Design Considerations

- **Microcontroller:** The STM32F030C6T6 was chosen for being inexpensive (0.5€ @ 30+ Qty) and having good documentation and support.
- **Battery:** The CR2032 was selected because of a compatable nominal voltage (3V), compact size and availability.
- **Sound system:** Buzzers/speakers had to be omitted due to the high internal resistance of the battery.
- **Power system:** Due to space constraints, a dedicated power button was not possible. Instead, every button is configured to wake up the MCU from STOP mode.
- **Size:** The PCB diameter of 27mm was selected for being the smallest diameter that could fit the battery holder.
- **Standby current:** To minimize standby current:
  - A low leakage curent PMOS was selected.
  - No LDOs or switching regulators were used.
  - The MCU operates mostly in STOP power saving mode.

---

## PCB Design & Layout Details
<p align="center">
  <img src="Media/Simon_pocket_PCB_top_layer.svg" width="45%" alt="PCB top layer" />
  <img src="Media/Simon_pocket_PCB_bottom_layer.svg" width="45%" alt="PCB bottom layer" />
</p>

### Layer Stackup (2-Layer)
1. **Top (Signal / Power):** MCU, LED and button traces. Power distribution.
2. **Bottom (Ground / Power):** Negative battery contact and ground plane. Power distribution.

### Key Layout Highlights
- **High-Speed USB:** Routed as 90Ω differential pairs with matched lengths (within ±0.1mm tolerance).
- **Analog Isolation:** Dedicated analog ground region connected to digital GND at a single star-ground point near the ADC.
- **Thermal Design:** Thermal vias placed under the main regulator to sink heat into internal copper planes.

---

## Code
The code was written in bare metal CMSIS C using the STM32CubeIDE. To upload the code an ST-LINK V2 has to be connected to the programming contacts on the PCB.

- **Microcontroller:** The STM32F030C6T6 was chosen for being inexpensive (0.5€ @ 30+ Qty) and having good documentation and support.


---

## 🧪 Validation & Test Results

![Oscilloscope Trace](docs/images/scope-capture.png)

- **Power Ripple Test:** 3.3V rail ripple measured under 15mV peak-to-peak at full 1.5A load.
- **Thermal Performance:** Max board temp reached 48°C under full load after 1 hour ambient testing.
- **Signal Integrity:** USB 2.0 full-speed eye diagram verified clean with minimal jitter.

---

## 🛠️ Design Iterations & Lessons Learned

- **Rev A Bug:** Ground plane beneath the RF antenna was not properly cleared, degrading wireless range by ~40%.
- **Rev B Fix:** Added antenna cutout area according to datasheet guidelines, restoring expected range.
- **Trade-off:** Chose QFN over BGA components to allow manual hot-air rework on a bench without X-ray inspection.

---

## 🧰 Tools & Software Used

- **EDA:** KiCad 10.0
- **Simulation:** LTspice (Power supply ripple & transient response)
- **Measurement:** Siglent SDS1104X-E Oscilloscope, Saleae Logic Pro 8
