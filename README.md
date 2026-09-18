# MT-Audio

A custom-built, **ESP32-powered car head unit** - a full replacement car stereo with FM radio (RDS), Bluetooth, USB, SD-card, and Aux inputs, a hardware EQ, and a real-time clock, all driven by custom firmware and mounted on a custom PCB.

## Overview

MT-Audio is a from-scratch car audio system combining custom hardware and firmware:

- **FM Radio** with RDS (station name/text) decoding and automatic best-station scanning
- **Bluetooth / USB / SD-card playback**, powered by my own [BT201 Arduino library](https://github.com/MichalisTamiolakis/BT201) for the BT201/KT1025 audio module
- **Aux input**
- **Hardware EQ** - bass, treble, balance, fade, and loudness, controlled via a TDA7313 audio processor IC
- **Ignition sensing** - automatically powers on/off and remembers state across power cycles, just like a factory head unit
- **Real-time clock** (DS3231) with a full date/time display and set mode
- **Ambient light sensing** for automatic LCD backlight dimming
- **Ambient temperature sensing** - based on the OEM NTC thermistor
- **Station presets** - save and recall up to 6 stations per band, with an auto-store "best stations" scan
- **Bluetooth phone call handling** - answer, reject, hang up, and caller ID display, straight on the head unit's screen
- **OTA firmware updates**
- Custom multiplexed button matrix + LCD-based UI, styled after a real car stereo's controls

## Hardware

- **MCU**: ESP32 (esp32doit-devkit-v1)
- **Audio processor**: TDA7313 (I²C-controlled EQ/input switching)
- **FM tuner**: SI4703, with RDS parsing
- **RTC**: DS3231
- **Bluetooth/USB/SD audio module**: BT201/KT1025
- **Display**: I²C LCD (LiquidCrystal_I2C)
- Ambient light and temperature sensors, ignition sense input, backlight PWM control

PCB schematics are included in the [`Schematics`](./Schematics) folder.

## Firmware

Located under [`Code/MTAudio`](./Code/MTAudio), built with **PlatformIO** on the Arduino framework for ESP32.

### Dependencies
- [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C)
- [RTClib](https://github.com/adafruit/RTClib) (Adafruit)
- [BT201](https://github.com/MichalisTamiolakis/BT201) - my own library for the BT201/KT1025 module
- SI4703 / RDS parsing libraries

### Building

1. Install [PlatformIO](https://platformio.org/) (e.g. via the VS Code extension).
2. Open the [`Code/MTAudio`](./Code/MTAudio) folder as a PlatformIO project.
3. Build/upload one of the two environments:
   - `esp32doit-devkit-v1-debug` - debug build with serial logging enabled
   - `esp32doit-devkit-v1-release` - release build
4. Flash to an ESP32 wired according to the schematics in [`Schematics`](./Schematics).

## Architecture

The firmware is built around a central `System` class implementing a state machine (`SystemState`: Off / On / Standby / Update) and a mode machine (`SystemMode`: Idle, Volume, Bass, Balance, Treble, Fade, Loudness, station save/select, date/time set, and more) that together drive what's shown on the LCD and how button input is interpreted - closely mirroring how a real factory head unit behaves, including remembering the pre-ignition-off state and resuming it when the ignition turns back on.

## License

Distributed under the MIT License. See [`LICENSE`](./LICENSE) for details.
