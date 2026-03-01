# 🤖 Line Maze Solver

An autonomous line-following robot that navigates and solves mazes using IR sensors and PID control, built with Arduino/C++.

---

## 📁 Project Structure

```
line-maze-solver/
├── main/               # Core maze-solving logic
├── 3-line-pid-follow/  # PID-based 3-sensor line following
├── ir-sensor-test/     # IR sensor calibration & testing
├── motor-test/         # Motor driver testing
└── demo/               # Demo sketches / example runs
```

---

## 🛠️ Components Used

| Component | Details |
|---|---|
| Microcontroller | Arduino Nano |
| IR Sensor Array | 5-channel analog IR sensor array |
| Motor Driver | TB6612FNG dual motor driver |
| Motors | 2× N20 metal gear motor (6V, 1000 RPM) |
| Battery | 2× 3.7V Li-ion cells (in series → ~7.4V) |
| Buck Converter | DC-DC buck module set to **5V** (powers Arduino Nano) |

---
*Built by [bishal-777](https://github.com/bishal-777)*