// The state machine has a lot of different constants and variables which are organized here
// Many of these contants are for LCD formatting

// I ran out of time so not all the display formatting is shown 

#ifndef SCANNER_STATE_MACHINE_CONSTANTS
#define SCANNER_STATE_MACHINE_CONSTANTS

#define DISPLAY_BB 1 // option to display bounding box to LCD
#define DISPLAY_FACE_STATUS 1 // option to display text to LCD ("Face Detected")

#define FACE_PRESENT 0
#define NO_FACE_PRESENT 1

#define LCD_W 240
#define LCD_H 320

#define LCD_TEXT_BUFF_SIZE 32

// white text at the top of the screen
#define STATE_TEXT_X 0
#define STATE_TEXT_Y 35
#define STATE_TEXT_W 240
#define STATE_TEXT_H 20

// colored text at the top of the screen
#define STATE_TEXT_DESC_X 0
#define STATE_TEXT_DESC_Y 15
#define STATE_TEXT_DESC_W 240
#define STATE_TEXT_DESC_H 20

// expiration periods for each state in seconds
#define SEARCH_PERIOD 5
#define POSITIONING_PERIOD 7
#define MEASUREMENT_PERIOD 8
#define MEASUREMENT_HOLD_TIME 3 // time that user needs to hold still for meaurement

#define INFO_TEXT_H 20

#define TIME_BAR_H 2

#define IDLE_TEXT_W 35
#define IDLE_TEXT_H 20

#define POS_TEXT_W 130
#define POS_TEXT_H 30
#define POS_TEXT_X 60
#define POS_TEXT_Y 240

// ideal center rectangle for user reference
#define IDEAL_X 80
#define IDEAL_Y 90
#define IDEAL_W 80
#define IDEAL_H 100
#define IDEAL_BB_LINE_W 1

#define TEMP_LIMIT 99.5 // 'fever'

#define IDLE_TEXT_PERIOD 100000 // 'loops'

#define CENTERING_THRESHOLD 10

#endif