#include "LcdControl.h"

/* Arduino pins related mapping */
// LCD pins
const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
// buttons pins
const int turnLeftButton = 2;
const int turnRightButton = 3;
const int speakerPin = 8;

/* Game related settings */
int notes[] = {165, 208, 220, 277};
// LCD dimensions
const int rowsCount = 2;
const int columnsCount = 40;
// used to control delay between screen updates, so the speed of the game (ms)
unsigned screenUpdateInterval;
// timeout used to display "GAME OVER" message
const int gameTimeout = 1000;
// holds timestamp of last event in game (screen update, game over)
unsigned long lastStepTime = 0;
// car position on LCD screen
int carColumn;
volatile int carRow;
// bool flag used to control game flow
volatile bool isGameOver = true;
// used to determine if car row position was changed to produce tone
int prevCarRow = carRow;
// data array that stores LCD state to make required calculations for car collision etc.
int board[rowsCount][columnsCount];

// instance of LcdControl
LcdControl myLcd(rs, en, d4, d5, d6, d7);

// bit array used to create custom char to display a car
byte car[8] = {
        0b00000,
        0b00000,
        0b01010,
        0b11111,
        0b11111,
        0b01010,
        0b00000,
        0b00000
};

// interrupt service routine for left button
void onLeftButtonPress() {
    if (isGameOver) {
        prepareBoard();
    } else {
        carRow = 0;
    }
}

// interrupt service routine for right button
void onRightButtonPress() {
    if (isGameOver) {
        prepareBoard();
    } else {
        carRow = 1;
    }
}

void showGameOver() {
    isGameOver = true;
    lastStepTime = millis();
    // display message
    myLcd.clear();
    myLcd.selectPosition(0, 3);
    myLcd.write("** GAME **");
    myLcd.selectPosition(1, 4);
    myLcd.write("* OVER *");
    // play tone
    const int delayVal = 100;
    tone(speakerPin, notes[3], delayVal);
    delay(delayVal);
    tone(speakerPin, notes[2], delayVal);
    delay(delayVal);
    tone(speakerPin, notes[1], delayVal);
    delay(delayVal);
    tone(speakerPin, notes[0], delayVal);
}

void prepareBoard() {
    // delay makes UX better since it will prevent accidental button press that will skip game over or welcome message
    if (millis() - lastStepTime > gameTimeout) {
        myLcd.clear();
        // clear board data
        for (int r = 0; r < rowsCount; r++) {
            for (int c = 0; c < columnsCount; c++) {
                board[r][c] = 32;
            }
        }
        // reset game speed and car position
        screenUpdateInterval = 100;
        carColumn = 1;
        carRow = 0;
        isGameOver = false;
    }
}

void setup() {
    pinMode(turnLeftButton, INPUT_PULLUP);
    pinMode(turnRightButton, INPUT_PULLUP);
    pinMode(speakerPin, OUTPUT);

    // for better randomization, uses analog noise to select sequence
    randomSeed(analogRead(A0));
    // create car bitmap in LCD character generator RAM
    myLcd.createChar(0, car);

    // welcome message
    myLcd.selectPosition(0, 5);
    myLcd.write("PRESS");
    myLcd.selectPosition(1, 4);
    myLcd.write("ANY KEY");

    // attach button related interrupt service routines
    attachInterrupt(digitalPinToInterrupt(turnLeftButton), onLeftButtonPress, CHANGE);
    attachInterrupt(digitalPinToInterrupt(turnRightButton), onRightButtonPress, CHANGE);
}

void loop() {
    if (!isGameOver) {
        // ensure that enough time has passed to render next step
        if ((millis() - lastStepTime) > screenUpdateInterval) {
            lastStepTime = millis();
            // shift entire LCD content to the left, this is how the car "moves"
            myLcd.shiftLeft();

            // check if car changed its row and play tone
            // this cannot be done in interrupt service routines since tone library also relays on ISR
            if (prevCarRow != carRow) {
                prevCarRow = carRow;
                tone(speakerPin, notes[3] * 2, 1);
            }

            // calculate position of the column that was shifted to the left (now it is offscreen) and update it with "empty" values (32 is ASCII code for a space)
            // or add an obstacle based on simple random logic
            // calculation is based on a fact that screen shift if cyclic and its columns count is 40
            int columnToUpdate = (columnsCount + carColumn - 1) % columnsCount;
            // randomly select row which will contain obstacle
            int rowWithObstacle = random(100) > 50 ? 0 : 1;
            int obstacle = (carColumn % 5) || (random(100) > 50) ? 32 : 42;
            // update target column
            for (int row = 0; row < rowsCount; row++) {
                board[row][columnToUpdate] = rowWithObstacle == row ? obstacle : 32;
                myLcd.selectPosition(row, columnToUpdate);
                myLcd.write(board[row][columnToUpdate]);
            }

            // check that car position is not taken by obstacle
            if (board[carRow][carColumn] != 32) {
                showGameOver();
            } else {
                // draw car
                myLcd.selectPosition(carRow, carColumn);
                myLcd.write((byte) 0);
            }

            // calculate nex car column
            carColumn = (carColumn + 1) % columnsCount;
            // basic logic to decrease screenUpdateInterval so the game speed will be increased
            screenUpdateInterval =
                    carColumn == 39 && screenUpdateInterval > 50 ? screenUpdateInterval - 5 : screenUpdateInterval;
        }
    }
}
