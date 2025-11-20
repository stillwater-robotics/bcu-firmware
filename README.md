# bcu-firmware
This repository contains the firmware for the BCU inside each agent. The BCU is implemented as an Ardruino UNO, which incorporates the following subsystems as part of the firmware:
- Safety Monitoring
- Collision Avoidance
- Messaging with Main Compute
- Actuation Control

## Included Software
- bcu_firmware/bcu_firmware.ino is the actual firmware for the BCU
  - Must be compiled with Arduino IDE
- test_messager/test_messager.ino is companion firmware used in the bench test setup
  - Must be compiled with Arduino IDE
- bcu_simulation.c runs the firmware controlling a simulated robot setup.
  - Requires SDL2
- unittest.c runs unit tests on the firmware.

## Download & Compilation
Note that this BCU code requires a physical Arduino to run. Schematics for a bench setup and the final robot can be found <TODO>.

If you do not have hardware, you can run a simulated version in the next section.

1. Download or clone the git repo as normal
```bash
cd <desired workspace>
git clone https://github.com/stillwater-robotics/bcu-firmware.git
```

2. Prepare submodules to get BICA
```bash
cd bcu-firmware
git submodule init
git submodule update
```

3. Download Arduino IDE
Official Site: https://www.arduino.cc/en/software

4. Open the .ino files in Arduino IDE

5. Select Board & Port
For boards, `test_messenger.ino` is intended for an Arduino Nano and `bcu_firmware.ino` intended for an Arduino Uno.

6. Compile & Upload
Use the built-in 'verify' and 'upload' buttons located in the menu bar of Arduino IDE.

## Simulation
