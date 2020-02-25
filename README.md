![Crystal Ball](https://emojipedia-us.s3.dualstack.us-west-1.amazonaws.com/thumbs/240/joypixels/239/crystal-ball_1f52e.png)
# FORTUNE Calibrator

Arduino fortune teller, with real-world interaction metrics.

- 20x4 LCD display screen
- Content and usage data via Firebase
- Two arcade buttons
- Coin slot trigger
- Boot modes: free play, "no chrome" quick interactions

### [Pics in the wiki](https://github.com/doublejosh/FortuneTeller/wiki)

## Build Your Own
1. Clone the code and upload to Arduino.
1. Create a Firebase project with a database.
1. Rename `configs.txt` to `configs.h` and add credentials.
1. Wire display, buttons, and switch.
1. Create sample data, see structure in configs.

### [Simple Starter](https://github.com/doublejosh/FortuneExample)

## Hardware
- Wemos D1 Mini (Arduino)
- LCD Display 20x4
- Potentiometer, resister
- Arcade buttons
- Jumpers
- Breadboard

## Dependencies
- [Firebase-ESP8266](https://github.com/mobizt/Firebase-ESP8266) (2.8.0)
- [LiquidCrystal](https://www.arduino.cc/en/Reference/LiquidCrystal)
- [ESP8266WiFi](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi)
- [ArduinoJson](https://github.com/bblanchon/ArduinoJson)
