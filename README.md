## Project is based on [Arduino Starter Kit](https://store.arduino.cc/products/arduino-starter-kit-multi-language) parts and its 16 x 2 [LCD screen](https://wiki-content.arduino.cc/documents/datasheets/LCDscreen.PDF)
You will need:
- Arduino board (UNO etc.)
- LCD screen (16 x 2)
- 2 pushbuttons
- resistor (220 Ohms)
- piezo capsule (optional)
- potentiometer (10kOhms) to adjust screen contrast (optional, ground V0 pin for max contrast)

Communication with LCD is based on 4-bit lightweight library that is included in the repo.

## Wiring
LCD:
- VSS to ground
- VDD to 5V
- V0 to potentiometer signal leg or to the ground
- RS to digital 12
- R/W to the ground (0 for write)
- E to digital 11
- DB4 to digital 7
- DB5 to digital 6
- DB6 to digital 5
- DB7 to digital 4
- LED+ to 5V through 220 Ohms resistor
- LED- to the ground

Buttons:
- left button to digital 2 and to the ground
- right button to digital 3 and to the ground
- piezo to digital 8 and to the ground
- potentiometer to 5V, ground and V0 (mentioned above)

wokwi link to wiring: https://wokwi.com/projects/378699721443912705

<img width="500" alt="Screenshot 2023-10-16 at 03 29 47" src="https://github.com/jemcik/arduino-car-game/assets/4085099/c714462b-7dd4-4b48-9f79-d3ef0ba1bed9">




