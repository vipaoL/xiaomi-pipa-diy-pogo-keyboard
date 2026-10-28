# DIY pogo keyboard for Xiaomi Pad 6
## Current state
- Hardware and firmware are completed
- Case is unfinished
- Android is not supported, only mainline Linux is tested. Stock MIUI/HyperOS most likely does some keyboard authorization. The keyboard may work with custom Android ROMs, but it is not tested
## How does it work
Pogo pinout (pin 1 is closest to the edge):
1. GND
2. Data (1.8V logic level, half-duplex UART)
3. VCC (3-4.2V from the battery)
As the logic level is 1.8V, the only right way is to use an MCU that uses the same voltage.

The tablet tests the keyboard before starting to work with it:  
1. If a magnet is placed near the hall sensor near the camera, the tablet will begin checking the data pin.
The resistance between the data and ground pins is expected to be approximately 50-800 kOhm.
Actually, it is most likely a voltage drop that is being checked. However, if the microcontroller pin is high-Z and a resistor is connected to it, then it works.
2. If the resistance is ok, the tablet will turn on the power on the VCC pin.
3. After this, the keyboard should then send a message. If the tablet doesn't receive anything, it will turn the power off and try again.
## Hardware
- MCU: STM32L051C8T6 soldered onto "blue pill" board instead of STM32F103
- LDO: MCP1700T-1802E
- Keyboard from Asus EeePC 701
- Synaptics touchpad from an unknown laptop
- SSD1306 128x64 OLED display for displaying debug info

Power path: VCC -> LDO -> MCU
## Firmware
The firmware is based on Arduino Framework. Apply `PS2Mouse.patch`, use PlatformIO to build the firmware.
## Case
Designed in OpenSCAD

<img width="1086" height="1036" alt="image" src="https://github.com/user-attachments/assets/8bfb5ae5-e659-44a7-aad6-c2d74c9cfbf0" />
