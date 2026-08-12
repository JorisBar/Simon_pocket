# Simon_pocket – A pocket version of the classic Simon says game

![Finished PCB Photo](docs/images/hero-photo.png)

This project was created to occupy the minds of people wanting a break from modern day distractions. Price and form factor were prioritized from day one and lead to certain design choises.

---

## 🎯 Constraints and design considerations

- **Battery selection:** the CR2032 was chosen due to compatible nominal voltage (3V), compact size, wide adoption
- **Lack of sound:** buzzers/speakers had to be omitted due to high internal resistance of the battery
- **Power Budget:** the MCU had to operate mostly STOP mode to lower standby current to 6uA ensuring battery life for multiple years
- **Key Specs:**
  - **Main Controller:** STM32G431KB (Arm Cortex-M4 @ 170MHz)
  - **Power Subsystem:** Buck-Boost regulator (3.3V–12V input -> 5V @ 2A output)
  - **PCB Specs:** 4-layer FR4, 1/1/1/1 oz Cu, 1.6mm thickness, controlled 50Ω impedance traces
---

## Conponent selection

- **Battery:** the CR2032 was chosen due to compatible nominal voltage (3V), compact size, wide adoption
- **Sound system:** buzzers/speakers had to be omitted due to high internal resistance of the battery
- **Microcontroller:** the STM32F030c6t6 was chosen due to low price (0.5€ @ 30+ Qty) and good documentation and support
- **Power Budget:** the MCU had to operate mostly STOP mode to lower standby current to 6uA ensuring battery life for multiple years
- **Key Specs:**
  - **Main Controller:** STM32G431KB (Arm Cortex-M4 @ 170MHz)
  - **Power Subsystem:** Buck-Boost regulator (3.3V–12V input -> 5V @ 2A output)
  - **PCB Specs:** 4-layer FR4, 1/1/1/1 oz Cu, 1.6mm thickness, controlled 50Ω impedance traces
---

## 🏗️ System Architecture

![Block Diagram](docs/images/block-diagram.png)

### Core Component Choices
- **MCU (STM32G431):** Selected for integrated high-speed op-amps and hardware CORDIC math accelerator.
- **Power (TPS62840):** Chosen for low quiescent current (60nA) to maximize battery longevity during sleep mode.

---

## 📐 PCB Design & Layout Details

![PCB Routing Screenshot](docs/images/pcb-layout.png)

### Layer Stackup (4-Layer)
1. **Top (Signal / High-Speed):** Critical traces, impedance-matched differential pairs.
2. **Inner 1 (GND):** Continuous ground plane for signal return paths and shielding.
3. **Inner 2 (Power):** Split power planes (3.3V, 5V, VBUS).
4. **Bottom (Signal / Power):** Non-critical routing and thermal copper pours.

### Key Layout Highlights
- **High-Speed USB:** Routed as 90Ω differential pairs with matched lengths (within ±0.1mm tolerance).
- **Analog Isolation:** Dedicated analog ground region connected to digital GND at a single star-ground point near the ADC.
- **Thermal Design:** Thermal vias placed under the main regulator to sink heat into internal copper planes.

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

- **CAD / EDA:** KiCad 8.0
- **Simulation:** LTspice (Power supply ripple & transient response)
- **Measurement:** Siglent SDS1104X-E Oscilloscope, Saleae Logic Pro 8
