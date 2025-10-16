# bcu-firmware
This repository contains the firmware for the BCU inside each agent. The BCU is implemented as an ardruino UNO, which incorporates the following subsystems as part of the firmware:
- Safety Monitoring
- Collision Avoidance
- Messaging with Main Compute
- Actuation Control

## Download & Compiling
Note that this code requires a physical arduino to run. Schematics for the BCU can be found <TODO>.

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
