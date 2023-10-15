#ifndef LcdControl_h
#define LcdControl_h

#include "Print.h"
#include "Arduino.h"

class LcdControl : public Print {
public:
    LcdControl(uint8_t rs, uint8_t enable, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

    void clear();

    void shiftLeft();

    void selectPosition(uint8_t row, uint8_t col);

    void createChar(uint8_t location, uint8_t charmap[]);

    size_t write(uint8_t);

    using Print::write;
private:
    void sendByte(uint8_t value);

    void sendNibble(uint8_t value);

    void command(uint8_t value);

    void processSignal();

    uint8_t _rs_pin;
    uint8_t _enable_pin;
    uint8_t _data_pins[4];
};

#endif