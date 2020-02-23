![Crystal Ball](https://emojipedia-us.s3.dualstack.us-west-1.amazonaws.com/thumbs/240/joypixels/239/crystal-ball_1f52e.png)
# FORTUNE Calibrator

Arduino fortune teller, with real-world interaction metrics.

- 20x4 LCD display screen
- Content and usage data via Firebase
- Two arcade buttons
- Coin slot trigger

## Build Your Own
1. Clone the code and upload to Arduino.
1. Create a Firebase project with a database.
1. Rename `configs.txt` to `configs.h` and add credentials.
1. Wire display, buttons, and switch.
1. Create sample data.
1. Update `FIREBASE_RESPONSE_SIZE` in `Firebase_ESP8266_Client`.

## Hardware
- Wemos D1 Mini (Arduino)
- LCD Display 20x4
- Potentiometer, resister
- Arcade buttons
- Jumpers
- Breadboard

### [Simple Starter](https://github.com/doublejosh/FortuneExample)
