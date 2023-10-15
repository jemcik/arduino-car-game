#include "Arduino.h"
#include "LcdControl.h"

LcdControl::LcdControl(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3) {
    _rs_pin = rs;
    _enable_pin = enable;
    _data_pins[0] = d0;
    _data_pins[1] = d1;
    _data_pins[2] = d2;
    _data_pins[3] = d3;

    // set pins
    pinMode(_rs_pin, OUTPUT);      // signal for selecting registers: write data to screen (1) or send command (0)
    pinMode(_enable_pin, OUTPUT);  // signal for LCD to read data from d0-d4 and _rs_pin pins and execute command
    for (int i = 0; i < 4; i++) {
        pinMode(_data_pins[i], OUTPUT);  // pins that are the source of binary data
    }

    delayMicroseconds(50000);
    digitalWrite(_rs_pin, LOW);
    digitalWrite(_enable_pin, LOW);

    // set 4-bit mode, this sequence is described in datasheet
    sendNibble(0b0011);
    delayMicroseconds(4500);
    sendNibble(0b0011);
    delayMicroseconds(4500);
    sendNibble(0b0011);
    delayMicroseconds(150);
    sendNibble(0b0010);

    // set number of lines + font
    sendByte(0b00101000);

    // display settings (display is on, no cursor, no blinking)
    sendByte(0b00001100);

    // entry mode
    sendByte(0b00000110);

    // clear display
    clear();
}

void LcdControl::sendNibble(uint8_t value) {
    for (int i = 0; i < 4; i++) {
        digitalWrite(_data_pins[i], (value >> i) & 0b1);
    }
    processSignal();
}

void LcdControl::sendByte(uint8_t value) {
    sendNibble(value >> 4);
    sendNibble(value);
}

void LcdControl::processSignal() {
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(1);
    digitalWrite(_enable_pin, HIGH);
    delayMicroseconds(1);
    digitalWrite(_enable_pin, LOW);
    delayMicroseconds(100);
}

// overloads Print.h parent method
size_t LcdControl::write(uint8_t value) {
    digitalWrite(_rs_pin, HIGH);
    sendByte(value);
    return 1;
}

void LcdControl::command(uint8_t value) {
    digitalWrite(_rs_pin, LOW);
    sendByte(value);
}

void LcdControl::selectPosition(uint8_t row, uint8_t col) {
    uint8_t adress = (row == 1 ? 0b1000000 : 0x0) + col;
    command(0b10000000 | adress);
}

void LcdControl::createChar(uint8_t location, uint8_t charmap[]) {
    command(0b1000000 | ((location & 0b111) << 3));

    for (int i = 0; i < 8; i++) {
        write(charmap[i]);
    }
}

void LcdControl::clear() {
    command(0b1);
    delayMicroseconds(2000);
}

void LcdControl::shiftLeft() {
    command(0b11000);
}
